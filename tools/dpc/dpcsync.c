/* 
 * DPC setup code for TL880-based cards
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

typedef struct {
	/* 0x10014 */
	unsigned char a:1;
	unsigned char b:1;
	unsigned char c:1;
	unsigned char d:1;	/* prog_scan */
	unsigned short e:10;
	unsigned short f:11;	/* xres */
	unsigned char g:1;
	unsigned char h:1;
	unsigned char i:1;
	unsigned char j:1;

	/* 0x10018 */
	unsigned short k:9;	/* h_backporch */
	unsigned char l:1;	/* inv_hsync */
	unsigned char m:1;	/* inv_vsync */
	unsigned char n:7;	/* h_synclen */
	unsigned short o:9;	/* h_frontporch */

	/* 0x1001c - first field */
	unsigned char p:3;	/* v_synclen_0 */
	unsigned char q:8;	/* v_frontporch_0 */
	unsigned short r:11;	/* yres_0 */
	unsigned char s:4;	/* v_backporch_0 */
	unsigned char t:1;	/* ntsc_flag */

	/* 0x10020 - second field */
	unsigned char u:3;	/* v_synclen_1 */
	unsigned char v:8;	/* v_frontporch_1 */
	unsigned short w:11;	/* yres_1 */
	unsigned char x:4;	/* v_backporch_1 */

	/* 0x10024 */
	unsigned long y:22;
	unsigned char z:1;
	unsigned char aa:4;

	/* 0x10028 */
	unsigned short bb:12;
	unsigned char cc:1;

	/* 0x5800 */
	unsigned long dd;

} mode_def_letters;

typedef struct {
	/* 0x10014 */
	unsigned char a:1;
	unsigned char b:1;
	unsigned char c:1;
	unsigned char prog_scan:1;	/* prog_scan */
	unsigned short e:10;
	unsigned short xres:11;		/* xres */
	unsigned char g:1;
	unsigned char h:1;
	unsigned char i:1;
	unsigned char j:1;

	/* 0x10018 */
	unsigned short h_backporch:9;	/* h_backporch */
	unsigned char inv_hsync:1;	/* inv_hsync */
	unsigned char inv_vsync:1;	/* inv_vsync */
	unsigned char h_synclen:7;	/* h_synclen */
	unsigned short h_frontporch:9;	/* h_frontporch */

	/* 0x1001c - first field */
	unsigned char v_synclen_0:3;	/* v_synclen_0 */
	unsigned char v_frontporch_0:8;	/* v_frontporch_0 */
	unsigned short yres_0:11;	/* yres_0 */
	unsigned char v_backporch_0:4;	/* v_backporch_0 */
	unsigned char ntsc_flag:1;	/* ntsc_flag */

	/* 0x10020 - second field */
	unsigned char v_synclen_1:3;	/* v_synclen_1 */
	unsigned char v_frontporch_1:8;	/* v_frontporch_1 */
	unsigned short yres_1:11;	/* yres_1 */
	unsigned char v_backporch_1:4;	/* v_backporch_1 */

	/* 0x10024 */
	unsigned long y:22;
	unsigned char z:1;
	unsigned char aa:4;

	/* 0x10028 */
	unsigned short bb:12;
	unsigned char cc:1;

	/* 0x5800 */
	unsigned long dd;

} mode_def;


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

/******************************************************************************/
static unsigned long component_video = 1;
static unsigned long invert_hsync = 0;
static unsigned long invert_vsync = 0;

void set_aux_horizontal_scaler(int width, int a)
{
	unsigned long value;

	value  = (((((a << 8) / width) & 0xff) * width + (a * 0xf00)) << 0x14) & 0xfff00000;
	value |= ((((((a << 8) / width) & 0xff) + 1) * width - (a << 8)) << 8) & 0x000fff00;
	value |= 					    ((a << 8) / width) & 0x000000ff;
	write_register(0x100c8, value);
	write_register(0x100d0, (0x6b3 - a * 2) | 0x75a000);
}

