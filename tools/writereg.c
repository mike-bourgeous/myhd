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
	unsigned long *regspace;
	long reg;
	unsigned long value;

	if(argc != 3) {
		printf("Usage: %s register value\n", argv[0]);
		return -1;
	}
	
	reg = strtoul(argv[1], NULL, 16);
	value = strtoul(argv[2], NULL, 16);

	if((ifd = open("/dev/tl880/reg0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/reg0");
		return -1;
	}

	printf("Mapping tl880 register space\n");
	if(!(regspace = mmap(0, 0x00100000, PROT_READ | PROT_WRITE, MAP_SHARED, ifd, 0))) {
		perror("Failed to mmap tl880 register space");
		close(ifd);
		return -1;
	}

	printf("Modifying register\n");
	printf("Setting region2+0x%08lx(%li) to 0x%08lx\n", (unsigned long)reg, reg, value);

	regspace[reg / 4] = __cpu_to_le32(value);
	
	printf("Unmapping region 2\n");
	munmap(regspace, 0x00100000);
	close(ifd);

	return 0;
}

