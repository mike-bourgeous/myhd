#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/types.h>

int main(int argc, char *argv[])
{
	int memfd, i, j;
	__u32 addr = 0;
	__u32 len = 0x01000000;
	__u32 width = 0x100;
	unsigned char *memspace;
	__u32 *lmspace;

	if(argc > 4 || (argc >= 2 && !strncmp(argv[1], "--help", 6))) {
		printf("Writes alternating values to TL880 RAM\n");
		printf("Usage: %s [addr [len] [width]]\n", argv[0]);
		return -1;
	}

	if(argc >= 2) {
		addr = strtoul(argv[1], NULL, 16);
	}

	if(argc >= 3) {
		len = strtoul(argv[2], NULL, 16);
	}

	if(argc >= 4) {
		width = strtoul(argv[3], NULL, 16);
	}

	addr &= 0xfffffffc;
	len &= 0xfffffffc;
	width &= 0xfffffffc;
	width = width > 8 ? width : 8;
	if(addr >= 0x01000000) {
		addr = 0;
	}
	
	if(addr + len >= 0x01000000) {
		len = 0x01000000 - addr;
	}

	if((memfd = open("/dev/tl880/mem0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/mem0");
		return -1;
	}
		
	printf("Mapping memory space\n");
	if(!(memspace = mmap(0, 0x01000000, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, 0))) {
		perror("Failed to mmap region 1 (mem)");
		close(memfd);
		return -1;
	}
	lmspace = (__u32 *)memspace;

	printf("Writing stripe values\n");
#if 0
	for(i = addr; i < addr + len; i++) {
		/*
		memspace[i] = (i % 2) * 0xf0;
		*/
		if(memspace[i] % 4 == 0) {
			memspace[i] = 0x7F;
		} else {
			memspace[i] = (i % 512) / 2;
		}
		/*
		memspace[i] = (i % 4) ? 0x00 : 0xff;
		*/
	}
#endif

	for(i = addr; i < addr + len; i += width) {
		for(j = 0; j < width / 2; j += 4) {
			lmspace[(i + j) / 4] = 0xffffffff;
		}
		for(j = width / 2; j < width; j += 4) {
			lmspace[(i + j) / 4] = 0x00000000;
		}
	}
	
	printf("Unmapping memory space\n");
	munmap(memspace, 0x01000000);
	close(memfd);

	return 0;
}

