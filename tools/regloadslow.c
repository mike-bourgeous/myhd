#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include "tl880.h"

static int regfd;

void write_register(__u32 reg, __u32 value)
{
	__u32 regval[2] = {reg, value};
	if(!regfd)
		return;
	
	if(ioctl(regfd, TL880IOCWRITEREG, regval) < 0) {
		perror("Unable to write register");
	}
}

int main(int argc, char *argv[])
{
	int ifd;
	int bytes;
	unsigned char *regspace;
	__u32 start = 0, slow_at = 0;
	__u32 reg, regval;
	int delay = 10;

	if(argc < 2 || argc > 5 || (argc >= 2 && (!strcmp(argv[1], "--help") || !strcmp(argv[argc - 1], "--help")))) {
		printf("Loads a binary dump of the tl880's registers with a default delay of 10ms per register.\n");
		printf("If delay is specified, at least that number of milliseconds will elapse between writes.\n");
		printf("If slow-at is specified, register loading will go fast until that register is reached.\n");
		printf("Usage: %s reg_file [start_addr [delay [slow-at]]]\n", argv[0]);
		return -1;
	}

	if(argc >= 3) {
		start = (strtoul(argv[2], NULL, 16) >> 2) << 2; // Make sure it's a multiple of four
	}

	if(argc >= 4) {
		delay = strtoul(argv[3], NULL, 0);
	}

	if(argc >= 5) {
		slow_at = (strtoul(argv[4], NULL, 16) >> 2) << 2; // Make sure it's a multiple of four
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

	if(slow_at > 0 && slow_at > start) {
		printf("Writing register space (fast)\n");
		if((bytes = read(ifd, regspace + start, slow_at - start)) == -1) {
			perror("Failed to load register dump");
			munmap(regspace, 0x00100000);
			close(regfd);
			close(ifd);
			return -1;
		}
		printf("Read %d bytes\n", bytes);
	}
	printf("Writing register space (slow)\n");
	for(reg = slow_at, regval = 0; reg < 0x40000; reg += 4) {
		if(read(ifd, &regval, 4) == -1) {
			perror("Failed to load register dump");
			munmap(regspace, 0x00100000);
			close(regfd);
			close(ifd);
			return -1;
		}

		printf("\rSetting register %x to 0x%08x", reg, regval);
		fflush(stdout);
		fsync(fileno(stdout));
		usleep(delay * 1000);
		write_register(reg, regval);
	}
	printf("Read ~%d more bytes\n", reg - slow_at);

	close(ifd);

	printf("Unmapping register space\n");
	munmap(regspace, 0x00100000);
	close(regfd);

	return 0;
}

