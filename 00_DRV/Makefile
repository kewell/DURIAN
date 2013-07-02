
obj-m := drv_skeleton.o
obj-m := force_panic.o
#KERNELDIR := /usr/L300/software/linux-2.6.12     //The kernel catalog is you compiled for the arm
KERNELDIR := /HYRD/wenjing/IDVR_firmware/kernel_v690

ARCH=arm
CROSS_COMPILE=arm-hisiv100-linux-

PWD := $(shell pwd)

modules:
	$(MAKE) ARCH=arm CROSS_COMPILE=arm-hisiv100-linux- -C $(KERNELDIR) M=$(PWD) modules
	'cp' *.ko /tmp

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions ?odule*
