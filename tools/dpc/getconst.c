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

void set_bits(unsigned long *value, long reg, long high_bit, long low_bit, unsigned long setvalue)
{
	register unsigned long mask = 0;

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

void write_register(long reg, unsigned long value)
{
	unsigned long regval[2] = {reg, value};
	if(!memfd)
		return;
	
	if(ioctl(memfd, TL880IOCWRITEREG, regval) < 0) {
		perror("Unable to write register");
	}
}

unsigned long read_register(long reg)
{
	if(ioctl(memfd, TL880IOCREADREG, &reg) < 0) {
		perror("Unable to read register");
		return 0;
	}
	return reg;
}

unsigned long read_regbits(long reg, long high_bit, long low_bit)
{
	int shift = high_bit - low_bit + 1;
	int mask = ~(0xffffffff << shift);
	
	return (read_register(reg) >> low_bit) & mask;
}


void write_regbits(long reg, long high_bit, long low_bit, unsigned long value)
{
	unsigned long curval = read_register(reg);

	set_bits(&curval, reg, high_bit, low_bit, value);

	write_register(reg, curval);
}

unsigned long tl880_calc_dpc_pll_const(unsigned long a, unsigned char b, unsigned char c)
{
	return (((((a & 0x3ff) << 8) | (b & 0x1f)) << 4) | (c & 3)) << 4;
}

void tl880_uncalc_dpc_pll_const(unsigned long dpcconst, unsigned long *a, unsigned char *b, unsigned char *c)
{
	*a = (dpcconst >> 0x10) & 0x3ff;
	*b = (dpcconst >> 8) & 0x1f;
	*c = (dpcconst >> 4) & 3;
}

int main(int argc, char *argv[])
{
	unsigned long dpcconst;
	unsigned long a = 0;
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
	
	printf("0x%08lx: a = 0x%03lx, b = 0x%02x, c = 0x%x\n", dpcconst, a, b, c);
	
	return 0;
}


