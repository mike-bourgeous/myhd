/* 
 * Video mode table for TL880-based cards
 *
 * (c) 2003-2007 Mike Bourgeous <nitrogen at users.sourceforge.net>
 *
 * $Log: tl880modes.c,v $
 * Revision 1.4  2007/03/26 19:25:06  nitrogen
 * Added CVS log generation and updated copyrights and e-mail addresses.
 *
 */
#include "tl880.h"

struct tl880_mode_def tl880_mode_table[] = {
	{
		.a = 0,
		.b = 1,
		.c = 1,
		.prog_scan = 1,
		.e = 540,
		.xres = 1024,
		.g = 1,
		.h = 1,
		.i = 0,
		.j = 0,

		.h_backporch = 31,
		.inv_hsync = 0,
		.inv_vsync = 0,
		.h_synclen = 127,
		.h_frontporch = 160,

		.v_synclen_0 = 6,
		.v_frontporch_0 = 29,
		.yres_0 = 768,
		.v_backporch_0 = 3,
		.ntsc_flag = 0,

		.v_synclen_1 = 0,
		.v_frontporch_1 = 0,
		.yres_1 = 0,
		.v_backporch_1 = 0,

		.y = 5,
		.z = 1,
		.aa = 4,

		.bb = 40,
		.cc = 1,
		.dd = 0x01151bb0
	}
};

