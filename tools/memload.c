#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/types.h>

int main(int argc, char *argv[])
{
	int ifdr, ifd;
	__u32 addr = 0;
	__u32 len = 0x01000000;
	unsigned char *memspace;
	struct stat *dumpstat;

	if(argc < 2 || argc > 4) {
		printf("Loads tl880 memory\n");
		printf("Usage: %s mem_file [addr [len]]\n", argv[0]);
		return -1;
	}

	if(argc >= 3) {
		addr = strtoul(argv[2], NULL, 16);
	}

	if(argc == 4) {
		len = strtoul(argv[3], NULL, 16);
	}

	if((ifdr = open("/dev/tl880/mem0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/mem0");
		return -1;
	}

	printf("Mapping memory space\n");
	if(!(memspace = mmap(0, 0x01000000, PROT_READ | PROT_WRITE, MAP_SHARED, ifdr, 0))) {
		perror("Failed to mmap card memory");
		close(ifdr);
		return -1;
	}

	if((ifd = open(argv[1], O_RDONLY, 0)) < 0) {
		fprintf(stderr, "Failed to open input file %s: %s\n", argv[1], strerror(errno));
		munmap(memspace, 0x01000000);
		close(ifdr);
		return -1;
	}

	dumpstat = calloc(1, sizeof(struct stat));
	if(fstat(ifd, dumpstat)) {
		fprintf(stderr, "Couldn't stat %s: %s\n", argv[1], strerror(errno));
		munmap(memspace, 0x01000000);
		close(ifdr);
		close(ifd);
		return -1;
	}
	
	if(len > dumpstat->st_size) {
		len = dumpstat->st_size;
	}

	addr &= 0xfffffffc;
	len &= 0xfffffffc;
	if(addr >= 0x01000000) {
		addr = 0;
	}
	
	if(addr + len > 0x01000000) {
		len = 0x01000000 - addr;
	}

	printf("Loading 0x%x bytes of %s at address 0x%x\n", len, argv[1], addr);
	if(read(ifd, memspace + addr, len) != len) {
		perror("Failed to load memory dump\n");
		munmap(memspace, 0x01000000);
		close(ifdr);
		close(ifd);
		return -1;
	}
	close(ifd);

	printf("Unmapping memory space\n");
	munmap(memspace, 0x01000000);
	close(ifdr);

	return 0;
}

