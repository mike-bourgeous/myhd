#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	float red, green, blue, alpha = 1.0;
	float y, pb, pr;
	
	if(argc < 4 || argc > 5) {
		printf("Usage: %s red green blue [alpha]\n", argv[0]);
		printf("Range is from 00 to FF (hexadecimal expected)\n");
		return -1;
	}

	red = (float)strtol(argv[1], NULL, 16) / 255.0;
	green = (float)strtol(argv[2], NULL, 16) / 255.0;
	blue = (float)strtol(argv[3], NULL, 16) / 255.0;
	if(argc == 5) {
		alpha = (float)strtol(argv[4], NULL, 16) / 255.0;
	}
	
	printf("Red: %1.5f\tGreen: %1.5f\tBlue:%1.5f\tAlpha:%1.5f\n", red, green, blue, alpha);

	/* 
	 * Color conversion coefficients taken from SMPTE-240M YPbPr (without gamma)
	 * http://www.neuro.sfc.keio.ac.jp/~aly/polygon/info/color-space-faq.html
	 */
	/*
	y = 0.2122 * red + 0.7013 * green + 0.0865 * blue;
	pb = -0.1162 * red - 0.3838 * green + 0.5000 * blue;
	pr = 0.5000 * red - 0.4451 * green - 0.0549 * blue;
	*/

	/*
	 * TL880 luma formula with SMPTE-240M chroma formula (?)
	 */
	y = 0.2126 * red + 0.7152 * green + 0.0722 * blue;
	pb = -0.1162 * red - 0.3838 * green + 0.5000 * blue;
	pr = 0.5000 * red - 0.4451 * green - 0.0549 * blue;

	printf("Y: %1.5f\tPb: %1.5f\tPr: %1.5f\tAlpha: %1.5f\n", y, pb, pr, alpha);

	pb += 0.5;
	pr += 0.5;

	/* Clamp ranges */
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

	printf("Hex value (AYPbPr): %02x%02x%02x%02x\n", 
		(unsigned char)(alpha * 127.0),
		(unsigned char)(y * 255.0), (unsigned char)(pb * 255.0), (unsigned char)(pr * 255.0));

	return 0;
}

