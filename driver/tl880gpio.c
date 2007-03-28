/* 
 * GPIO stuff for TL880-based cards
 *
 * (c) 2003-2007 Mike Bourgeous <nitrogen at users.sourceforge.net>
 * (c) 2007 Jason P. Matthews
 *
 * $Log: tl880gpio.c,v $
 * Revision 1.10  2007/03/28 08:01:30  nitrogen
 * Initialization improvements, VPX improvements, minor comment and error message tweaks, better docs
 *
 * Revision 1.9  2007/03/26 19:45:17  nitrogen
 * Reformatted to match rest of driver, corrected to use tl880_vpx_read_fp's return value.
 *
 * Revision 1.8  2007/03/26 19:23:56  nitrogen
 * Added GPIO patch by Jason P. Matthews.
 *
 */
#include "tl880.h"

unsigned char tl880_set_gpio(struct tl880_dev *tl880dev, unsigned int gpio_line, unsigned char state)
{
	if(CHECK_NULL(tl880dev)) {
		return 0;
	}

	if(debug) {
		printk(KERN_DEBUG "tl880: gpio %i set to %i\n", gpio_line, state);
	}
	
	state = state ? 1 : 0;
	
	if(tl880dev->card_type == TL880_CARD_JANUS) {
		switch(gpio_line) {
			case 0:
				tl880_write_regbits(tl880dev, 0x10198, 8, 8, state);
				return 1;
				break;
			case 1:
				if(state == 1) {
					tl880_write_regbits(tl880dev, 0x10198, 0xa, 0xa, ~state);
					tl880_write_regbits(tl880dev, 0x10198, 9, 9, state);
					return 1;
				} else {
					tl880_write_regbits(tl880dev, 0x10198, 9, 9, state);
					tl880_write_regbits(tl880dev, 0x10198, 0xa, 0xa, ~state);
					return 1;
				}
				break;
			case 2: // VGA output enable
				if(state == 1) {
					tl880_write_regbits(tl880dev, 0x10198, 9, 9, ~state);
					tl880_write_regbits(tl880dev, 0x10198, 0xa, 0xa, state);
					return 1;
				} else {
					tl880_write_regbits(tl880dev, 0x10198, 0xa, 0xa, state);
					tl880_write_regbits(tl880dev, 0x10198, 9, 9, ~state);
					return 1;
				}
				break;
			case 3:
				if(state == 1) {
					tl880_write_regbits(tl880dev, 0x10198, 0xb, 0xb, state);
					tl880_write_regbits(tl880dev, 0x10198, 0xe, 0xe, ~state);
					return 1;
				} else {
					tl880_write_regbits(tl880dev, 0x10198, 0xe, 0xe, ~state);
					tl880_write_regbits(tl880dev, 0x10198, 0xb, 0xb, state);
					return 1;
				}
				break;
			case 4:
				if(state == 1) {
					tl880_write_regbits(tl880dev, 0x10198, 0xe, 0xe, state);
					tl880_write_regbits(tl880dev, 0x10198, 0xb, 0xb, ~state);
					return 1;
				} else {
					tl880_write_regbits(tl880dev, 0x10198, 0xb, 0xb, ~state);
					tl880_write_regbits(tl880dev, 0x10198, 0xe, 0xe, state);
					return 1;
				}
				break;
			case 8:
				if(state == 0) {
					tl880_write_regbits(tl880dev, 0x10194, 0xc, 0xc, 1);
					tl880_write_regbits(tl880dev, 0x10198, 0xc, 0xc, 0);
					return 1;
				}
				
				if(state == 1) {
					tl880_write_regbits(tl880dev, 0x10194, 0xc, 0xc, 1);
					tl880_write_regbits(tl880dev, 0x10198, 0xc, 0xc, 1);
					return 1;
				} else {
					tl880_write_regbits(tl880dev, 0x10194, 0xc, 0xc, 0);
					return 1;
				}
				break;
			case 9:
				if(state == 0) {
					tl880_write_regbits(tl880dev, 0x10194, 0xd, 0xd, 1);
					tl880_write_regbits(tl880dev, 0x10198, 0xd, 0xd, 0);
					return 1;
				}
				
				if(state == 1) {
					tl880_write_regbits(tl880dev, 0x10194, 0xd, 0xd, 1);
					tl880_write_regbits(tl880dev, 0x10198, 0xd, 0xd, 1);
					return 1;
				} else {
					tl880_write_regbits(tl880dev, 0x10194, 0xd, 0xd, 0);
					return 1;
				}
				break;
			default:
				return 1;
				break;
		}
	} else if(tl880dev->card_type == TL880_CARD_MYHD_MDP100A) {
		switch(gpio_line) {
			case 0:
				tl880_write_regbits(tl880dev, 0x10198, 8, 8, state);
				return 1;
				break;
			case 1:
				if(state == 1) {
					tl880_write_regbits(tl880dev, 0x10198, 0xa, 0xa, ~state);
					tl880_write_regbits(tl880dev, 0x10198, 9, 9, state);
					return 1;
				} else {
					tl880_write_regbits(tl880dev, 0x10198, 9, 9, state);
					tl880_write_regbits(tl880dev, 0x10198, 0xa, 0xa, ~state);
					return 1;
				}
				break;
			case 2: // VGA output enable
				if(state == 1) {
					tl880_write_regbits(tl880dev, 0x10198, 9, 9, ~state);
					tl880_write_regbits(tl880dev, 0x10198, 0xa, 0xa, state);
					return 1;
				} else {
					tl880_write_regbits(tl880dev, 0x10198, 0xa, 0xa, state);
					tl880_write_regbits(tl880dev, 0x10198, 9, 9, ~state);
					return 1;
				}
				break;
			case 3:
				if(state != 1) {
					tl880_write_regbits(tl880dev, 0x10198, 0xc, 0xc, ~state);
					tl880_write_regbits(tl880dev, 0x10198, 0xb, 0xb, state);
					return 1;
				} else {
					tl880_write_regbits(tl880dev, 0x10198, 0xb, 0xb, state);
					return 1;
				}
				break;
			case 4:
				if(state == 1) {
					tl880_write_regbits(tl880dev, 0x10198, 0xc, 0xc, state);
					tl880_write_regbits(tl880dev, 0x10198, 0xd, 0xd, state);
					return 1;
				} else {
					tl880_write_regbits(tl880dev, 0x10198, 0xb, 0xb, ~state);
					tl880_write_regbits(tl880dev, 0x10198, 0xc, 0xc, state);
					tl880_write_regbits(tl880dev, 0x10198, 0xd, 0xd, state);
					return 1;
				}
				break;
			case 5:
				tl880_write_regbits(tl880dev, 0x10198, 0xd, 0xd, state);
				return 1;
				break;
			case 6:
				tl880_write_regbits(tl880dev, 0x10198, 0xe, 0xe, state);
				return 1;
				break;
			default:
				return 1;
				break;
		}
	} else if (tl880dev->card_type >= TL880_CARD_MYHD_MDP100 && tl880dev->card_type < TL880_CARD_MYHD_MDP120) {
		if(gpio_line > 9) {
			return 1;
		}
		switch(gpio_line) {
			case 0:
				tl880_write_regbits(tl880dev, 0x10198, 8, 8, state);
				return 1;
				break;
			case 1:
				return 1;
				break;
			case 2: // VGA output enable
				tl880_write_regbits(tl880dev, 0x10198, 0xa, 0xa, state);
				return 1;
				break;
			case 3:
				if(!state) {
					tl880_write_regbits(tl880dev, 0x10198, 0xc, 0xc, ~state);
					tl880_write_regbits(tl880dev, 0x10198, 0xb, 0xb, state);
					return 1;
				} else {
					tl880_write_regbits(tl880dev, 0x10198, 0xb, 0xb, state);
					return 1;
				}
				break;
			case 4:
				if(state == 1) {
					tl880_write_regbits(tl880dev, 0x10198, 0xc, 0xc, state);
					tl880_write_regbits(tl880dev, 0x10198, 0xd, 0xd, state);
					return 1;
				} else {
					tl880_write_regbits(tl880dev, 0x10198, 0xb, 0xb, ~state);
					tl880_write_regbits(tl880dev, 0x10198, 0xc, 0xc, state);
					tl880_write_regbits(tl880dev, 0x10198, 0xd, 0xd, state);
					return 1;
				}
				break;
			case 5:
				tl880_write_regbits(tl880dev, 0x10198, 0xd, 0xd, state);
				return 1;
				break;
			case 6:
				tl880_write_regbits(tl880dev, 0x10194, 0xe, 0xe, 1);
				tl880_write_regbits(tl880dev, 0x10198, 0xe, 0xe, state);
				return 1;
				break;
			case 7:
				return 1;
				break;
			case 8:
				if(state == 0) {
					tl880_write_regbits(tl880dev, 0x10194, 9, 9, 1);
					tl880_write_regbits(tl880dev, 0x10198, 9, 9, 0);
					return 1;
				} else {
					if(state == 1) {
						tl880_write_regbits(tl880dev, 0x10194, 9, 9, 1);
						tl880_write_regbits(tl880dev, 0x10198, 9, 9, 1);
						return 1;
					} else {
						tl880_write_regbits(tl880dev, 0x10194, 9, 9, 0);
						return 1;
					}
				}
				break;
			case 9:
				if(state == 0) {
					tl880_write_regbits(tl880dev, 0x10194, 0xf, 0xf, 1);
					tl880_write_regbits(tl880dev, 0x10198, 0xf, 0xf, 0);
					return 1;
				} else {
					if(state == 1) {
						tl880_write_regbits(tl880dev, 0x10194, 0xf, 0xf, 1);
						tl880_write_regbits(tl880dev, 0x10198, 0xf, 0xf, 1);
						return 1;
					} else {
						tl880_write_regbits(tl880dev, 0x10194, 0xf, 0xf, 0);
						return 1;
					}
				}
				break;
		}
	} else if (tl880dev->card_type == TL880_CARD_MYHD_MDP120 || tl880dev->card_type == TL880_CARD_MYHD_MDP130) {
		int gpio_value;
		if (gpio_line != 0) {
			/* The MDP-120 uses the GPIO lines on the VPX chip */
			if((gpio_value = tl880_vpx_read_fp(tl880dev, 0x154)) == (short)(-1)) {
				return 0;
			} else {
				switch(gpio_line) {
					case 1: 
					case 3: 
					case 4: 
					case 5: 
					case 7: 
					default:
						return 0;
					case 0:
						break;
					case 2: // VGA output enable
						if (state == 0)
							gpio_value &= ~4;
						else
							gpio_value |= 4;
						break;
					case 6:
						if (state == 0)
							gpio_value &= ~0x40;
						else
							gpio_value |= 0x40;
						break;
					case 8:
						if (state == 0)
							gpio_value &= ~2;
						else
							gpio_value |= 2;
						break;
					case 9: 
						if (state == 0)
							gpio_value &= ~0x80;
						else
							gpio_value |= 0x80;
						break;
					case 10:
						if (state == 0)
							gpio_value &= ~1;
						else
							gpio_value |= 1;
						break;
					case 11:
						if (state == 0)
							gpio_value &= ~8;
						else
							gpio_value |= 8;
						break;
					case 12:
						if (state == 0)
							gpio_value &= ~0x10;
						else
							gpio_value |= 0x10;
						break;
					case 13: 
						if (state == 0)
							gpio_value &= ~0x20;
						else
							gpio_value |= 0x20;
						break;
				}

				if(tl880_vpx_write_fp(tl880dev, 0x154, 0x300 | gpio_value)) {
					return 1;
				} else {
					return 0;
				}
			}
		} else {
			/* Something to do with audio */
			if(state == 0) {
				unsigned long value;

				tl880_write_register(tl880dev, 0x3004, 0);
				tl880_write_register(tl880dev, 0x306C, 0xC30000C3);

				value = 0;
				set_bits(&value, 0x3058, 3, 2, 1);
				set_bits(&value, 0x3058, 0x17, 0x10, 0xC3);
				tl880_write_register(tl880dev, 0x3058, value);

				tl880_write_register(tl880dev, 0x3040, 0x1000);
				tl880_write_register(tl880dev, 0x3044, 0xFFC);
				tl880_write_register(tl880dev, 0x3048, 0x1020);

				tl880_clear_sdram(tl880dev, 0x1000, 0x1040, 0xFFFFFFFF);

				value = 0;
				set_bits(&value, 0x3000, 0x11, 0x10, 1);
				set_bits(&value, 0x3000, 0x13, 0x12, 1);
				set_bits(&value, 0x3000, 0x19, 0x18, 1);
				tl880_write_register(tl880dev, 0x3000, value);

				value = 0;
				set_bits(&value, 0x3004, 0, 0, 1);
				set_bits(&value, 0x3004, 0x13, 0x12, 1);
				tl880_write_register(tl880dev, 0x3004, value);
				return 0;
			} else {
				tl880_write_register(tl880dev, 0x306C, 0);
				tl880_write_regbits(tl880dev, 0x3058, 3, 2, 2);
				tl880_write_regbits(tl880dev, 0x3058, 0x17, 0x10, 0);
				tl880_write_regbits(tl880dev, 0x3004, 0x13, 0x12, 0);
				return 0;
			}
		}
	} else {
		printk(KERN_WARNING "tl880: Attempt to set GPIO on card for which GPIO routines not yet written\n");
		printk(KERN_WARNING "tl880: card_type=%i\n", tl880dev->card_type);
	}

	return 1;
}