void set_aux_vertical_scaler(unsigned long height, unsigned long a, unsigned long interlace)
{
	unsigned long value;

	value  = (((((a << 8) / height) & 0xff) * height + (a * 0xf00)) << 0x14) & 0xfff00000;
	value |= ((((((a << 8) / height) & 0xff) + 1) * height - (a << 8)) << 8) & 0x000fff00;
	value |= 					     ((a << 8) / height) & 0x000000ff;

	write_register(0x100cc, value);
	value = read_register(0x100c0);

	while(value != (value = read_register(0x100c0)));

	value = interlace ? 2 : 0;

	write_register(0x100c0, (((a << 8) / height) << 8) | value | height);
	write_register(0x100d4, 0x1050020d | (0x41000 - (a << 0xa)));
	write_register(0x100d8, 0x20b00107 | (0x82c00 - (a << 0xa)));
}

void set_aux_picture_origin(unsigned long x, unsigned long y)
{
	write_register(0x100c4, x | (y << 0xc));
}

unsigned long test_aux_dscr_mbox_empty()
{
	return (read_register(0x25728) ? 0 : 1);
}

unsigned long test_aux_current_line_count()
{
	return read_register(0x2573c);
}

void aux_reset()
{
	int dbg_me = 0, dbg_lc = 0;

	while(!test_aux_dscr_mbox_empty()) {
		write_register(0x25728, 0);
		dbg_me++;
	}
	while(test_aux_current_line_count()) {
		write_register(0x2573c, 0);
		dbg_lc++;
	}
	write_register(0x25744, 1);
}

void set_aux_720x480p(int srcw, int srch, int full)
{
	unsigned long value = 0;
	long reg = 0x100c0;

	set_bits(&value, reg, 0, 0, 0);
	set_bits(&value, reg, 1, 1, 0);
	set_bits(&value, reg, 2, 2, component_video ? 0 : 1);
	set_bits(&value, reg, 3, 3, 0);
	set_bits(&value, reg, 4, 4, 1);
	set_bits(&value, reg, 0xf, 8, 0);
	set_bits(&value, reg, 0x1b, 0x10, 0x235);	/* 565 */

	write_register(reg, value);

	set_aux_picture_origin(0, 2);
	set_aux_horizontal_scaler(0x2d0, srcw);	/* 720 */
	set_aux_vertical_scaler(0x1e3, srch, 0);	/* 483 */
}

void set_aux_720x480i(int srcw, int srch, int full)
{
	unsigned long value = 0;
	long reg = 0x100c0;

	set_bits(&value, reg, 0, 0, 0);
	set_bits(&value, reg, 1, 1, 1);
	set_bits(&value, reg, 2, 2, component_video ? 0 : 1);
	set_bits(&value, reg, 3, 3, 0);
	set_bits(&value, reg, 4, 4, 1);
	set_bits(&value, reg, 0xf, 8, 0);
	set_bits(&value, reg, 0x1b, 0x10, 0x235);	/* 565 */

	write_register(reg, value);

	set_aux_picture_origin(0, 0);
	set_aux_horizontal_scaler(0x2d0, srcw);	/* 720 */
	set_aux_vertical_scaler(0xf3, srch, 1);	/* 243 */
}

void set_aux_1280x720p(int srcw, int srch, int full)
{
	unsigned long value = 0;
	long reg = 0x100c0;

	set_bits(&value, reg, 0, 0, 0);
	set_bits(&value, reg, 1, 1, 0);
	set_bits(&value, reg, 2, 2, component_video ? 0 : 1);
	set_bits(&value, reg, 3, 3, 0);
	set_bits(&value, reg, 4, 4, 1);
	set_bits(&value, reg, 0xf, 8, 0);
	set_bits(&value, reg, 0x1b, 0x10, 0x2d1);	/* 721 */

	write_register(reg, value);

	if(full) {
		set_aux_picture_origin(0, 0);
		set_aux_horizontal_scaler(0x500, srcw);	/* 1280 */
		set_aux_vertical_scaler(0x2d0, srch, 0);	/* 720 */
	} else {
		set_aux_picture_origin(0, 0xc);
		set_aux_horizontal_scaler(0x500, srcw);	/* 1280 */
		set_aux_vertical_scaler(0x2b7, srch, 0);	/* 695 */
	}
}

