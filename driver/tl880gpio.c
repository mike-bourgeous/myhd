/* 
 * GPIO stuff for TL880-based cards
 *
 * (c) 2003 Mike Bourgeous <nitrogen@slimetech.com>
 */
#include "tl880.h"

unsigned char tl880_set_gpio(struct tl880_dev *tl880dev, unsigned int gpio_line, unsigned char state)
{
	if(!tl880dev) {
		return 0;
	}

	// printk(KERN_DEBUG "tl880: gpio %i set to %i\n", gpio_line, state);
	
	state = state ? 1 : 0;
	
	if(tl880dev->board_type == TL880_CARD_JANUS) {
		switch(gpio_line) {
			case 0:
				write_regbits(tl880dev, 0x10198, 8, 8, state);
				return 1;
				break;
			case 1:
				if(state == 1) {
					write_regbits(tl880dev, 0x10198, 0xa, 0xa, ~state);
					write_regbits(tl880dev, 0x10198, 9, 9, state);
					return 1;
				} else {
					write_regbits(tl880dev, 0x10198, 9, 9, state);
					write_regbits(tl880dev, 0x10198, 0xa, 0xa, ~state);
					return 1;
				}
				break;
			case 2:
				if(state == 1) {
					write_regbits(tl880dev, 0x10198, 9, 9, ~state);
					write_regbits(tl880dev, 0x10198, 0xa, 0xa, state);
					return 1;
				} else {
					write_regbits(tl880dev, 0x10198, 0xa, 0xa, state);
					write_regbits(tl880dev, 0x10198, 9, 9, ~state);
					return 1;
				}
				break;
			case 3:
				if(state == 1) {
					write_regbits(tl880dev, 0x10198, 0xb, 0xb, state);
					write_regbits(tl880dev, 0x10198, 0xe, 0xe, ~state);
					return 1;
				} else {
					write_regbits(tl880dev, 0x10198, 0xe, 0xe, ~state);
					write_regbits(tl880dev, 0x10198, 0xb, 0xb, state);
					return 1;
				}
				break;
			case 4:
				if(state == 1) {
					write_regbits(tl880dev, 0x10198, 0xe, 0xe, state);
					write_regbits(tl880dev, 0x10198, 0xb, 0xb, ~state);
					return 1;
				} else {
					write_regbits(tl880dev, 0x10198, 0xb, 0xb, ~state);
					write_regbits(tl880dev, 0x10198, 0xe, 0xe, state);
					return 1;
				}
				break;
			case 8:
				if(state == 0) {
					write_regbits(tl880dev, 0x10194, 0xc, 0xc, 1);
					write_regbits(tl880dev, 0x10198, 0xc, 0xc, 0);
					return 1;
				}
				
				if(state == 1) {
					write_regbits(tl880dev, 0x10194, 0xc, 0xc, 1);
					write_regbits(tl880dev, 0x10198, 0xc, 0xc, 1);
					return 1;
				} else {
					write_regbits(tl880dev, 0x10194, 0xc, 0xc, 0);
					return 1;
				}
				break;
			case 9:
				if(state == 0) {
					write_regbits(tl880dev, 0x10194, 0xd, 0xd, 1);
					write_regbits(tl880dev, 0x10198, 0xd, 0xd, 0);
					return 1;
				}
				
				if(state == 1) {
					write_regbits(tl880dev, 0x10194, 0xd, 0xd, 1);
					write_regbits(tl880dev, 0x10198, 0xd, 0xd, 1);
					return 1;
				} else {
					write_regbits(tl880dev, 0x10194, 0xd, 0xd, 0);
					return 1;
				}
				break;
			default:
				return 1;
				break;
		}
	} else if(tl880dev->board_type == TL880_CARD_MYHD_MDP100A) {
		switch(gpio_line) {
			case 0:
				write_regbits(tl880dev, 0x10198, 8, 8, state);
				return 1;
				break;
			case 1:
				if(state == 1) {
					write_regbits(tl880dev, 0x10198, 0xa, 0xa, ~state);
					write_regbits(tl880dev, 0x10198, 9, 9, state);
					return 1;
				} else {
					write_regbits(tl880dev, 0x10198, 9, 9, state);
					write_regbits(tl880dev, 0x10198, 0xa, 0xa, ~state);
					return 1;
				}
				break;
			case 2:
				if(state == 1) {
					write_regbits(tl880dev, 0x10198, 9, 9, ~state);
					write_regbits(tl880dev, 0x10198, 0xa, 0xa, state);
					return 1;
				} else {
					write_regbits(tl880dev, 0x10198, 0xa, 0xa, state);
					write_regbits(tl880dev, 0x10198, 9, 9, ~state);
					return 1;
				}
				break;
			case 3:
				if(state != 1) {
					write_regbits(tl880dev, 0x10198, 0xc, 0xc, ~state);
					write_regbits(tl880dev, 0x10198, 0xb, 0xb, state);
					return 1;
				} else {
					write_regbits(tl880dev, 0x10198, 0xb, 0xb, state);
					return 1;
				}
				break;
			case 4:
				if(state == 1) {
					write_regbits(tl880dev, 0x10198, 0xc, 0xc, state);
					write_regbits(tl880dev, 0x10198, 0xd, 0xd, state);
					return 1;
				} else {
					write_regbits(tl880dev, 0x10198, 0xb, 0xb, ~state);
					write_regbits(tl880dev, 0x10198, 0xc, 0xc, state);
					write_regbits(tl880dev, 0x10198, 0xd, 0xd, state);
					return 1;
				}
				break;
			case 5:
				write_regbits(tl880dev, 0x10198, 0xd, 0xd, state);
				return 1;
				break;
			case 6:
				write_regbits(tl880dev, 0x10198, 0xe, 0xe, state);
				return 1;
				break;
			default:
				return 1;
				break;
		}
	} else if(tl880dev->board_type >= 3 /* TL880_CARD_MDP100 */ && tl880dev->board_type < 0x10 /* First Hauppauge */) {
		if(tl880dev->board_type == TL880_CARD_MYHD_MDP120) {
			printk(KERN_WARNING "tl880: MDP120 GPIO not implemented, using MDP110B GPIO.  May not be correct.\n");
		}
		if(gpio_line > 9) {
			return 1;
		}
		switch(gpio_line) {
			case 0:
				write_regbits(tl880dev, 0x10198, 8, 8, state);
				return 1;
				break;
			case 1:
				return 1;
				break;
			case 2:
				write_regbits(tl880dev, 0x10198, 0xa, 0xa, state);
				return 1;
				break;
			case 3:
				if(!state) {
					write_regbits(tl880dev, 0x10198, 0xc, 0xc, ~state);
					write_regbits(tl880dev, 0x10198, 0xb, 0xb, state);
					return 1;
				} else {
					write_regbits(tl880dev, 0x10198, 0xb, 0xb, state);
					return 1;
				}
				break;
			case 4:
				if(state == 1) {
					write_regbits(tl880dev, 0x10198, 0xc, 0xc, state);
					write_regbits(tl880dev, 0x10198, 0xd, 0xd, state);
					return 1;
				} else {
					write_regbits(tl880dev, 0x10198, 0xb, 0xb, ~state);
					write_regbits(tl880dev, 0x10198, 0xc, 0xc, state);
					write_regbits(tl880dev, 0x10198, 0xd, 0xd, state);
					return 1;
				}
				break;
			case 5:
				write_regbits(tl880dev, 0x10198, 0xd, 0xd, state);
				return 1;
				break;
			case 6:
				write_regbits(tl880dev, 0x10198, 0xe, 0xe, state);
				return 1;
				break;
			case 7:
				return 1;
				break;
			case 8:
				if(state == 0) {
					write_regbits(tl880dev, 0x10194, 9, 9, 1);
					write_regbits(tl880dev, 0x10198, 9, 9, 0);
					return 1;
				} else {
					if(state == 1) {
						write_regbits(tl880dev, 0x10194, 9, 9, 1);
						write_regbits(tl880dev, 0x10198, 9, 9, 1);
						return 1;
					} else {
						write_regbits(tl880dev, 0x10194, 9, 9, 0);
						return 1;
					}
				}
				break;
			case 9:
				if(state == 0) {
					write_regbits(tl880dev, 0x10194, 0xf, 0xf, 1);
					write_regbits(tl880dev, 0x10198, 0xf, 0xf, 0);
					return 1;
				} else {
					if(state == 1) {
						write_regbits(tl880dev, 0x10194, 0xf, 0xf, 1);
						write_regbits(tl880dev, 0x10198, 0xf, 0xf, 1);
						return 1;
					} else {
						write_regbits(tl880dev, 0x10194, 0xf, 0xf, 0);
						return 1;
					}
				}
				break;
		}
	} else {
		printk(KERN_WARNING "tl880: Attempt to set GPIO on card for which GPIO routines not yet written\n");
		printk(KERN_WARNING "tl880: board_type=%i\n", tl880dev->board_type);
	}

	return 1;
}

