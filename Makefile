obj-m += HD44780_driver.o
HD44780_driver-objs += driver_main.o file_operations.o
ccflags-y := -std=gnu99 -O2 -Wno-declaration-after-statement

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
