#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/byteorder.h>
#include <linux/byteorder/swab.h>
#include <linux/byteorder/generic.h>
#include "tl880.h"

int main(int argc, char *argv[])
{
	int ifd;
	unsigned long cursorpos;
	unsigned int x, y;

	if(argc != 3) {
		printf("Usage: %s x y\n", argv[0]);
		return -1;
	}
	
	x = strtoul(argv[1], NULL, 16);
	y = strtoul(argv[2], NULL, 16);

	if((ifd = open("/dev/tl880/reg0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/reg0");
		return -1;
	}

	cursorpos = (x & 0xffff) | ((y & 0xffff) << 16);

	printf("Modifying register\n");
	printf("Setting register 0x%08x to 0x%08lx\n", 0x10104, cursorpos);

	if(ioctl(ifd, TL880IOCSETCURSORPOS, &cursorpos) < 0) {
		perror("Unable to write cursor position");
		close(ifd);
		return -1;
	}
	
	close(ifd);

	return 0;
}

