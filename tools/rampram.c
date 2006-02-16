#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
	int memfd, i, j;
	unsigned long addr = 0;
	unsigned long len = 0x01000000;
	unsigned char *memspace;
	unsigned long *lmspace;

	if(argc > 3 || (argc >= 2 && !strncmp(argv[1], "--help", 6))) {
		printf("Writes alternating values to TL880 RAM\n");
		printf("Usage: %s [addr [len]]\n", argv[0]);
		return -1;
	}

	if(argc >= 2) {
		addr = strtoul(argv[1], NULL, 16);
	}

	if(argc == 3) {
		len = strtoul(argv[2], NULL, 16);
	}

	addr &= 0xfffffffc;
	len &= 0xfffffffc;
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
	lmspace = (unsigned long *)memspace;

	printf("Writing ramp values\n");

	for(i = addr; i < addr + len; i += 0x100) {
		for(j = 0; j < 0x100 && i + j < addr + len; j += 4) {
			lmspace[(i + j) / 4] = j | j << 8 | j << 16 | j << 24;
		}
	}
	
	printf("Unmapping memory space\n");
	munmap(memspace, 0x01000000);
	close(memfd);

	return 0;
}

