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
	long reg = 0;
	unsigned long value;

	if(argc != 2) {
		printf("Usage: %s register\n", argv[0]);
		return -1;
	}
	
	reg = strtoul(argv[1], NULL, 16);

	if((ifd = open("/dev/tl880/reg0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/reg0");
		fprintf(stderr, "You must install the TL880 kernel module, and have write access to /dev/tl880/*\n");
		return -1;
	}

	printf("Mapping tl880 register space\n");
	if(!(regspace = mmap(0, 0x00100000, PROT_READ | PROT_WRITE, MAP_SHARED, ifd, 0))) {
		perror("Failed to mmap tl880 register space");
		close(ifd);
		return -1;
	}

	value = __le32_to_cpu(regspace[reg/4]);
	printf("Register 0x%08lx is 0x%08lx\n", reg, value);

	printf("Unmapping region 2\n");
	munmap(regspace, 0x00100000);
	close(ifd);

	return 0;
}

