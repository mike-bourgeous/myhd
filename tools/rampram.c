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
	int memfd;
	__u32 i, j;
	__u32 addr = 0;
	__u32 len = 0x01000000;
	__u32 ramplen = 0x100;
	unsigned char *memspace;
	__u32 *lmspace;

	if(argc > 4 || (argc >= 2 && !strncmp(argv[1], "--help", 6))) {
		printf("Writes alternating values to TL880 RAM\n");
		printf("Usage: %s [addr [len [ramplen]]]\n", argv[0]);
		return -1;
	}

	if(argc >= 2) {
		addr = strtoul(argv[1], NULL, 16);
	}

	if(argc >= 3) {
		len = strtoul(argv[2], NULL, 16);
	}

	if(argc >= 4) {
		ramplen = strtoul(argv[3], NULL, 16);
	}

	addr &= 0xfffffffc;
	len &= 0xfffffffc;
	ramplen &= 0xfffffffc;
	ramplen = ramplen > 8 ? ramplen : 8;
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

	printf("Writing ramp values\n");

	for(i = addr; i < addr + len; i += ramplen) {
		for(j = 0; j < ramplen && i + j < addr + len; j += 4) {
			__u32 k = j * 0x100 / ramplen;
			lmspace[(i + j) / 4] = k | k << 8 | k << 16 | k << 24;
			printf("writing 0x%08x to 0x%08x\n", 
				k | k << 8 | k << 16 | k << 24,
				i + j);
		}
	}
	
	printf("Unmapping memory space\n");
	munmap(memspace, 0x01000000);
	close(memfd);

	return 0;
}

