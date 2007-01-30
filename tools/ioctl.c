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

#include <linux/videodev.h>
#include <linux/video_decoder.h>

#include "tl880.h"

int main(int argc, char *argv[])
{
	int ifd;
	unsigned long cmd, arg;

	if(argc != 3) {
		printf("Usage: %s ioctl_cmd ioctl_arg\n(values in hexadecimal)\n", argv[0]);
		printf("ex:\n");
		printf("  tl880 commands:\n");
		printf("    TL880IOCSETVIP: %s %x num\n", argv[0], TL880IOCSETVIP);
		printf("  vpx commands:\n");
		printf("    DECODER_DUMP: %s %x 0\n", argv[0], DECODER_DUMP);
		printf("    DECODER_GET_STATUS: %s %x 0\n", argv[0], DECODER_GET_STATUS);
		printf("    DECODER_SET_NORM: %s %x num\n", argv[0], DECODER_SET_NORM);
		printf("    DECODER_SET_INPUT: %s %x num\n", argv[0], DECODER_SET_INPUT);
		printf("    DECODER_ENABLE_OUTPUT: %s %x num\n", argv[0], DECODER_ENABLE_OUTPUT);
		printf("  tuner commands:\n");
		printf("    VIDIOCSFREQ: %s %x num\n", argv[0], VIDIOCSFREQ);
		printf("    VIDIOC_LOG_STATUS: %s %x 0\n", argv[0], VIDIOC_LOG_STATUS);
		printf("    VIDIOC_S_STD: %s %x num\n", argv[0], VIDIOC_S_STD);
		return -1;
	}
	
	cmd = strtoul(argv[1], NULL, 16);
	arg = strtoul(argv[2], NULL, 16);

	if((ifd = open("/dev/tl880/reg0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/reg0");
		return -1;
	}

	if(ioctl(ifd, cmd, &arg) < 0) {
		perror("Error executing ioctl");
		close(ifd);
		return -1;
	}

	printf("arg after ioctl: %lu (0x%08lx)\n", arg, arg);
	
	close(ifd);

	return 0;
}

