SMPCFLAGS=`uname -a | grep -i SMP >/dev/null 2>&1 && echo "-D__module__smp -DSMP" || echo ""`
MODCFLAGS=-Wall -DMODULE -O2 -D__KERNEL__ -DLINUX -I/lib/modules/`uname -r`/build/include
CC=gcc $(MODCFLAGS) $(SMPCFLAGS)
NOBJS=tl880kern.o tl880reg.o tl880util.o tl880ni2c.o tl880init.o tl880gpio.o tl880int.o \
      tl880vip.o tl880input.o tl880audio.o
OBJS=tl880kern.o tl880reg.o tl880util.o tl880i2c.o tl880init.o tl880gpio.o tl880int.o \
     tl880vip.o tl880input.o tl880audio.o

oldi2c:		$(OBJS)
	ld -m elf_i386 -r -o tl880.o $(OBJS)

newi2c:		$(NOBJS)
	ld -m elf_i386 -r -o tl880.o $(NOBJS)

clean:
	rm -f *.o

