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
	__u32 regval[2];

	if(argc != 3) {
		printf("Usage: %s register value\n", argv[0]);
		return -1;
	}
	
	regval[0] = strtoul(argv[1], NULL, 16);
	regval[1] = strtoul(argv[2], NULL, 16);

	if((ifd = open("/dev/tl880/reg0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/reg0");
		return -1;
	}

	printf("Modifying register\n");
	printf("Setting register 0x%08x to 0x%08x\n", regval[0], regval[1]);

	if(ioctl(ifd, TL880IOCWRITEREG, regval) < 0) {
		perror("Unable to write register");
		close(ifd);
		return -1;
	}
	
	close(ifd);

	return 0;
}

