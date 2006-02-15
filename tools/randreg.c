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
	int memfd, i;
	unsigned long addr = 0;
	unsigned long len = 0x01000000;
	unsigned long randval;
	unsigned long *memspace;

	if(argc > 3 || (argc >= 2 && !strncmp(argv[1], "--help", 6))) {
		printf("Randomizes all or part of TL880 registers (careful!!!)\n");
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
	if(addr >= 0x0100000) {
		addr = 0;
	}
	
	if(addr + len >= 0x0100000) {
		len = 0x0100000 - addr;
	}

	if((memfd = open("/dev/tl880/reg0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/reg0");
		return -1;
	}
		
	printf("Mapping memory space\n");
	if(!(memspace = mmap(0, 0x01000000, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, 0))) {
		perror("Failed to mmap region 1 (mem)");
		close(memfd);
		return -1;
	}

	printf("Writing random values\n");
	srand(time(0));
	for(i = addr; i < addr + len; i += 4) {
		randval = rand();
		if(i % 64 == 0) {
			fprintf(stderr, "ulong mem[%x] = %08lx\r", i, randval);
		}
		memspace[i / 4] = randval;
	}

	fprintf(stderr, "\n");
	
	printf("Unmapping memory space\n");
	munmap(memspace, 0x01000000);
	close(memfd);

	return 0;
}

