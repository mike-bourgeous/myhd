all: randram striperam setalpha rampram regload regdump memdump memload memloadswab \
	memset readmem writemem readreg writereg setbits write4reg randreg iocsetvip \
	iocreadreg iocwritereg iocsetgpio cursor rgb2ypbpr ioccursor randosd osdtest \
	osdimage hexcount ioctl channel scripts

randram: randram.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall randram.c -o randram -ggdb3

striperam: striperam.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall striperam.c -o striperam -ggdb3

setalpha: setalpha.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall setalpha.c -o setalpha -ggdb3

rampram: rampram.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall rampram.c -o rampram -ggdb3

regload: regload.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall regload.c -o regload -ggdb3

regdump: regdump.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall regdump.c -o regdump -ggdb3

memdump: memdump.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall memdump.c -o memdump -ggdb3

memload: memload.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall memload.c -o memload -ggdb3

memloadswab: memloadswab.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall memloadswab.c -o memloadswab -ggdb3

memset: memset.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall memset.c -o memset -ggdb3
	
readmem: readmem.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall readmem.c -o readmem -ggdb3

writemem: writemem.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall writemem.c -o writemem -ggdb3

readreg: readreg.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall readreg.c -o readreg -ggdb3

writereg: writereg.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall writereg.c -o writereg -ggdb3

setbits: setbits.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall setbits.c -o setbits -ggdb3

write4reg: write4reg.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall write4reg.c -o write4reg -ggdb3
	
randreg: randreg.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall randreg.c -o randreg -ggdb3

iocsetvip: iocsetvip.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall iocsetvip.c -o iocsetvip -ggdb3 -I../driver/

iocreadreg: iocreadreg.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall iocreadreg.c -o iocreadreg -ggdb3 -I../driver/

iocwritereg: iocwritereg.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall iocwritereg.c -o iocwritereg -ggdb3 -I../driver/

iocsetgpio: iocsetgpio.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall iocsetgpio.c -o iocsetgpio -ggdb3 -I../driver/

cursor: cursor.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall cursor.c -o cursor -ggdb3 -I../driver/

rgb2ypbpr: rgb2ypbpr.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall rgb2ypbpr.c -o rgb2ypbpr -ggdb3

ioccursor: ioccursor.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall ioccursor.c -o ioccursor -ggdb3 -I../driver/

randosd: randosd.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall randosd.c -o randosd -ggdb3 -I../driver/

osdtest: osdtest.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall osdtest.c -o osdtest -ggdb3 -I../driver/

osdimage: osdimage.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall osdimage.c -o osdimage -ggdb3 -I../driver/

hexcount: hexcount.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall hexcount.c -o hexcount -s

ioctl: ioctl.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall ioctl.c -o ioctl -ggdb3 -I../driver/

channel: channel.c
	gcc ${CFLAGS} -std=gnu99 -Os -Wall channel.c -o channel -ggdb3 -I../driver/

scripts:
	chmod +x testdpcint.sh dpc2dump.sh dpc2rand.sh osd.sh \
		testioctl.sh

clean:
	rm -f randram regload regdump readreg setbits write4reg \
	writereg iocsetvip iocreadreg iocwritereg cursor rgb2ypbpr \
	striperam ioccursor randosd osdtest memdump osdimage \
	memload rampram hexcount readmem writemem randreg setalpha \
	memloadswab memset iocsetgpio ioctl channel

#	make -C dpc clean
#	make -C example clean
#	make -C firmware clean


