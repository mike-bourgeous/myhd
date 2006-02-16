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
	unsigned long *memspace;
	long mem = 0;
	unsigned long value;

	if(argc != 2) {
		printf("Usage: %s address\n", argv[0]);
		return -1;
	}
	
	mem = strtoul(argv[1], NULL, 16);

	if((ifd = open("/dev/tl880/mem0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/mem0");
		fprintf(stderr, "You must install the TL880 kernel module, and have write access to /dev/tl880/*\n");
		return -1;
	}

	/* printf("Mapping tl880 memory space\n"); */
	if(!(memspace = mmap(0, 0x01000000, PROT_READ | PROT_WRITE, MAP_SHARED, ifd, 0))) {
		perror("Failed to mmap tl880 memory space");
		close(ifd);
		return -1;
	}

	value = __le32_to_cpu(memspace[mem/4]);
	printf("Memory at 0x%08lx is 0x%08lx\n", mem, value);

	/* printf("Unmapping memory\n"); */
	munmap(memspace, 0x01000000);
	close(ifd);

	return 0;
}

