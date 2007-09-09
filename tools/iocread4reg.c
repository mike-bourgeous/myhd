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
	__u32 reg;
	__u32 start;

	if(argc != 2) {
		printf("Usage: %s register\n", argv[0]);
		return -1;
	}
	
	if((ifd = open("/dev/tl880/reg0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/reg0");
		fprintf(stderr, "You must install the TL880 kernel module, and have write access to /dev/tl880/*\n");
		return -1;
	}

	start = strtoul(argv[1], NULL, 16);
	printf("%08x  ", start);
	for(reg = start; reg < start + 0x10; reg += 4) {
		value = reg;
		if(ioctl(ifd, TL880IOCREADREG, &value) < 0) {
			perror("Unable to read register");
			close(ifd);
			return -1;
		}

		printf("%08x ", value);
	}
	printf("\n");

	close(ifd);

	return 0;
}

