/* 
 * Toggle video passthrough for TL880-based cards
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

static int memfd = 0;

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

__u32 tl880_calc_dpc_pll_const(__u32 a, unsigned char b, unsigned char c)
{
	return (((((a & 0x3ff) << 8) | (b & 0x1f)) << 4) | (c & 3)) << 4;
}

int main(int argc, char *argv[])
{
	__u32 a;
	unsigned char b, c;
	
	if(argc != 4) {
		printf("Usage: %s a b c\n", argv[0]);
		return -1;
	}
	
	a = strtoul(argv[1], NULL, 16);
	b = strtoul(argv[2], NULL, 16);
	c = strtoul(argv[3], NULL, 16);
	
	if(map_regspace()) {
		return -1;
	}

	a = tl880_calc_dpc_pll_const(a, b, c);

	printf("0x5800: 0x%08x\n", a);
	
	write_register(0x5800, a);

	unmap_regspace();
	
	return 0;
}


