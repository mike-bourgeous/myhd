/* 
 * Memory writing support for TL880-based cards
 *
 * (c) 2006-2007 Mike Bourgeous <nitrogen at users.sourceforge.net>
 * (c) 2007 Jason P. Matthews
 *
 * $Log: tl880mem.c,v $
 * Revision 1.4  2007/04/24 06:32:13  nitrogen
 * Changed most int/long types to explicit 32-bit sizes.  Fixed compilation and execution on 64-bit CPUs.
 *
 * Revision 1.3  2007/03/26 19:23:56  nitrogen
 * Added GPIO patch by Jason P. Matthews.
 *
 */
#include "tl880.h"

void tl880_write_memory(struct tl880_dev *tl880dev, u32 mem, u32 value)
{
	if(!tl880dev || !tl880dev->memspace) {
		return;
	}
	writel(value, (void *)(tl880dev->memspace + mem));
}

u32 tl880_read_memory(struct tl880_dev *tl880dev, u32 mem)
{
	u32 value;
	if(!tl880dev || !tl880dev->memspace) {
		return 0;
	}
	value = readl((void *)(tl880dev->memspace + mem));
	return value;
}

u32 tl880_read_membits(struct tl880_dev *tl880dev, u32 mem, int high_bit, int low_bit)
{
	int shift = high_bit - low_bit + 1;
	int mask = ~(0xffffffff << shift);
	
	return (tl880_read_memory(tl880dev, mem) >> low_bit) & mask;
}

void tl880_write_membits(struct tl880_dev *tl880dev, u32 mem, int high_bit, int low_bit, u32 value)
{
	u32 curval;

	// printk(KERN_DEBUG "tl880: tl880_write_membits 0x%08lx 0x%lx 0x%lx %08lx\n", mem, high_bit, low_bit, value);

	curval = tl880_read_memory(tl880dev, mem);

	set_bits(&curval, mem, high_bit, low_bit, value);

	tl880_write_memory(tl880dev, mem, curval);
}

void tl880_clear_sdram(struct tl880_dev *tl880dev, u32 start_addr, u32 end_addr, u32 value)
{
   while (start_addr < end_addr)
   {
      tl880_write_memory(tl880dev,start_addr,value);
      start_addr += 4;
   }
   
   return;
}

EXPORT_SYMBOL(tl880_write_memory);
EXPORT_SYMBOL(tl880_read_memory);
EXPORT_SYMBOL(tl880_write_membits);
EXPORT_SYMBOL(tl880_read_membits);
EXPORT_SYMBOL(tl880_clear_sdram);
