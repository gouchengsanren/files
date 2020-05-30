
CC := arm-linux-gnueabihf-gcc
PWD := $(shell echo `pwd`)
KERNEL_SOURCE := $(PWD)/../
NFS := /home/book/nfs_rootfs/

obj-m := button.o

all: release install

release:
	make -C $(KERNEL_SOURCE) M=$(PWD) modules
	$(CC) -o app signal.c

install:
	cp -a *.ko app $(NFS)

clean:
	make -C $(KERNEL_SOURCE) M=$(PWD) modules clean
	rm app

