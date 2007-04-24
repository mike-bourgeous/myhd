#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/byteorder.h>
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



void write_regbits(__u32 reg, int high_bit, int low_bit, __u32 value)
{
	__u32 curval = read_register(reg);

	set_bits(&curval, reg, high_bit, low_bit, value);

	write_register(reg, curval);
}

/******************************************************************************/


/*
 * Range: 0-255 all channels
 * Return: value suitable for writing to regs 10140-1017c (cursor palette)
 */
__u32 rgb_to_ypbpr(__u8 r, __u8 g, __u8 b, __u8 a)
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
	r = (__u32)(y * 255.0);
	/* New Pb */
	g = (__u32)(pb * 255.0);
	/* New Pr */
	b = (__u32)(pr * 255.0);
	/* New A */
	a = alpha * 127.0;

	return (a << 24) | (r << 16) | (g << 8) | b;
}

#define rgba_to_argb(r, g, b, a) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))

int main(int argc, char *argv[])
{
	int memfd, i, j;
	unsigned char *memspace;
	__u32 *lmspace;
	unsigned char r1 = 0xff;
	unsigned char g1 = 0x00;
	unsigned char b1 = 0x00;
	unsigned char a1 = 0x7f;
	unsigned char r2 = 0x00;
	unsigned char g2 = 0x00;
	unsigned char b2 = 0xff;
	unsigned char a2 = 0x7f;

	if(argc != 1 && argc != 5 && argc != 9) {
		printf("Draws a test image on the TL880 screen\n");
		printf("Usage: %s [r1 g1 b1 a1 [r2 g2 b2 a2]] (all values range from 0-255)\n", argv[0]);
		return -1;
	}
	
	switch(argc)
	{
		case 9:
			r2 = strtoul(argv[5], NULL, 10);
			g2 = strtoul(argv[6], NULL, 10);
			b2 = strtoul(argv[7], NULL, 10);
			a2 = strtoul(argv[8], NULL, 10) / 2;
		case 5:
			r1 = strtoul(argv[1], NULL, 10);
			g1 = strtoul(argv[2], NULL, 10);
			b1 = strtoul(argv[3], NULL, 10);
			a1 = strtoul(argv[4], NULL, 10) / 2;
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
	lmspace = (__u32 *)memspace;

	/*
	for(i = 0x10088; i <= 0x100ac; i += 4) {
		write_register(i, 0x0);
	}
	*/

	printf("Turning on OSD\n");
	write_regbits(0x10000, 2, 2, 1);

	printf("Setting OSD memory offset and other OSD parameters\n");
	write_register(0x10080, 0x0);
	write_register(0x10084, 0x2d8000);
	write_register(0x10094, 0x8000);

	printf("Writing colorful data to memory\n");
	for(i = 0x218000; i < 0x25c500; i += 0x4800) {
		/*
		if(memspace[i] % 4 == 0) {
			memspace[i] = 0x7F;
		} else {
		*/
			/* memspace[i] = (i % 1024) / 4; */
		/*
		}
		*/
		/*
		memspace[i] = (rgb_to_ypbpr(255, 255, 255, 255) & 0xFF000000) >> 24;
		memspace[i + 1] = (rgb_to_ypbpr(255, 255, 255, 255) & 0xFF0000) >> 16;
		memspace[i + 2] = (rgb_to_ypbpr(255, 255, 255, 255) & 0xFF00) >> 8;
		memspace[i + 3] = (rgb_to_ypbpr(255, 255, 255, 255) & 0xFF);
		*/
		for(j = 0; j < 0x2400; j += 4) {
			/* lmspace[(i + j) / 4] = __cpu_to_be32(0xff7000ff); */
			/* lmspace[(i + j) / 4] = __cpu_to_be32(0xfefefe7f); */
			/* lmspace[(i + j) / 4] = rgba_to_argb(0x00, 0xff, 0xff, 0xff); */
			lmspace[(i + j) / 4] = rgba_to_argb(r1, g1, b1, a1);
		}
		for(j = 0x2400; j < 0x4800; j += 4) {
			/* lmspace[(i + j) / 4] = __cpu_to_be32(0x7f00ffff); */
			/* lmspace[(i + j) / 4] = __cpu_to_be32(0x00ff007f); */
			/* lmspace[(i + j) / 4] = rgba_to_argb(0x00, 0xff, 0x00, 0xff); */
			lmspace[(i + j) / 4] = rgba_to_argb(r2, g2, b2, a2);
		}
	}

	printf("Writing OSD parameters to memory\n");
	lmspace[0x2d8000 / 4] = 0x1ff0000b;
	lmspace[0x2d8004 / 4] = 0x70000000;
	lmspace[0x2d8008 / 4] = 0x00218000;
	lmspace[0x2d800c / 4] = 0xb40025c5;

	printf("Unmapping memory space\n");
	munmap(memspace, 0x01000000);
	close(memfd);

	printf("Closing register space\n");
	unmap_regspace();

	return 0;
}

