all:
	gcc -O2 -Wall firmupload.c -o firmupload -ggdb3

clean:
	rm -f firmupload

dist:	clean
	dd if=/dev/zero bs=1 count=3668 of=mcufirmware.bin
	dd if=/dev/zero bs=1 count=1024 of=mcudata.bin
	dd if=/dev/zero bs=1 count=3586 of=tsdfirmware.bin
	dd if=/dev/zero bs=1 count=2988 of=tsddata.bin

