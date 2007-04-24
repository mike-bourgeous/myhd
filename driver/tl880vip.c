/*
 * VIP port stuff for TL880-based cards
 *
 * (c) 2003-2007 Mike Bourgeous <nitrogen at users.sourceforge.net>
 *
 * $Log: tl880vip.c,v $
 * Revision 1.5  2007/04/24 06:32:14  nitrogen
 * Changed most int/long types to explicit 32-bit sizes.  Fixed compilation and execution on 64-bit CPUs.
 *
 * Revision 1.4  2007/03/26 19:25:06  nitrogen
 * Added CVS log generation and updated copyrights and e-mail addresses.
 *
 */
#include "tl880.h"


/* Set state of VIP port on card? */
void tl880_set_vip(struct tl880_dev *tl880dev, unsigned long vip_mode)
{
	u32 value;

	switch(vip_mode) {
		case 0:
			value = tl880_read_register(tl880dev, 0x10190);
			set_bits(&value, 0x10190, 7, 0, 0xff);
			set_bits(&value, 0x10190, 0x17, 0x10, 0xff);
			tl880_write_register(tl880dev, 0x10190, value);
			value = tl880_read_register(tl880dev, 0x10194);
			set_bits(&value, 0x10194, 7, 0, 0);
			set_bits(&value, 0x10194, 0x17, 0x10, 0);
			if(tl880dev->card_type == TL880_CARD_MYHD_MDP110) {
				set_bits(&value, 0x10194, 0x13, 0x11, 7);
			}
			tl880_write_register(tl880dev, 0x10194, value);
			tl880_set_gpio(tl880dev, 3, 1);
			tl880_set_gpio(tl880dev, 4, 1);
			break;
		case 1:
			value = tl880_read_register(tl880dev, 0x10190);
			set_bits(&value, 0x10190, 7, 0, 0xff);
			set_bits(&value, 0x10190, 0x17, 0x10, 0xff);
			tl880_write_register(tl880dev, 0x10190, value);
			value = tl880_read_register(tl880dev, 0x10194);
			set_bits(&value, 0x10194, 7, 0, 0);
			set_bits(&value, 0x10194, 0x17, 0x10, 0);
			if(tl880dev->card_type == TL880_CARD_MYHD_MDP110) {
				set_bits(&value, 0x10194, 0x13, 0x11, 7);
			}
			tl880_write_register(tl880dev, 0x10194, value);
			tl880_set_gpio(tl880dev, 3, 0);
			break;
		case 2:
			if(tl880dev->card_type != TL880_CARD_MYHD_MDP110) {
				tl880_set_gpio(tl880dev, 4, 0);
				value = tl880_read_register(tl880dev, 0x10190);
				set_bits(&value, 0x10190, 7, 0, 0);
				set_bits(&value, 0x10190, 0x17, 0x10, 0);
				tl880_write_register(tl880dev, 0x10190, value);
				value = tl880_read_register(tl880dev, 0x10194);
				set_bits(&value, 0x10194, 7, 0, 0xff);
				set_bits(&value, 0x10194, 0x17, 0x10, 0xff);
				tl880_write_register(tl880dev, 0x10194, value);
			}
			break;
		default:
			break;
	}
}


