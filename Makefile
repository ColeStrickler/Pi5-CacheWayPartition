MODULE_NAME := test_driver
KERNEL_SRC := /lib/modules/$(shell uname -r)/build
PWD := $(CURDIR)
obj-m += test_driver.o

EXTRA_CFLAGS += -march=armv8-a
driver: $(MODULE_NAME).ko

$(MODULE_NAME).ko : $(MODULE_NAME).o 
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules


$(MODULE_NAME).o : $(MODULE_NAME).c
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules


driver : $(MODULE_NAME).ko utest

clean:
	rm ./utest ./test_driver.mod ./test_driver.ko ./Module.symvers ./modules.order ./test_driver.mod.o

install:
	sudo insmod ./test_driver.ko

utest:
	gcc -o utest utest.c -pthread
	chmod +x utest
