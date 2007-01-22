#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <asm/byteorder.h>

void read_data(char *filename, unsigned char *memspace, unsigned long len)
{
	FILE *image;
	struct stat imgstat;
	int size;
	unsigned long buf = 0, *memlong = (unsigned long *)memspace;
	int i;
	
	if(!filename) {
		fprintf(stderr, "read_image: NULL filename\n");
		return;
	}

	if(!(image = fopen(filename, "rb"))) {
		fprintf(stderr, "Unable to open %s for reading: %s\n", filename, strerror(errno));
		return;
	}

	if(fstat(fileno(image), &imgstat)) {
		fprintf(stderr, "Unable to stat file %s: %s\n", filename, strerror(errno));
		fclose(image);
		return;
	}
	
	size = imgstat.st_size;
	if(len < size)
		size = len;

	/*
	if(size > 0xd8000) {
		size = 0xd8000;
	}
	*/

	for(i = 0; i < size; i += 4)
	{
		fread(&buf, 1, 4, image);
		memlong[i / 4] = __cpu_to_be32(buf);
	}

	fclose(image);
}



int main(int argc, char *argv[])
{
	int ifdr;
	unsigned long addr = 0;
	unsigned long len = 0x01000000;
	unsigned char *memspace;
	unsigned long *lmspace;

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
	lmspace = (unsigned long *)memspace;

	addr &= 0xfffffffc;
	len &= 0xfffffffc;
	len = len > 4 ? len : 4;
	if(addr >= 0x01000000) {
		addr = 0;
	}
	
	if(addr + len > 0x01000000) {
		len = 0x01000000 - addr;
	}

	printf("Loading 0x%lx bytes of %s at address 0x%lx\n", len, argv[1], addr);
	read_data(argv[1], memspace + addr, len);

	printf("Unmapping memory space\n");
	munmap(memspace, 0x01000000);
	close(ifdr);

	return 0;
}

