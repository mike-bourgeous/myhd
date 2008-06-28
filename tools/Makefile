# Build the MyHD tools

CC = gcc
CFLAGS=-O0 -ggdb3
LOCAL_CFLAGS = -std=gnu99 -Wall -I../driver/
COMPILE = ${CC} ${LOCAL_CFLAGS} ${CFLAGS}

all: scripts striperam setalpha rampram regload regdump memdump memload memloadswab \
	memset readmem writemem readreg writereg setbits write4reg randreg iocsetvip \
	iocreadreg iocwritereg iocsetgpio cursor rgb2ypbpr ioccursor randosd osdtest \
	osdimage hexcount ioctl channel randram regloadslow iocread4reg

	make -C dpc

randram: randram.c
	${COMPILE} randram.c -o randram

striperam: striperam.c
	${COMPILE} striperam.c -o striperam

setalpha: setalpha.c
	${COMPILE} setalpha.c -o setalpha

rampram: rampram.c
	${COMPILE} rampram.c -o rampram

regload: regload.c
	${COMPILE} regload.c -o regload

regdump: regdump.c
	${COMPILE} regdump.c -o regdump

memdump: memdump.c
	${COMPILE} memdump.c -o memdump

memload: memload.c
	${COMPILE} memload.c -o memload

memloadswab: memloadswab.c
	${COMPILE} memloadswab.c -o memloadswab

memset: memset.c
	${COMPILE} memset.c -o memset
	
readmem: readmem.c
	${COMPILE} readmem.c -o readmem

writemem: writemem.c
	${COMPILE} writemem.c -o writemem

readreg: readreg.c
	${COMPILE} readreg.c -o readreg

writereg: writereg.c
	${COMPILE} writereg.c -o writereg

setbits: setbits.c
	${COMPILE} setbits.c -o setbits

write4reg: write4reg.c
	${COMPILE} write4reg.c -o write4reg
	
randreg: randreg.c
	${COMPILE} randreg.c -o randreg

iocsetvip: iocsetvip.c
	${COMPILE} iocsetvip.c -o iocsetvip

iocreadreg: iocreadreg.c
	${COMPILE} iocreadreg.c -o iocreadreg

iocwritereg: iocwritereg.c
	${COMPILE} iocwritereg.c -o iocwritereg

iocsetgpio: iocsetgpio.c
	${COMPILE} iocsetgpio.c -o iocsetgpio

cursor: cursor.c
	${COMPILE} cursor.c -o cursor

rgb2ypbpr: rgb2ypbpr.c
	${COMPILE} rgb2ypbpr.c -o rgb2ypbpr

ioccursor: ioccursor.c
	${COMPILE} ioccursor.c -o ioccursor

randosd: randosd.c
	${COMPILE} randosd.c -o randosd

osdtest: osdtest.c
	${COMPILE} osdtest.c -o osdtest

osdimage: osdimage.c
	${COMPILE} osdimage.c -o osdimage

hexcount: hexcount.c
	${COMPILE} hexcount.c -o hexcount

ioctl: ioctl.c
	${COMPILE} ioctl.c -o ioctl

channel: channel.c
	${COMPILE} channel.c -o channel

regloadslow: regloadslow.c
	${COMPILE} regloadslow.c -o regloadslow

iocread4reg: iocread4reg.c
	${COMPILE} iocread4reg.c -o iocread4reg



scripts:
	chmod +x testdpcint.sh dpc2dump.sh dpc2rand.sh osd.sh \
		testioctl.sh

clean:
	rm -f randram regload regdump readreg setbits write4reg \
	writereg iocsetvip iocreadreg iocwritereg cursor rgb2ypbpr \
	striperam ioccursor randosd osdtest memdump osdimage \
	memload rampram hexcount readmem writemem randreg setalpha \
	memloadswab memset iocsetgpio ioctl channel regloadslow

#	make -C dpc clean
#	make -C example clean
#	make -C firmware clean


