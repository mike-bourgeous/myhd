/* 
 * Memory writing support for TL880-based cards (possibly also TL850)
 *
 * (c) 2006 Mike Bourgeous <nitrogen@slimetech.com>
 */
#include "tl880.h"

void tl880_write_memory(struct tl880_dev *tl880dev, unsigned long mem, unsigned long value)
{
	if(!tl880dev || !tl880dev->memspace) {
		return;
	}
	writel(value, (void *)(tl880dev->memspace + mem));
}

unsigned long tl880_read_memory(struct tl880_dev *tl880dev, unsigned long mem)
{
	unsigned long value;
	if(!tl880dev || !tl880dev->memspace) {
		return 0;
	}
	value = readl((void *)(tl880dev->memspace + mem));
	return value;
}

unsigned long tl880_read_membits(struct tl880_dev *tl880dev, unsigned long mem, long high_bit, long low_bit)
{
	int shift = high_bit - low_bit + 1;
	int mask = ~(0xffffffff << shift);
	
	return (tl880_read_memory(tl880dev, mem) >> low_bit) & mask;
}

void tl880_write_membits(struct tl880_dev *tl880dev, unsigned long mem, long high_bit, long low_bit, unsigned long value)
{
	unsigned long curval;

	// printk(KERN_DEBUG "tl880: tl880_write_membits 0x%08lx 0x%lx 0x%lx %08lx\n", mem, high_bit, low_bit, value);

	curval = tl880_read_memory(tl880dev, mem);

	set_bits(&curval, mem, high_bit, low_bit, value);

	tl880_write_memory(tl880dev, mem, curval);
}

EXPORT_SYMBOL(tl880_write_memory);
EXPORT_SYMBOL(tl880_read_memory);
EXPORT_SYMBOL(tl880_write_membits);
EXPORT_SYMBOL(tl880_read_membits);

