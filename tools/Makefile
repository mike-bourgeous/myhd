all:
	gcc -std=gnu99 -Os -Wall randram.c -o randram -ggdb3
	gcc -std=gnu99 -Os -Wall striperam.c -o striperam -ggdb3
	gcc -std=gnu99 -Os -Wall setalpha.c -o setalpha -ggdb3
	gcc -std=gnu99 -Os -Wall rampram.c -o rampram -ggdb3
	gcc -std=gnu99 -Os -Wall regload.c -o regload -ggdb3
	gcc -std=gnu99 -Os -Wall regdump.c -o regdump -ggdb3
	gcc -std=gnu99 -Os -Wall memdump.c -o memdump -ggdb3
	gcc -std=gnu99 -Os -Wall memload.c -o memload -ggdb3
	gcc -std=gnu99 -Os -Wall memloadswab.c -o memloadswab -ggdb3
	
	gcc -std=gnu99 -Os -Wall memset.c -o memset -ggdb3
	
	gcc -std=gnu99 -Os -Wall readmem.c -o readmem -ggdb3
	gcc -std=gnu99 -Os -Wall writemem.c -o writemem -ggdb3
	
	gcc -std=gnu99 -Os -Wall readreg.c -o readreg -ggdb3
	gcc -std=gnu99 -Os -Wall writereg.c -o writereg -ggdb3
	gcc -std=gnu99 -Os -Wall setbits.c -o setbits -ggdb3
	gcc -std=gnu99 -Os -Wall write4reg.c -o write4reg -ggdb3
	
	gcc -std=gnu99 -Os -Wall randreg.c -o randreg -ggdb3
	
	gcc -std=gnu99 -Os -Wall iocsetvip.c -o iocsetvip -ggdb3 -I../driver/
	gcc -std=gnu99 -Os -Wall iocreadreg.c -o iocreadreg -ggdb3 -I../driver/
	gcc -std=gnu99 -Os -Wall iocwritereg.c -o iocwritereg -ggdb3 -I../driver/
	
	gcc -std=gnu99 -Os -Wall cursor.c -o cursor -ggdb3 -I../driver/
	
	gcc -std=gnu99 -Os -Wall rgb2ypbpr.c -o rgb2ypbpr -ggdb3
	
	gcc -std=gnu99 -Os -Wall ioccursor.c -o ioccursor -ggdb3 -I../driver/
	
	gcc -std=gnu99 -Os -Wall randosd.c -o randosd -ggdb3 -I../driver/
	gcc -std=gnu99 -Os -Wall osdtest.c -o osdtest -ggdb3 -I../driver/
	gcc -std=gnu99 -Os -Wall osdimage.c -o osdimage -ggdb3 -I../driver/
	
	gcc -std=gnu99 -Os -Wall hexcount.c -o hexcount -s
	
	gcc -std=gnu99 -Os -Wall ioctl.c -o ioctl -ggdb3 -I../driver/
	
	chmod +x testdpcint.sh dpc2dump.sh dpc2rand.sh osd.sh \
		testioctl.sh

clean:
	rm -f randram regload regdump readreg setbits write4reg \
	writereg iocsetvip iocreadreg iocwritereg cursor rgb2ypbpr \
	striperam ioccursor randosd osdtest memdump osdimage \
	memload rampram hexcount readmem writemem randreg setalpha \
	memloadswab memset

	make -C dpc clean
	make -C example clean
	make -C firmware clean


