MODULE_NAME := way-part-control
KERNEL_SRC := /lib/modules/$(shell uname -r)/build
PWD := $(CURDIR)
obj-m += $(MODULE_NAME).o

EXTRA_CFLAGS += -march=armv8-a
driver: $(MODULE_NAME).ko

$(MODULE_NAME).ko : $(MODULE_NAME).o 
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules


$(MODULE_NAME).o : $(MODULE_NAME).c
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules


driver : $(MODULE_NAME).ko utest

clean:
	rm ./wpuser-control ./$(MODULE_NAME).mod ./$(MODULE_NAME).ko ./Module.symvers ./modules.order ./$(MODULE_NAME).mod.o

install:
	sudo insmod ./$(MODULE_NAME).ko

utest:
	gcc -o wpuser-control wpuser-control.c -pthread
	chmod +x wpuser-control
