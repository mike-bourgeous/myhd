/* 
 * Register support for TL880-based cards (possibly also TL850)
 *
 * (c) 2003 Mike Bourgeous <nitrogen@slimetech.com>
 */
#include "tl880.h"

void write_register(struct tl880_dev *tl880dev, unsigned long reg, unsigned long value)
{
	if(!tl880dev || !tl880dev->regspace) {
		return;
	}
	writel(value, (void *)(tl880dev->regspace + reg));
}

unsigned long read_register(struct tl880_dev *tl880dev, unsigned long reg)
{
	unsigned long value;
	if(!tl880dev || !tl880dev->regspace) {
		return 0;
	}
	value = readl((void *)(tl880dev->regspace + reg));
	return value;
}

unsigned long read_regbits(struct tl880_dev *tl880dev, unsigned long reg, long high_bit, long low_bit)
{
	int shift = high_bit - low_bit + 1;
	int mask = ~(0xffffffff << shift);
	
	return (read_register(tl880dev, reg) >> low_bit) & mask;
}


void write_regbits(struct tl880_dev *tl880dev, unsigned long reg, long high_bit, long low_bit, unsigned long value)
{
	unsigned long curval;

	// printk(KERN_DEBUG "tl880: write_regbits 0x%08lx 0x%lx 0x%lx %08lx\n", reg, high_bit, low_bit, value);

	curval = read_register(tl880dev, reg);

	set_bits(&curval, reg, high_bit, low_bit, value);

	write_register(tl880dev, reg, curval);
}


