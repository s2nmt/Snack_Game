TOOLCHAIN:= /home/s2nmt/work/beagle_bone_black/bb-kernel/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-
BBB_KERNEL:= /home/s2nmt/work/beagle_bone_black/bb-kernel/KERNEL/

EXTRA_CFLAGS=-Wall
obj-m := ssd1306_oled_driver.o
ssd1306_oled_driver-objs = ssd1306_lib.o ssd1306_driver.o

all:
	make ARCH=arm CROSS_COMPILE=$(TOOLCHAIN) -C $(BBB_KERNEL) M=$(shell pwd) modules
	
clean:
	make -C $(BBB_KERNEL) M=$(shell pwd) clean
