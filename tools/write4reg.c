/*
 * Non-prefetchable 32 bit memory at 0xdc000000 [0xdfffffff].
 * Non-prefetchable 32 bit memory at 0xe2000000 [0xe20fffff].
 * Non-prefetchable 32 bit memory at 0xe2100000 [0xe210ffff].
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <asm/byteorder.h>
#include <linux/byteorder/swab.h>
#include <linux/byteorder/generic.h>

int main(int argc, char *argv[])
{
	int ifd;
	unsigned long *buf2;
	int i;
	long reg = 0;
	unsigned long value[4] = {0, 0, 0, 0};

	if(argc != 6) {
		printf("Usage: %s register value value value value\n", argv[0]);
		return -1;
	}

	reg = strtoul(argv[1], NULL, 16);
	value[0] = strtoul(argv[2], NULL, 16);
	value[1] = strtoul(argv[3], NULL, 16);
	value[2] = strtoul(argv[4], NULL, 16);
	value[3] = strtoul(argv[5], NULL, 16);

	if((ifd = open("/dev/tl880/reg0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/reg0");
		return -1;
	}

	if(!(buf2 = mmap(0, 0x00100000, PROT_READ | PROT_WRITE, MAP_SHARED, ifd, 0))) {
		perror("Failed to mmap tl880 register space");
		close(ifd);
		return -1;
	}

	for(i = 0; i < 4; i++) {
		/* 
		 * Note: this isn't safe for all registers, as reading from some registers can cause a state
		 * change on the card
		 */
		printf("reg: 0x%08lx old: 0x%08lx new: 0x%08lx\n", reg + i * 4, buf2[i + reg / 4], value[i]);
		buf2[i + reg / 4] = value[i];
	}
	
	munmap(buf2, 0x00100000);
	close(ifd);

	return 0;
}

