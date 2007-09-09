MODULE_NAME = snd-tl880
$(MODULE_NAME)-objs = tl880alsa.o

RM	= @rm -rf
JUNK	= *~ *.bak

# First pass, kernel Makefile reads module objects
ifneq ($(KERNELRELEASE),)
obj-m := $(MODULE_NAME).o

# EXTRA_CFLAGS += -I$(PWD)/../include # for extra include dir if needed
EXTRA_CFLAGS += -Wall -Werror -I$(PWD)/../

# Second pass, the actual build.
else
KVER	?= $(shell uname -r)
KDIR    := $(shell test -d /lib/modules/$(KVER)/build && echo /lib/modules/$(KVER)/build || echo /lib/modules/$(KVER)/source)
PWD     := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

distclean: clean
	$(RM) $(JUNK) $(OBJS)

help:
	$(MAKE) -C $(KDIR) M=$(PWD) help

install:
	$(MAKE) -C $(KDIR) M=$(PWD) modules_install

endif

