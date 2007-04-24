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

#define VIDIOC_INT_I2S_CLOCK_FREQ 	_IOW ('d', 108, unsigned int)
#define AUDC_SET_RADIO        _IO('d',88)
/*
enum v4l2_tuner_type {
	V4L2_TUNER_RADIO	     = 1,
	V4L2_TUNER_ANALOG_TV	     = 2,
	V4L2_TUNER_DIGITAL_TV	     = 3,
};
*/
/* Switch the tuner to a specific tuner mode. Replacement of AUDC_SET_RADIO */
#define VIDIOC_INT_S_TUNER_MODE	     _IOW('d', 93, enum v4l2_tuner_type)



#include "tl880.h"

int main(int argc, char *argv[])
{
	int ifd;
	__u32 cmd;
	__u32 arg;

	if(argc != 3) {
		printf("Usage: %s ioctl_cmd ioctl_arg\n(values in hexadecimal)\n", argv[0]);
		printf("ex:\n");
		printf("  tl880 commands:\n");
		printf("    TL880IOCSETVIP: %s %lx num\n", argv[0], TL880IOCSETVIP);
		printf("  vpx commands:\n");
		printf("    DECODER_DUMP: %s %x 0\n", argv[0], DECODER_DUMP);
		printf("    DECODER_GET_STATUS: %s %lx 0\n", argv[0], DECODER_GET_STATUS);
		printf("    DECODER_SET_NORM: %s %lx num\n", argv[0], DECODER_SET_NORM);
		printf("    DECODER_SET_INPUT: %s %lx num\n", argv[0], DECODER_SET_INPUT);
		printf("    DECODER_ENABLE_OUTPUT: %s %lx num\n", argv[0], DECODER_ENABLE_OUTPUT);
		printf("  tuner commands:\n");
		printf("    VIDIOCSFREQ: %s %lx num\n", argv[0], VIDIOCSFREQ);
		printf("    VIDIOC_LOG_STATUS: %s %x 0\n", argv[0], VIDIOC_LOG_STATUS);
		printf("    VIDIOC_S_STD: %s %lx num\n", argv[0], VIDIOC_S_STD);
		printf("  msp commands:\n");
		printf("    VIDIOC_INT_I2S_CLOCK_FREQ: %s %lx 1024000/2048000\n", argv[0], VIDIOC_INT_I2S_CLOCK_FREQ);
		printf("    AUDC_SET_RADIO: %s %x 0/1\n", argv[0], AUDC_SET_RADIO);
		printf("    VIDIOC_INT_S_TUNER_MODE: %s %lx 1-3\n", argv[0], VIDIOC_INT_S_TUNER_MODE);

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

	printf("arg after ioctl: %u (0x%08x)\n", arg, arg);
	
	close(ifd);

	return 0;
}

