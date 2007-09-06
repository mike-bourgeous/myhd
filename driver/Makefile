MODULE_NAME = tl880
$(MODULE_NAME)-objs = tl880kern.o tl880reg.o tl880util.o tl880i2c.o tl880init.o tl880gpio.o \
	tl880int.o tl880vip.o tl880input.o tl880audio.o tl880dpc.o tl880demux.o \
	tl880dma.o tl880modes.o tl880cursor.o tl880osd.o tl880proc.o tl880mem.o tl880vpx.o \
	tl880msp.o genalloc.o

RM	= @rm -rf
JUNK	= *~ *.bak

# First pass, kernel Makefile reads module objects
ifneq ($(KERNELRELEASE),)
obj-m := $(MODULE_NAME).o

# EXTRA_CFLAGS += -I$(PWD)/../include # for extra include dir if needed
EXTRA_CFLAGS += -Wall

# Second pass, the actual build.
else
KVER	?= $(shell uname -r)
KDIR    := $(shell test -d /lib/modules/$(KVER)/build && echo /lib/modules/$(KVER)/build || echo /lib/modules/$(KVER)/source)
PWD     := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	chmod +x tl880_mkdev tl880_mkdev_static tl880_rmdev tl880_load

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

distclean: clean
	$(RM) $(JUNK) $(OBJS)

help:
	$(MAKE) -C $(KDIR) M=$(PWD) help

install:
	$(MAKE) -C $(KDIR) M=$(PWD) modules_install

endif

