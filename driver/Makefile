SMPCFLAGS=`uname -a | grep -i SMP >/dev/null 2>&1 && echo "-D__module__smp -DSMP" || echo ""`
MODCFLAGS=-Wall -DMODULE -O2 -D__KERNEL__ -DLINUX -I/lib/modules/`uname -r`/build/include
CC=gcc $(MODCFLAGS) $(SMPCFLAGS)
OBJS=tl880kern.o tl880reg.o tl880util.o tl880i2c.o tl880init.o tl880gpio.o \
      tl880int.o tl880vip.o tl880input.o tl880audio.o tl880dpc.o tl880demux.o \
      tl880dma.o tl880modes.o tl880cursor.o tl880osd.o

driver:		$(OBJS)
	ld -m elf_i386 -r -o tl880.o $(OBJS)

clean:
	rm -f *.o

