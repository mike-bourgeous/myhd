all:
	gcc -O2 -Wall randram.c -o randram -ggdb3
	gcc -O2 -Wall striperam.c -o striperam -ggdb3
	gcc -O2 -Wall regload.c -o regload -ggdb3
	gcc -O2 -Wall regdump.c -o regdump -ggdb3
	gcc -O2 -Wall memdump.c -o memdump -ggdb3
	
	gcc -O2 -Wall readreg.c -o readreg -ggdb3
	gcc -O2 -Wall writereg.c -o writereg -ggdb3
	gcc -O2 -Wall setbits.c -o setbits -ggdb3
	gcc -O2 -Wall write4reg.c -o write4reg -ggdb3
	
	gcc -O2 -Wall iocsetvip.c -o iocsetvip -ggdb3 -I../driver/
	gcc -O2 -Wall iocreadreg.c -o iocreadreg -ggdb3 -I../driver/
	gcc -O2 -Wall iocwritereg.c -o iocwritereg -ggdb3 -I../driver/
	
	gcc -O2 -Wall cursor.c -o cursor -ggdb3 -I../driver/
	
	gcc -O2 -Wall rgb2ypbpr.c -o rgb2ypbpr -ggdb3
	
	gcc -O2 -Wall ioccursor.c -o ioccursor -ggdb3 -I../driver/
	
	gcc -O2 -Wall randosd.c -o randosd -ggdb3 -I../driver/
	gcc -O2 -Wall osdtest.c -o osdtest -ggdb3 -I../driver/
	
	chmod +x testdpcint.sh dpc2dump.sh dpc2rand.sh

clean:
	rm -f randram regload regdump readreg setbits write4reg \
	writereg iocsetvip iocreadreg iocwritereg cursor rgb2ypbpr \
	striperam ioccursor randosd osdtest memdump

