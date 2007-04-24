#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include "tl880.h"

static int regfd = 0;

void write_register(__u32 reg, __u32 value)
{
	__u32 regval[2] = {reg, value};
	if(!regfd)
		return;
	
	if(ioctl(regfd, TL880IOCWRITEREG, regval) < 0) {
		perror("Unable to write register");
	}
}

__u32 read_register(__u32 reg)
{
	if(ioctl(regfd, TL880IOCREADREG, &reg) < 0) {
		perror("Unable to read register");
		return 0;
	}
	return reg;
}




int main(int argc, char *argv[])
{
	int i;
	__u32 rv;

	if(argc != 1) {
		printf("Randomizes the OSD context (registers 10080 to 100ac)\n");
		printf("Usage: %s\n", argv[0]);
		return -1;
	}

	if((regfd = open("/dev/tl880/reg0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/reg0");
		return -1;
	}
		
	printf("Writing random values to registers\n");
	srand(time(0));
	for(i = 0x10080; i < 0x100ac; i += 4) {
		rv = rand();
		fprintf(stderr, "u__u32 reg[%x] = 0x%x\r", i, rv);
		write_register(i, rv);
		usleep(200000);
	}

	fprintf(stderr, "\n");
	
	close(regfd);

	return 0;
}

