/*
 * VIP port stuff for TL880-based cards
 *
 * (c) 2003 Mike Bourgeous <nitrogen@slimetech.com>
 */
#include "tl880.h"


/* Set state of VIP port on card? */
void tl880_set_vip(struct tl880_dev *tl880dev, unsigned long vip_mode)
{
	unsigned long value;

	switch(vip_mode) {
		case 0:
			value = read_register(tl880dev, 0x10190);
			set_bits(&value, 0x10190, 7, 0, 0xff);
			set_bits(&value, 0x10190, 0x17, 0x10, 0xff);
			write_register(tl880dev, 0x10190, value);
			value = read_register(tl880dev, 0x10194);
			set_bits(&value, 0x10194, 7, 0, 0);
			set_bits(&value, 0x10194, 0x17, 0x10, 0);
			if(tl880dev->board_type == TL880_CARD_MYHD_MDP110) {
				set_bits(&value, 0x10194, 0x13, 0x11, 7);
			}
			write_register(tl880dev, 0x10194, value);
			tl880_set_gpio(tl880dev, 3, 1);
			tl880_set_gpio(tl880dev, 4, 1);
			break;
		case 1:
			value = read_register(tl880dev, 0x10190);
			set_bits(&value, 0x10190, 7, 0, 0xff);
			set_bits(&value, 0x10190, 0x17, 0x10, 0xff);
			write_register(tl880dev, 0x10190, value);
			value = read_register(tl880dev, 0x10194);
			set_bits(&value, 0x10194, 7, 0, 0);
			set_bits(&value, 0x10194, 0x17, 0x10, 0);
			if(tl880dev->board_type == TL880_CARD_MYHD_MDP110) {
				set_bits(&value, 0x10194, 0x13, 0x11, 7);
			}
			write_register(tl880dev, 0x10194, value);
			tl880_set_gpio(tl880dev, 3, 0);
			break;
		case 2:
			if(tl880dev->board_type != TL880_CARD_MYHD_MDP110) {
				tl880_set_gpio(tl880dev, 4, 0);
				value = read_register(tl880dev, 0x10190);
				set_bits(&value, 0x10190, 7, 0, 0);
				set_bits(&value, 0x10190, 0x17, 0x10, 0);
				write_register(tl880dev, 0x10190, value);
				value = read_register(tl880dev, 0x10194);
				set_bits(&value, 0x10194, 7, 0, 0xff);
				set_bits(&value, 0x10194, 0x17, 0x10, 0xff);
				write_register(tl880dev, 0x10194, value);
			}
			break;
		default:
			break;
	}
}