void set_aux_1920x1080i(int srcw, int srch, int full)
{
	unsigned long value = 0;
	long reg = 0x100c0;

	set_bits(&value, reg, 0, 0, 0);
	set_bits(&value, reg, 1, 1, 1);
	set_bits(&value, reg, 2, 2, component_video ? 0 : 1);
	set_bits(&value, reg, 3, 3, 0);
	set_bits(&value, reg, 4, 4, 1);
	set_bits(&value, reg, 0xf, 8, 0);
	set_bits(&value, reg, 0x1b, 0x10, 0x271);	/* 625 */

	write_register(reg, value);

	if(full) {
		set_aux_picture_origin(0, 0);
		set_aux_horizontal_scaler(0x780, srcw);	/* 1920 */
		set_aux_vertical_scaler(0x21c, srch, 1);	/* 540 */
	} else {
		set_aux_picture_origin(0, 0xc);
		set_aux_horizontal_scaler(0x780, srcw);	/* 1920 */
		set_aux_vertical_scaler(0x208, srch, 1);	/* 520 */
	}
}

void set_aux_1024x768p(int srcw, int srch, int full)
{
	unsigned long value = 0;
	long reg = 0x100c0;

	set_bits(&value, reg, 0, 0, 0);
	set_bits(&value, reg, 1, 1, 0);
	set_bits(&value, reg, 2, 2, component_video ? 0 : 1);
	set_bits(&value, reg, 3, 3, 0);
	set_bits(&value, reg, 4, 4, 1);
	set_bits(&value, reg, 0xf, 8, 0);
	set_bits(&value, reg, 0x1b, 0x10, 0x2d1);	/* 721 */

	write_register(reg, value);

	set_aux_picture_origin(0, 2);
	set_aux_horizontal_scaler(0x400, srcw);	/* 1024 */
	set_aux_vertical_scaler(0x300, srch, 0);	/* 768 */
}

void set_aux_1440x1080i(int srcw, int srch, int full)
{
	unsigned long value = 0;
	long reg = 0x100c0;

	set_bits(&value, reg, 0, 0, 0);
	set_bits(&value, reg, 1, 1, 1);
	set_bits(&value, reg, 2, 2, component_video ? 0 : 1);
	set_bits(&value, reg, 3, 3, 0);
	set_bits(&value, reg, 4, 4, 1);
	set_bits(&value, reg, 0xf, 8, 0);
	set_bits(&value, reg, 0x1b, 0x10, 0x271);	/* 625 */

	write_register(reg, value);

	set_aux_picture_origin(0, 2);
	set_aux_horizontal_scaler(0x5a0, srcw);	/* 1440 */
	set_aux_vertical_scaler(0x21c, srch, 1);	/* 540 */
}

void set_sync_720x480i()
{
	unsigned long value = 0;
	unsigned long reg;
	
	
	reg = 0x10014;
	set_bits(&value, reg, 0, 0, 0);			/* A - 0 */
	set_bits(&value, reg, 1, 1, 0);			/* B - 0 */
	set_bits(&value, reg, 2, 2, 1);			/* C - 1 */
	set_bits(&value, reg, 3, 3, 0);			/* D - 0 */
	set_bits(&value, reg, 0xf, 6, 0xf4);		/* E - 244 */
	set_bits(&value, reg, 0x1a, 0x10, 0x2d0);	/* F - 720 */
	set_bits(&value, reg, 0x1b, 0x1b, 1);		/* G - 1 */
	set_bits(&value, reg, 0x1c, 0x1c, 1);		/* H - 1 */
	set_bits(&value, reg, 0x1d, 0x1d, 0);		/* I - 0 */
	set_bits(&value, reg, 0x1e, 0x1e, 0);		/* J - 0 */
	write_register(reg, value);

	reg = 0x10018;
	value = 0;
	set_bits(&value, reg, 8, 0, 0xf);		/* K - 15 */
	set_bits(&value, reg, 9, 9, invert_hsync ? 1 : 0); /* L - 0 */
	set_bits(&value, reg, 0xa, 0xa, invert_vsync ? 1 : 0); /* M - 0 */
	set_bits(&value, reg, 0x12, 0xc, 0x3f);		/* N - 63 */
	set_bits(&value, reg, 0x1c, 0x14, 0x38);	/* O - 56 */
	write_register(reg, value);

	reg = 0x1001c;
	value = 0;
	set_bits(&value, reg, 2, 0, 3);			/* P - 3 */
	set_bits(&value, reg, 0xb, 4, 0xe);		/* Q - 14 */
	set_bits(&value, reg, 0x16, 0xc, 0xf3);		/* R - 243 */
	set_bits(&value, reg, 0x1b, 0x18, 2);		/* S - 2 */
	set_bits(&value, reg, 0x1c, 0x1c, 0);		/* T - 1 */
	write_register(reg, value);

	reg = 0x10020;
	value = 0;
	set_bits(&value, reg, 2, 0, 4);			/* U - 4 */
	set_bits(&value, reg, 0xb, 4, 0xd);		/* V - 13 */
	set_bits(&value, reg, 0x16, 0xc, 0xf3);		/* W - 243 */
	set_bits(&value, reg, 0x1b, 0x18, 3);		/* X - 3 */
	write_register(reg, value);

	reg = 0x10028;
	value = 0;
	set_bits(&value, reg, 0xb, 0, 0x28);		/* BB - 40 */
	set_bits(&value, reg, 0x18, 0x18, 1);		/* CC - 1 */
	write_register(reg, value);
	
	reg = 0x10024;
	value = 0;
	set_bits(&value, reg, 0x15, 0, 5);		/* Y - 5 */
	set_bits(&value, reg, 0x18, 0x18, 1);		/* Z - 1 */
	set_bits(&value, reg, 0x1f, 0x1c, 4);		/* AA - 4 */
	write_register(reg, value);
}

