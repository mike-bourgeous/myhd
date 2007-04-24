#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include "tl880.h"

int main(int argc, char *argv[])
{
	int ifd;
	__u32 value;

	if(argc != 2) {
		printf("Usage: %s register\n", argv[0]);
		return -1;
	}
	
	value = strtoul(argv[1], NULL, 16);

	if((ifd = open("/dev/tl880/reg0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/reg0");
		fprintf(stderr, "You must install the TL880 kernel module, and have write access to /dev/tl880/*\n");
		return -1;
	}

	if(ioctl(ifd, TL880IOCREADREG, &value) < 0) {
		perror("Unable to read register");
		close(ifd);
		return -1;
	}

	printf("Register 0x%08x is 0x%08x\n", (__u32)strtoul(argv[1], NULL, 16), value);

	close(ifd);

	return 0;
}

