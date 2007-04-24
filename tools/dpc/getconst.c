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

void tl880_uncalc_dpc_pll_const(__u32 dpcconst, __u32 *a, unsigned char *b, unsigned char *c)
{
	*a = (dpcconst >> 0x10) & 0x3ff;
	*b = (dpcconst >> 8) & 0x1f;
	*c = (dpcconst >> 4) & 3;
}

int main(int argc, char *argv[])
{
	__u32 dpcconst;
	__u32 a = 0;
	unsigned char b = 0, c = 0;
	
	if(argc == 2) {
		/* Calculate from parameter */
		dpcconst = strtoul(argv[1], NULL, 16);
		tl880_uncalc_dpc_pll_const(dpcconst, &a, &b, &c);
	} else if(argc == 1) {
		/* Read from card */
		if(map_regspace()) {
			return -1;
		}

		dpcconst = read_register(0x5800);
		tl880_uncalc_dpc_pll_const(dpcconst, &a, &b, &c);
		
		unmap_regspace();
	} else {
		printf("Usage: %s [c]\n", argv[0]);
		return -1;
	}
	
	printf("0x%08x: a = 0x%03x, b = 0x%02x, c = 0x%x\n", dpcconst, a, b, c);
	
	return 0;
}