void set_sync_720x480p()
{
	unsigned long value = 0;
	long reg = 0x10014;

	set_bits(&value, reg, 0, 0, 0);			/* A - 0 */
	set_bits(&value, reg, 1, 1, 0);			/* B - 0 */
	set_bits(&value, reg, 2, 2, 1);			/* C - 1 */
	set_bits(&value, reg, 3, 3, 1);			/* D - 1 */
	set_bits(&value, reg, 0xf, 6, 0xf4);		/* E - 244 */
	set_bits(&value, reg, 0x1a, 0x10, 0x2d0);	/* F - 720 */
	set_bits(&value, reg, 0x1b, 0x1b, 1);		/* G - 1 */
	set_bits(&value, reg, 0x1c, 0x1c, 1);		/* H - 1 */
	set_bits(&value, reg, 0x1d, 0x1d, 0);		/* I - 0 */
	set_bits(&value, reg, 0x1e, 0x1e, 0);		/* J - 0 */
	write_register(reg, value);

	reg += 4;
	value = 0;
	set_bits(&value, reg, 8, 0, 0xe);		/* K - 14 */
	set_bits(&value, reg, 9, 9, invert_hsync ? 1 : 0); /* L - 0 */
	set_bits(&value, reg, 0xa, 0xa, invert_vsync ? 1 : 0); /* M - 0 */
	set_bits(&value, reg, 0x12, 0xc, 0x3f);		/* N - 63 */
	set_bits(&value, reg, 0x1c, 0x14, 0x39);	/* O - 56 */
	write_register(reg, value);

	reg = 0x1001c;
	value = 0;
	set_bits(&value, reg, 2, 0, 6);			/* P - 6 */
	set_bits(&value, reg, 0xb, 4, 0x1e);		/* Q - 14 */
	set_bits(&value, reg, 0x16, 0xc, 0x1e3);	/* R - 483 */
	set_bits(&value, reg, 0x1b, 0x18, 6);		/* S - 6 */
	set_bits(&value, reg, 0x1c, 0x1c, 0);		/* T - 0 */
	write_register(reg, value);

	reg += 4;
	value = 0;
	set_bits(&value, reg, 2, 0, 0);			/* U - 0 */
	set_bits(&value, reg, 0xb, 4, 0);		/* V - 0 */
	set_bits(&value, reg, 0x16, 0xc, 0);		/* W - 0 */
	set_bits(&value, reg, 0x1b, 0x18, 0);		/* X - 0 */
	set_bits(&value, reg, 0x1c, 0x1c, 0);		/* Y - 0 */
	write_register(reg, value);

	reg = 0x10028;
	value = 0;
	set_bits(&value, reg, 0xb, 0, 0x28);		/* CC - 40 */
	set_bits(&value, reg, 0x18, 0x18, 1);		/* DD - 1 */
	write_register(reg, value);
	
	reg -= 4;
	value = 0;
	set_bits(&value, reg, 0x15, 0, 5);		/* Z - 5 */
	set_bits(&value, reg, 0x18, 0x18, 1);		/* AA - 1 */
	set_bits(&value, reg, 0x1f, 0x1c, 4);		/* BB - 4 */
	write_register(reg, value);
}

