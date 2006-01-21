#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "tl880.h"


static int regfd = 0;

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

void write_register(long reg, unsigned long value)
{
	unsigned long regval[2] = {reg, value};
	if(!regfd)
		return;
	
	if(ioctl(regfd, TL880IOCWRITEREG, regval) < 0) {
		perror("Unable to write register");
	}
}

unsigned long read_register(long reg)
{
	if(ioctl(regfd, TL880IOCREADREG, &reg) < 0) {
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



void write_regbits(long reg, long high_bit, long low_bit, unsigned long value)
{
	unsigned long curval = read_register(reg);

	set_bits(&curval, reg, high_bit, low_bit, value);

	write_register(reg, curval);
}

/******************************************************************************/


/*
 * Range: 0-255 all channels
 * Return: value suitable for writing to regs 10140-1017c (cursor palette)
 */
unsigned long rgb2ypbpr(unsigned long r, unsigned long g, unsigned long b, unsigned long a)
{
	float red = (float)r / 255.0;
	float green = (float)g / 255.0;
	float blue = (float)b / 255.0;
	float alpha = (float)a / 255.0;
	float y, pb, pr;

	/*
	 * TL880 luma formula with SMPTE-240M chroma formula (?)
	 */
	y = 0.2126 * red + 0.7152 * green + 0.0722 * blue;
	pb = -0.1162 * red - 0.3838 * green + 0.5000 * blue + 0.5;
	pr = 0.5000 * red - 0.4451 * green - 0.0549 * blue + 0.5;

	/* Clamp range */
	if(y < 0.0) {
		y = 0.0;
	} else if(y > 1.0) {
		y = 1.0;
	}

	if(pb < 0.0) {
		pb = 0.0;
	} else if(pb > 1.0) {
		pb = 1.0;
	}

	if(pr < 0.0) {
		pr = 0.0;
	} else if(pr > 1.0) {
		pr = 1.0;
	}

	if(alpha < 0.0) {
		alpha = 0.0;
	} else if(alpha > 1.0) {
		alpha = 1.0;
	}


	/* Use r, g, b, a as temporary storage variables to save namespace */
	/* New Y */
	r = (unsigned long)(y * 255.0);
	/* New Pb */
	g = (unsigned long)(pb * 255.0);
	/* New Pr */
	b = (unsigned long)(pr * 255.0);
	/* New A */
	a = alpha * 127.0;

	return (a << 24) | (r << 16) | (g << 8) | b;
}



int main(int argc, char *argv[])
{
	int memfd, i;
	unsigned char *memspace;
	unsigned long *lmspace;

	if(argc != 1) {
		printf("Draws a cursor on the TL880 screen\n");
		printf("Usage: %s\n", argv[0]);
		return -1;
	}

	printf("Accessing register space\n");
	if(map_regspace()) {
		perror("Failed to map register space");
		return -1;
	}
	
	printf("Mapping memory space\n");
	if((memfd = open("/dev/tl880/mem0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/mem0");
		unmap_regspace();
		return -1;
	}
	if(!(memspace = mmap(0, 0x01000000, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, 0))) {
		perror("Failed to mmap memory region");
		close(memfd);
		unmap_regspace();
		return -1;
	}
	lmspace = (unsigned long *)memspace;

	for(i = 0x10088; i <= 0x100ac; i += 4) {
		write_register(i, 0x0);
	}

	printf("Setting OSD memory offset and other OSD parameters\n");
	write_register(0x10080, 0x0);
	write_register(0x10084, 0x2d8000);
	write_register(0x10094, 0x8000);

	printf("Writing colorful data to memory\n");
	for(i = 0x100000; i < 0x1d8000; i++) {
		/*
		if(memspace[i] % 4 == 0) {
			memspace[i] = 0x7F;
		} else {
		*/
			memspace[i] = (i % 1024) / 4;
		/*
		}
		*/
	}

	printf("Writing OSD parameters to memory\n");
	lmspace[0x2d8000 / 4] = 0x1ff00012;
	lmspace[0x2d8004 / 4] = 0x00008000;
	lmspace[0x2d8008 / 4] = 0x08100000;
	lmspace[0x2d800c / 4] = 0x0c000000;
	

	printf("Unmapping memory space\n");
	munmap(memspace, 0x01000000);
	close(memfd);

	printf("Closing register space\n");
	unmap_regspace();

	return 0;
}

