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
	int memfd;
	unsigned long i, j;
	unsigned long addr = 0;
	unsigned long len = 0x01000000;
	unsigned long value = 0x0;
	unsigned long valsize = 4;
	unsigned char *memspace;
	unsigned short *smspace;
	unsigned long *lmspace;

	if(argc > 5 || (argc >= 2 && !strncmp(argv[1], "--help", 6))) {
		printf("Writes repeated values to TL880 RAM\n");
		printf("Usage: %s [value(hex) [addr [len [nbytes(1 to 4)]]]]\n", argv[0]);
		return -1;
	}

	if(argc >= 2) {
		value = strtoul(argv[1], NULL, 16);
	}

	if(argc >= 3) {
		addr = strtoul(argv[2], NULL, 16);
	}

	if(argc >= 4) {
		len = strtoul(argv[3], NULL, 16);
	}

	if(argc >= 5) {
		valsize = strtoul(argv[4], NULL, 10);
		if(valsize < 1) {
			valsize = 1;
		} else if(valsize > 4) {
			valsize = 4;
		}
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

	smspace = (unsigned short *)memspace;
	lmspace = (unsigned long *)memspace;

	printf("Writing value %08lx from %lx to %lx\n", value, addr, addr + len);

	switch(valsize) {
		default:
			fprintf(stderr, "Broken valsize %ld\n", valsize);
			break;
		case 1:
			for(i = addr; i < addr + len; i += valsize) {
				memspace[i] = value & 0xff;
				fprintf(stderr, "uchar mem[%lx] = %08lx\r", i, value & 0xff);
			}
			break;
		case 2:
			for(i = addr; i < addr + len; i += valsize) {
				smspace[i / 2] = value & 0xffff;
				fprintf(stderr, "ushort mem[%lx] = %04lx\r", i, value & 0xffff);
			}
			break;
		case 3:
			for(i = addr; i < addr + len; i += valsize) {
				memspace[i] = (value >> 16) & 0xff;
				memspace[i + 1] = (value >> 8) & 0xff;
				memspace[i + 2] = value & 0xff;
				fprintf(stderr, "uchar mem[%lx-%lx] = %06lx\r", i, i+2, value & 0xffffff);
			}
			break;
		case 4:
			for(i = addr; i < addr + len; i += valsize) {
				lmspace[i / 4] = value;
				fprintf(stderr, "ushort mem[%lx] = %08lx\r", i, value);
			}
			break;
	}

	fprintf(stderr, "\n");

	printf("Unmapping memory space\n");
	munmap(memspace, 0x01000000);
	close(memfd);

	return 0;
}

