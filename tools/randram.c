#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
	int memfd, i;
	unsigned char rv, *memspace;

	if(argc != 1) {
		printf("Randomizes the beginning of tl880 RAM\n");
		printf("Usage: %s\n", argv[0]);
		return -1;
	}

	if((memfd = open("/dev/tl880/mem0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/mem0");
		return -1;
	}
		
	printf("Mapping memory space\n");
	if(!(memspace = mmap(0, 0x04000000, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, 0))) {
		perror("Failed to mmap region 1 (mem)");
		close(memfd);
		return -1;
	}

	printf("Writing random values\n");
	srand(time(0));
	for(i = 0; i < 0x800000; i++) {
		rv = rand();
		if(i % 32 == 0) {
			fprintf(stderr, "uchar mem[%i] = %u\r", i, rv);
		}
		memspace[i] = rv;
	}

	fprintf(stderr, "\n");
	
	printf("Unmapping memory space\n");
	munmap(memspace, 0x04000000);
	close(memfd);

	return 0;
}

