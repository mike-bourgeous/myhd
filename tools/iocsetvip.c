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
	unsigned long vipstate;

	if(argc != 2) {
		printf("Usage: %s vipstate(0,1,2)\n", argv[0]);
		return -1;
	}
	
	vipstate = strtoul(argv[1], NULL, 16);

	if((ifd = open("/dev/tl880/reg0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/reg0");
		return -1;
	}

	if(ioctl(ifd, TL880IOCSETVIP, &vipstate) < 0) {
		perror("Unable to set vip state");
		close(ifd);
		return -1;
	}
	
	close(ifd);

	return 0;
}

