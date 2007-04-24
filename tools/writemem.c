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
	__u32 *memspace;
	__u32 mem;
	__u32 value;

	if(argc != 3) {
		printf("Usage: %s address value\n", argv[0]);
		return -1;
	}
	
	mem = strtoul(argv[1], NULL, 16);
	value = strtoul(argv[2], NULL, 16);

	if((ifd = open("/dev/tl880/mem0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/mem0");
		return -1;
	}

	printf("Mapping tl880 memory space\n");
	if(!(memspace = mmap(0, 0x01000000, PROT_READ | PROT_WRITE, MAP_SHARED, ifd, 0))) {
		perror("Failed to mmap tl880 memory space");
		close(ifd);
		return -1;
	}

	printf("Modifying memory\n");
	printf("Setting memory+0x%08x(%d) to 0x%08x\n", (__u32)mem, mem, value);

	memspace[mem / 4] = __cpu_to_le32(value);
	
	printf("Unmapping memory\n");
	munmap(memspace, 0x01000000);
	close(ifd);

	return 0;
}

