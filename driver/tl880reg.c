/* 
 * Register support for TL880-based cards
 *
 * (c) 2003-2007 Mike Bourgeous <nitrogen at users.sourceforge.net>
 *
 * $Log: tl880reg.c,v $
 * Revision 1.4  2007/03/26 19:25:06  nitrogen
 * Added CVS log generation and updated copyrights and e-mail addresses.
 *
 */
#include "tl880.h"

void tl880_write_register(struct tl880_dev *tl880dev, unsigned long reg, unsigned long value)
{
	if(CHECK_NULL(tl880dev) || CHECK_NULL(tl880dev->regspace)) {
		return;
	}
	if(reg >= 0x100000) {
		return;
	}
	writel(value, (void *)(tl880dev->regspace + reg));
}

unsigned long tl880_read_register(struct tl880_dev *tl880dev, unsigned long reg)
{
	unsigned long value;
	if(CHECK_NULL(tl880dev) || CHECK_NULL(tl880dev->regspace)) {
		return 0;
	}
	if(reg >= 0x100000) {
		return -1;
	}
	value = readl((void *)(tl880dev->regspace + reg));
	return value;
}

unsigned long tl880_read_regbits(struct tl880_dev *tl880dev, unsigned long reg, long high_bit, long low_bit)
{
	int shift = high_bit - low_bit + 1;
	int mask = ~(0xffffffff << shift);
	
	return (tl880_read_register(tl880dev, reg) >> low_bit) & mask;
}

void tl880_write_regbits(struct tl880_dev *tl880dev, unsigned long reg, long high_bit, long low_bit, unsigned long value)
{
	unsigned long curval;

	// printk(KERN_DEBUG "tl880: tl880_write_regbits 0x%08lx 0x%lx 0x%lx %08lx\n", reg, high_bit, low_bit, value);

	curval = tl880_read_register(tl880dev, reg);

	set_bits(&curval, reg, high_bit, low_bit, value);

	tl880_write_register(tl880dev, reg, curval);
}

EXPORT_SYMBOL(tl880_write_register);
EXPORT_SYMBOL(tl880_read_register);
EXPORT_SYMBOL(tl880_write_regbits);
EXPORT_SYMBOL(tl880_read_regbits);

