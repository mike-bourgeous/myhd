#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/byteorder.h>
#include <linux/byteorder/swab.h>
#include <linux/byteorder/generic.h>
#include "tl880.h"

int main(int argc, char *argv[])
{
	int fd;
	unsigned long regval[2];

	if(argc != 3) {
		printf("Usage: %s gpio_line state\n", argv[0]);
		return -1;
	}
	
	regval[0] = strtoul(argv[1], NULL, 16);
	regval[1] = strtoul(argv[2], NULL, 16);

	if((fd = open("/dev/tl880/reg0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/reg0");
		return -1;
	}

	if(ioctl(fd, TL880IOCSETGPIO, regval) < 0) {
		perror("Unable to write register");
		close(fd);
		return -1;
	}
	
	close(fd);

	return 0;
}

