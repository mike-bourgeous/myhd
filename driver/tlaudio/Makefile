SMPCFLAGS=`uname -a | grep -i SMP >/dev/null 2>&1 && echo "-D__module__smp -DSMP" || echo ""`
MODCFLAGS=-Wall -DMODULE -O2 -D__KERNEL__ -DLINUX -I/lib/modules/`uname -r`/build/include -I../tlaudio/
CC=gcc $(MODCFLAGS) $(SMPCFLAGS)
OBJS=msp3400.o

driver:		$(OBJS)
	ld -m elf_i386 -r -o tlmsp3400.o $(OBJS)

clean:
	rm -f *.o

