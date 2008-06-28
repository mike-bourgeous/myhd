/* 
 * Toggle video passthrough for TL880-based cards
 *
 * (c) 2003-2008 Mike Bourgeous <nitrogen@users.sourceforge.net>
 * (c) 2007 Jason P. Matthews
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "tl880.h"

static int memfd = 0;

void set_gpio(__u32 line, __u32 state)
{
	__u32 regval[2];

	regval[0] = line;
	regval[1] = state;

	if(ioctl(memfd, TL880IOCSETGPIO, regval) < 0) {
		perror("Unable to set gpio");
	}
}

void set_bits(__u32 *value, __u32 reg, int high_bit, int low_bit, __u32 setvalue)
{
	register __u32 mask = 0;

	/* set bits from high_bit to low_bit in mask to 1 */
	mask = ~(0xFFFFFFFF << (high_bit - low_bit + 1)) << low_bit;
	setvalue <<= low_bit;
	setvalue &= mask;
	mask = ~mask & *value;
	setvalue |= mask;
	*value = setvalue;
}

int map_regspace()
{
	if((memfd = open("/dev/tl880/reg0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/reg0");
		return -1;
	}
	return 0;
}

void unmap_regspace()
{
	close(memfd);
}

void write_register(__u32 reg, __u32 value)
{
	__u32 regval[2] = {reg, value};
	if(!memfd)
		return;
	
	if(ioctl(memfd, TL880IOCWRITEREG, regval) < 0) {
		perror("Unable to write register");
	}
}

__u32 read_register(__u32 reg)
{
	if(ioctl(memfd, TL880IOCREADREG, &reg) < 0) {
		perror("Unable to read register");
		return 0;
	}
	return reg;
}

__u32 read_regbits(__u32 reg, int high_bit, int low_bit)
{
	int shift = high_bit - low_bit + 1;
	int mask = ~(0xffffffff << shift);
	
	return (read_register(reg) >> low_bit) & mask;
}


void write_regbits(__u32 reg, int high_bit, int low_bit, __u32 value)
{
	__u32 curval = read_register(reg);

	set_bits(&curval, reg, high_bit, low_bit, value);

	write_register(reg, curval);
}

int main(int argc, char *argv[])
{
	int output_enable;
	if(argc != 2) {
		printf("Usage: %s output_enable\n", argv[0]);
		return -1;
	}
	
	output_enable = strtoul(argv[1], NULL, 10) ? 1 : 0;
	
	if(map_regspace()) {
		return -1;
	}
	
 	if(output_enable) {
 		/*
		 * write_regbits(0x10190, 0xa, 0xa, output_enable);
		 * write_regbits(0x10194, 0xa, 0xa, output_enable);
		 * write_regbits(0x10198, 0xa, 0xa, output_enable);
		 */
 		set_gpio(2, 1);
 		set_gpio(0xd, 1);
 	} else {
 		/* write_register(0x10000, 0x80); */
 		set_gpio(2, 0);
 		set_gpio(0xd, 0);
	}

	unmap_regspace();
	
	return 0;
}


