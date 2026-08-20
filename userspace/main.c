/*
Userspace setting parameters program
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#define MODULE_NAME "hello_module"
#define MIN_INTERVAL 1
#define MAX_INTERVAL 86400

/*
Writing kernel module parameter value via sysfs
*/
static int write_sysfs_param(const char *param_name, const char *value)
{
    char path[PATH_MAX];
    FILE *file;

    /*Initializing path to parameter*/
    if (snprintf(path, sizeof(path), "/sys/module/" MODULE_NAME "/parameters/%s", param_name)
        >= (int)sizeof(path)) {
        fprintf(stderr, "Error: parameter path is too big\n");
        
        return -1;
    }

    file = fopen(path, "w");

    if (file == NULL) {
        fprintf(stderr, "Error: cannot open %s: %s\n", path, strerror(errno));

        return -1;
    }

    if (fprintf(file, "%s", value) < 0) {
        fprintf(stderr, "Error: cannot write to %s: %s\n", path, strerror(errno));
        fclose(file);

        return -1;
    }

    fclose(file);

    return 0;
}

/*
Validation of possible interval_sec parameter values
*/
static int validate_interval(const char *value) {
    char *end;
    long interval;

    errno = 0;
    
    interval = strtol(value, &end, 10);
    
    /*Checks for not-a-number values and errors*/
    if (errno != 0 || *value == '\0' || *end != '\0') {
        return -1;
    }

    if (interval < MIN_INTERVAL || interval > MAX_INTERVAL) {
        return -1;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filepath> <interval_sec (%d-%d)>\n", argv[0], 
            MIN_INTERVAL, MAX_INTERVAL);

        return EXIT_FAILURE;
    }

    if (validate_interval(argv[2]) != 0) {
        fprintf(stderr, "Error: interval must be in range %d-%d\n", 
            MIN_INTERVAL, MAX_INTERVAL);

        return EXIT_FAILURE;
    }

    if (write_sysfs_param("filepath", argv[1]) != 0) return EXIT_FAILURE;
    if (write_sysfs_param("interval_sec", argv[2]) != 0) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}