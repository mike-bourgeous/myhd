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

void write_regbits(long reg, long high_bit, long low_bit, unsigned long value)
{
	unsigned long curval = read_register(reg);

	set_bits(&curval, reg, high_bit, low_bit, value);

	write_register(reg, curval);
}

/******************************************************************************/

unsigned char cursor[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


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
	int memfd, i, j/*, k, l*/;
	unsigned char *memspace;
	unsigned long x, y;

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
	if(!(memspace = mmap(0, 0x04000000, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, 0))) {
		perror("Failed to mmap region 1 (mem)");
		close(memfd);
		unmap_regspace();
		return -1;
	}

	printf("Clearing cursor info\n");
	write_regbits(0x10000, 3, 3, 0);
	write_regbits(0x10000, 8, 8, 0);
	write_register(0x10100, 0);
	write_register(0x10104, 0);
	write_register(0x10108, 0);

	printf("Writing cursor to TL880 memory\n");
	for(i = 0; i < 512; i += 4) {
		int j = i * 2;

		/* Pixels are packed two 4-bit pixels per byte, with each 4-byte set reversed */
		memspace[0x1d8000 + i + 3] = ((cursor[j + 0] & 0xf) << 4) | (cursor[j + 1] & 0x0f);
		memspace[0x1d8000 + i + 2] = ((cursor[j + 2] & 0xf) << 4) | (cursor[j + 3] & 0x0f);
		memspace[0x1d8000 + i + 1] = ((cursor[j + 4] & 0xf) << 4) | (cursor[j + 5] & 0x0f);
		memspace[0x1d8000 + i + 0] = ((cursor[j + 6] & 0xf) << 4) | (cursor[j + 7] & 0x0f);
	}

	printf("Setting cursor color palette\n");
	/*
	l = 0;
	for(i = 0; i < 256; i += 127 + (i ? 1 : 0)) {
		for(j = 0; j < 256; j += 127 + (j ? 0 : 1)) {
			for(k = 0; k < 256; k += 255) {
				printf("n: %i - r: %i, g: %i, b: %i, a: %i\n", l, i, j, k, l ? 255 : 0);
				write_register(0x10140 + l * 4, rgb2ypbpr(i, j, k, l ? 255 : 0));
				l++;
			}
		}
	}
	*/
	write_register(0x10140, rgb2ypbpr(0, 0, 0, 0));
	write_register(0x10144, rgb2ypbpr(0, 0, 255, 96));
	write_register(0x10148, rgb2ypbpr(0, 128, 0, 96));
	write_register(0x1014c, rgb2ypbpr(0, 128, 255, 96));
	write_register(0x10150, rgb2ypbpr(0, 255, 0, 96));
	write_register(0x10154, rgb2ypbpr(0, 255, 255, 96));
	write_register(0x10158, rgb2ypbpr(96, 0, 0, 96));
	write_register(0x1015c, rgb2ypbpr(127, 0, 255, 127));
	write_register(0x10160, rgb2ypbpr(127, 128, 0, 127));
	write_register(0x10164, rgb2ypbpr(127, 128, 255, 127));
	write_register(0x10168, rgb2ypbpr(127, 255, 0, 127));
	write_register(0x1016c, rgb2ypbpr(127, 255, 255, 127));
	write_register(0x10170, rgb2ypbpr(255, 0, 0, 160));
	write_register(0x10174, rgb2ypbpr(255, 0, 255, 160));
	write_register(0x10178, rgb2ypbpr(255, 128, 0, 160));
	write_register(0x1017c, rgb2ypbpr(255, 255, 255, 160));


	printf("Setting cursor memory offset\n");
	write_register(0x10100, 0x4d8000);

	printf("Setting cursor size\n");
	write_regbits(0x10108, 9, 8, 4);
	write_regbits(0x10108, 0x19, 0x18, 4);
	write_regbits(0x10108, 7, 0, 0);
	write_regbits(0x10108, 0x17, 0x10, 0);

	printf("Setting cursor position\n");
	write_regbits(0x10104, 0xa, 0, 0x100);
	write_regbits(0x10104, 0x1a, 0x10, 0x100);

	printf("Displaying cursor\n");
	write_regbits(0x10000, 3, 3, 1);

	for(y = 0; y < 0x300; y += (0x300 / 4)) {
		for(x = 0; x < 0x300; x += 4) {
			write_regbits(0x10104, 0xa, 0, x);
			write_regbits(0x10104, 0x1a, 0x10, y + (x / 8));
			usleep(1000);
		}
		for(x = 0x300; x > 0; x -= 4) {
			write_regbits(0x10104, 0xa, 0, x);
			write_regbits(0x10104, 0x1a, 0x10, y + (0x300 / 8) + ((0x300 - x) / 8));
			usleep(1000);
		}
	}

	write_regbits(0x10104, 0xa, 0, 0x100);
	write_regbits(0x10104, 0x1a, 0x10, 0x100);

	printf("Cycling cursor colors\n");
	for(j = 0; j < 16; j++) {
		for(i = 0; i < 512; i += 4) {
			int o = i * 2;
			memspace[0x1d8000 + i + 3] = ((cursor[o + 0] & j) << 4) | (cursor[o + 1] & j);
			memspace[0x1d8000 + i + 2] = ((cursor[o + 2] & j) << 4) | (cursor[o + 3] & j);
			memspace[0x1d8000 + i + 1] = ((cursor[o + 4] & j) << 4) | (cursor[o + 5] & j);
			memspace[0x1d8000 + i + 0] = ((cursor[o + 6] & j) << 4) | (cursor[o + 7] & j);
		}
		printf("%i\r", j);
		usleep(500000);
	}

	printf("Cycling cursor alpha\n");
	for(i = 0; i < 5; i++) {
		for(j = 0; j < 256; j++) {
			write_register(0x1017c, rgb2ypbpr(255, 255, 255, j));
			usleep(1000);
		}
		for(j = 255; j >= 0; j--) {
			write_register(0x1017c, rgb2ypbpr(255, 255, 255, j));
			usleep(1000);
		}
	}


	write_register(0x1017c, rgb2ypbpr(255, 255, 255, 160));



	printf("Unmapping memory space\n");
	munmap(memspace, 0x04000000);
	close(memfd);

	printf("Closing register space\n");
	unmap_regspace();

	return 0;
}

