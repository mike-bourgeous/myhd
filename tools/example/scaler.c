/* 
 * Scaler setup code for TL880-based cards
 *
 * (c) 2003 Mike Bourgeous <nitrogen@slimetech.com>
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "tl880.h"

static int regfd = 0;

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
	if((regfd = open("/dev/tl880/reg0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/reg0");
		regfd = 0;
		return -1;
	}
	
	return 0;
}

void unmap_regspace()
{
	close(regfd);
}

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

/******************************************************************************/

int main(int argc, char *argv[])
{
	if(map_regspace()) {
		return -1;
	}
	unmap_regspace();
	return 0;
}


