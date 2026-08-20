obj-m := hello_module.o

KDIR ?= /lib/modules/$(shell uname -r)/build

PWD := $(shell pwd)

.PHONY: all clean load unload

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
load:
	sudo insmod hello_module.ko
unload:
	sudo rmmod hello_module