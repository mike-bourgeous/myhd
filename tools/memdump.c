#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
	int ifdr, ofd;
	unsigned long addr = 0;
	unsigned long len = 0x01000000;
	unsigned char *memspace;

	if(argc < 2 || argc > 4) {
		printf("Dumps tl880 memory\n");
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

	addr &= 0xfffffffc;
	len &= 0xfffffffc;
	if(addr >= 0x01000000) {
		addr = 0;
	}
	
	if(addr + len > 0x01000000) {
		len = 0x01000000 - addr;
	}
	
	printf("Writing 0x%lx bytes starting at offset 0x%lx to %s\n", len, addr, argv[1]);
	if((ofd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) {
		fprintf(stderr, "Failed to open output file %s: %s\n", argv[1], strerror(errno));
		munmap(memspace, 0x01000000);
		close(ifdr);
		return -1;
	}
	if(write(ofd, memspace + addr, len) != len) {
		perror("Failed to write region 0\n");
		munmap(memspace, 0x01000000);
		close(ifdr);
		close(ofd);
		return -1;
	}
	close(ofd);

	printf("Unmapping memory space\n");
	munmap(memspace, 0x01000000);
	close(ifdr);

	return 0;
}

