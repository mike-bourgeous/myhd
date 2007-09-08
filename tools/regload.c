#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/types.h>

int main(int argc, char *argv[])
{
	int regfd, ifd;
	int bytes;
	unsigned char *regspace;
	__u32 start = 0;

	if(argc < 2 || argc > 3 || (argc >= 2 && !strcmp(argv[1], "--help"))) {
		printf("Loads a binary dump of the tl880's registers\n");
		printf("Usage: %s reg_file [start_addr]\n", argv[0]);
		return -1;
	}

	if(argc == 3) {
		start = (strtoul(argv[2], NULL, 16) >> 2) << 2; // Make sure it's a multiple of four
	}

	if((regfd = open("/dev/tl880/reg0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/reg0");
		return -1;
	}

	printf("Mapping register space\n");
	if(!(regspace = mmap(0, 0x00100000, PROT_READ | PROT_WRITE, MAP_SHARED, regfd, 0))) {
		perror("Failed to mmap register space");
		close(regfd);
		return -1;
	}

	printf("Opening register dump\n");
	if((ifd = open(argv[1], O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) {
		fprintf(stderr, "Failed to open output file %s: %s\n", argv[1], strerror(errno));
		munmap(regspace, 0x00100000);
		close(regfd);
		return -1;
	}

	printf("Writing register space\n");
	if((bytes = read(ifd, regspace + start, 0x0040000)) == -1) {
		perror("Failed to load register dump");
		munmap(regspace, 0x00100000);
		close(regfd);
		close(ifd);
		return -1;
	}
	printf("Read %i bytes\n", bytes);
	close(ifd);

	
	printf("Unmapping register space\n");
	munmap(regspace, 0x00100000);
	close(regfd);

	return 0;
}