void set_sync_1920x1080i()
{
	unsigned long value = 0;
	long reg = 0x10014;

	set_bits(&value, reg, 0, 0, 0);			/* A - 0 */
	set_bits(&value, reg, 1, 1, 1);			/* B - 1 */
	set_bits(&value, reg, 2, 2, 1);			/* C - 1 */
	set_bits(&value, reg, 3, 3, 0);			/* D - 0 */
	set_bits(&value, reg, 0xf, 6, 0x368);		/* E - 872 */
	set_bits(&value, reg, 0x1a, 0x10, 0x780);	/* F - 1920 */
	set_bits(&value, reg, 0x1b, 0x1b, 1);		/* G - 1 */
	set_bits(&value, reg, 0x1c, 0x1c, 1);		/* H - 1 */
	set_bits(&value, reg, 0x1d, 0x1d, 0);		/* I - 0 */
	set_bits(&value, reg, 0x1e, 0x1e, 0);		/* J - 0 */
	write_register(reg, value);

	reg = 0x10018;
	value = 0;
	set_bits(&value, reg, 8, 0, 0x2c);		/* K - 44 */
	set_bits(&value, reg, 9, 9, invert_hsync ? 1 : 0); /* L - 0 */
	set_bits(&value, reg, 0xa, 0xa, invert_vsync ? 1 : 0); /* M - 0 */
	set_bits(&value, reg, 0x12, 0xc, 0x58);		/* N - 88 */
	set_bits(&value, reg, 0x1c, 0x14, 0x8c);	/* O - 140 */
	write_register(reg, value);

	reg = 0x1001c;
	value = 0;
	set_bits(&value, reg, 2, 0, 5);			/* P - 5 */
	set_bits(&value, reg, 0xb, 4, 0xf);		/* Q - 15 */
	set_bits(&value, reg, 0x16, 0xc, 0x21c);	/* R - 540 */
	set_bits(&value, reg, 0x1b, 0x18, 2);		/* S - 2 */
	set_bits(&value, reg, 0x1c, 0x1c, 0);		/* T - 0 */
	write_register(reg, value);
	
	reg = 0x10020;
	value = 0;
	set_bits(&value, reg, 2, 0, 5);			/* U - 5 */
	set_bits(&value, reg, 0xb, 4, 0x10);		/* V - 16 */
	set_bits(&value, reg, 0x16, 0xc, 0x21c);	/* W - 540 */
	set_bits(&value, reg, 0x1b, 0x18, 2);		/* X - 2 */
	write_register(reg, value);

	reg = 0x10028;
	value = 0;
	set_bits(&value, reg, 0xb, 0, 0x28);		/* BB - 40 */
	set_bits(&value, reg, 0x18, 0x18, 1);		/* CC - 1 */
	write_register(reg, value);
	
	reg = 0x10024;
	value = 0;
	set_bits(&value, reg, 0x15, 0, 5);		/* Y - 5 */
	set_bits(&value, reg, 0x18, 0x18, 1);		/* Z - 1 */
	set_bits(&value, reg, 0x1f, 0x1c, 4);		/* AA - 4 */
	write_register(reg, value);
}

