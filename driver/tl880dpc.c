/* 
 * Video mode stuff for TL880 cards
 *
 * (c) 2003 Mike Bourgeous <nitrogen@slimetech.com>
 */
#include "tl880.h"

void tl880_set_dpc_pll_const(struct tl880_dev *tl880dev, unsigned long a, unsigned char b, unsigned char c)
{
	write_register(tl880dev, 0x5800, (((((a & 0x3ff) << 8) | (b & 0x1f)) << 4) | (c & 3)) << 4);
}

/* XXX: This function is not finished! */
void tl880_init_dpc_pll(struct tl880_dev *tl880dev)
{
	int i;
	
	write_register(tl880dev, 0x10014, 0x80000000);

	/* set_dpc_clock */
	{
		/* write_register(tl880dev, 0x5800, 0x60070); */
		/* set_dpc_pll_const(0x14, 2, 1) */
		{
			int arg0 = 0x14, arg4 = 0x2, arg8 = 0x1;

			arg0 &= 0x3ff;
			arg0 <<= 8;
			arg4 &= 0x1f;
			arg0 += arg4;
			arg0 <<= 4;
			arg8 &= 3;
			arg0 += arg8;
			arg0 <<= 4;
			//write_register(tl880dev, 0x5800, arg0);
			/* This value makes 1024x768p exactly 60Hz 48.3kHz */
			write_register(tl880dev, 0x5800, 0x00160320);
		}
		write_register(tl880dev, 0x5d14, 0x0);
	}

	for(i = 0; i < 10; i++) {
		read_register(tl880dev, 0x10014);
	}

	write_register(tl880dev, 0x10014, 0);
}


