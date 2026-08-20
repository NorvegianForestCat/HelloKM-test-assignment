#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/workqueue.h>
#include <linux/mutex.h>
#include <linux/string.h>
#include <linux/limits.h>

#define DEFAULT_filepath "/tmp/hello_module_output.txt"
#define MIN_INTERVAL_SEC 1
#define MAX_INTERVAL_SEC 86400
#define DEFAULT_INTERVAL_SEC 5

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hello Kernel Module test assignment");
MODULE_VERSION("1.0");

static char filepath[PATH_MAX] = DEFAULT_filepath;
static unsigned int interval_sec = DEFAULT_INTERVAL_SEC;
static DEFINE_MUTEX(params_lock);
static struct timer_list hello_timer;
static struct work_struct work;

static const char hello_message[] = "Hello from kernel module\n";

static int set_filepath(const char *path, const struct kernel_param *kp)
{
    int ret;

    mutex_lock(&params_lock);
    ret = strscpy(filepath, path, sizeof(filepath));
    mutex_unlock(&params_lock);
    
    if (ret < 0) {
        printk(KERN_ERR "hello_module: strscpy failed\n");
        return ret;
    }

    return 0;
}

static int set_interval(const char *val, const struct kernel_param *kp)
{
    unsigned int interval;
    
    int ret = kstrtouint(val, 10, &interval);
    
    if (ret != 0) {
        return ret;
    }

    if (interval < MIN_INTERVAL_SEC || interval > MAX_INTERVAL_SEC) {
        printk(KERN_ERR "Interval must be between %d and %d seconds\n", MIN_INTERVAL_SEC, MAX_INTERVAL_SEC);
        return -EINVAL;
    }

    mutex_lock(&params_lock);
    interval_sec = interval;
    mutex_unlock(&params_lock);

    return 0;
}

static const struct kernel_param_ops filepath_ops = {
    .set = set_filepath,
    .get = param_get_string,
};

static const struct kernel_param_ops interval_ops = {
    .set = set_interval,
    .get = param_get_uint,
};

static struct kparam_string filepath_param = {
    .maxlen = PATH_MAX,
    .string = filepath,
};

module_param_cb(filepath, &filepath_ops, &filepath_param, 0644);
module_param_cb(interval_sec, &interval_ops, &interval_sec, 0644);

MODULE_PARM_DESC(filepath, "Path to the output file (default: /tmp/hello_module_output.txt)");
MODULE_PARM_DESC(interval_sec, "Interval in seconds for writing to the file (default: 5, 1-86400)");

static void write_hello_msg_hdlr(struct work_struct *work)
{
    struct file *file;
    loff_t pos = 0;
    ssize_t bytes_written;
    size_t msg_len = sizeof(hello_message) - 1;
    /*kmalloc used for stack overflow protection*/
    char *path_copy = kmalloc(PATH_MAX, GFP_KERNEL);

    if (!path_copy) {
        printk(KERN_ERR "hello_module: kmalloc failed\n");
        return;
    }

    mutex_lock(&params_lock);
    strscpy(path_copy, filepath, PATH_MAX);
    mutex_unlock(&params_lock);

    file = filp_open(path_copy, O_WRONLY | O_CREAT | O_APPEND, 0644);
    
    if (IS_ERR(file)) {
        printk(KERN_ERR "hello_module: cannot open file '%s': %ld\n", path_copy, PTR_ERR(file));

        kfree(path_copy);
        return;
    }

    bytes_written = kernel_write(file, hello_message, msg_len, &pos);

    if (bytes_written < 0) {
        printk(KERN_ERR "hello_module: kernel_write() failed: %zd\n", bytes_written);
    } else if (bytes_written != msg_len) {
        printk(KERN_WARNING "hello_module: short write %zd/%zu\n", bytes_written, msg_len);
    }
    
    filp_close(file, NULL);
    kfree(path_copy);
}

static void timer_callback(struct timer_list *timer) 
{
    unsigned int cur_interval;

    schedule_work(&work);

    mutex_lock(&params_lock);
    cur_interval = interval_sec;
    mutex_unlock(&params_lock);

    mod_timer(&hello_timer, jiffies + cur_interval*HZ);
}

static int __init hello_module_init(void)
{
    mutex_lock(&params_lock);
    if (interval_sec > MAX_INTERVAL_SEC || interval_sec < MIN_INTERVAL_SEC) {
        printk(KERN_ERR "Interval must be between %d and %d seconds\n", MIN_INTERVAL_SEC, MAX_INTERVAL_SEC);
        mutex_unlock(&params_lock);

        return -EINVAL;
    }
    mutex_unlock(&params_lock);

    printk(KERN_INFO "hello_module: loaded, filepath='%s', interval_sec=%u\n", filepath, interval_sec);

    INIT_WORK(&work, write_hello_msg_hdlr);
    timer_setup(&hello_timer, timer_callback, 0);

    mod_timer(&hello_timer, jiffies + interval_sec * HZ);

    return 0;
}

static void __exit hello_module_exit(void)
{
    /*destructors waiting for end of timer callback and work handler to prevent oops*/
    del_timer_sync(&hello_timer);
    cancel_work_sync(&work);
    printk(KERN_INFO "hello_module: unloaded\n");
}

module_init(hello_module_init);
module_exit(hello_module_exit);