void set_sync_1024x768p()
{
	unsigned long value = 0;
	long reg;
	
	reg = 0x10014;
	set_bits(&value, reg, 0, 0, 0);			/* A - 0 */
	set_bits(&value, reg, 1, 1, 1);			/* B - 1 */
	set_bits(&value, reg, 2, 2, 1);			/* C - 1 */
	set_bits(&value, reg, 3, 3, 1);			/* D - 1 */
	set_bits(&value, reg, 0xf, 6, 0x21c);		/* E - 540 */
	set_bits(&value, reg, 0x1a, 0x10, 0x400);	/* F - 1024 */
	set_bits(&value, reg, 0x1b, 0x1b, 1);		/* G - 1 */
	set_bits(&value, reg, 0x1c, 0x1c, 1);		/* H - 1 */
	set_bits(&value, reg, 0x1d, 0x1d, 0);		/* I - 0 */
	set_bits(&value, reg, 0x1e, 0x1e, 0);		/* J - 0 */
	printf("1024x768p register %05lx: 0x%08lx\n", reg, value);
	write_register(reg, value);

	reg = 0x10018;
	value = 0;
	set_bits(&value, reg, 8, 0, 0x19);		/* K - 31 */
	set_bits(&value, reg, 9, 9, invert_hsync ? 1 : 0); /* L - 0 */
	set_bits(&value, reg, 0xa, 0xa, invert_vsync ? 1 : 0); /* M - 0 */
	set_bits(&value, reg, 0x12, 0xc, 0x7f);		/* N - 127 */
	set_bits(&value, reg, 0x1c, 0x14, 0xa0);	/* O - 160 */
	printf("1024x768p register %05lx: 0x%08lx\n", reg, value);
	write_register(reg, value);

	reg = 0x1001c;
	value = 0;
	set_bits(&value, reg, 2, 0, 6);			/* P - 6 */
	set_bits(&value, reg, 0xb, 4, 0x1d);		/* Q - 29 */
	set_bits(&value, reg, 0x16, 0xc, 0x300);	/* R - 768 */
	set_bits(&value, reg, 0x1b, 0x18, 3);		/* S - 3 */
	set_bits(&value, reg, 0x1c, 0x1c, 0);		/* T - 0 */
	printf("1024x768p register %05lx: 0x%08lx\n", reg, value);
	write_register(reg, value);

	reg = 0x10020;
	value = 0;
	set_bits(&value, reg, 2, 0, 0);			/* U - 0 */
	set_bits(&value, reg, 0xb, 4, 0);		/* V - 0 */
	set_bits(&value, reg, 0x16, 0xc, 0);		/* W - 0 */
	set_bits(&value, reg, 0x1b, 0x18, 0);		/* X - 0 */
	printf("1024x768p register %05lx: 0x%08lx\n", reg, value);
	write_register(reg, value);
	
	reg = 0x10028;
	value = 0;
	set_bits(&value, reg, 0xb, 0, 0x28);		/* BB - 40 */
	set_bits(&value, reg, 0x18, 0x18, 1);		/* CC - 1 */
	printf("1024x768p register %05lx: 0x%08lx\n", reg, value);
	write_register(reg, value);
	
	reg = 0x10024;
	value = 0;
	set_bits(&value, reg, 0x15, 0, 5);		/* Y - 5 */
	set_bits(&value, reg, 0x18, 0x18, 1);		/* Z - 1 */
	set_bits(&value, reg, 0x1f, 0x1c, 4);		/* AA - 4 */
	printf("1024x768p register %05lx: 0x%08lx\n", reg, value);
	write_register(reg, value);
}

int main(int argc, char *argv[])
{
	if(argc == 4) {
		invert_hsync = atol(argv[1]);
		invert_vsync = atol(argv[2]);
		component_video = atol(argv[3]);
	} else {
		printf("Usage: %s invert_hsync invert_vsync comp_video\n", argv[0]);
		return -1;
	}
	if(map_regspace()) {
		fprintf(stderr, "Unable to map register space\n");
		return -1;
	}
	aux_reset();
	/*
	set_sync_720x480i();
	set_aux_720x480i(1920, 1080, 1);
	*/
	set_sync_1024x768p();
	set_aux_1024x768p(1920, 1080, 1);

	/* 1920x1080i doesn't work (yet) */
	/*
	set_sync_1920x1080i();
	set_aux_1920x1080i(1920, 1080, 1);
	*/
	write_register(0x10000, 0x132);
	write_regbits(0x10190, 0xa, 0xa, 1);
	write_regbits(0x10194, 0xa, 0xa, 1);
	write_regbits(0x10198, 0xa, 0xa, 1);

	unmap_regspace();
	
	return 0;
}


