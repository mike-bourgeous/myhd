/* 
 * Memory writing support for TL880-based cards
 *
 * (c) 2006-2007 Mike Bourgeous <nitrogen at users.sourceforge.net>
 * (c) 2007 Jason P. Matthews
 *
 * $Log: tl880mem.c,v $
 * Revision 1.5  2007/09/06 05:22:05  nitrogen
 * Improvements to audio support, documentation, and card memory management.
 *
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

/*
 * This is basically like memset() but it operates on card memory.
 */
void tl880_clear_sdram(struct tl880_dev *tl880dev, u32 start_addr, u32 end_addr, u32 value) /* {{{ */
{
     	while (start_addr < end_addr) {
	  	tl880_write_memory(tl880dev,start_addr,value);
	  	start_addr += 4;
     	}
     	
     	return;
} /* }}} */

/*
 * Initializes the memory pool used to allocate card memory.
 *
 * Returns nonzero if there was a failure.
 */
int tl880_init_memory(struct tl880_dev *tl880dev) /* {{{ */
{
	if(CHECK_NULL(tl880dev) || TL_ASSERT(tl880dev->pool == NULL)) {
		return -1;
	}

	tl880dev->pool = gen_pool_create(2, -1);
	if(TL_ASSERT(tl880dev->pool != NULL)) {
		return -1;
	}

	/* Should I specify a specific NUMA node here ever? */
	/* The pool starts at 0x11000 because earlier points in RAM are used for something already */
	if(TL_ASSERT(gen_pool_add(tl880dev->pool, 0x11000, tl880dev->memlen - 0x11000, -1) == 0)) {
		gen_pool_destroy(tl880dev->pool);
		return -1;
	}

	return 0;
} /* }}} */

/*
 * Deinitializes the memory pool for card memory.
 */
void tl880_deinit_memory(struct tl880_dev *tl880dev) /* {{{ */
{
	if(CHECK_NULL(tl880dev)) {
		return;
	}

	if(!CHECK_NULL_W(tl880dev->pool)) {
		gen_pool_destroy(tl880dev->pool);
		tl880dev->pool = NULL;
	}
} /* }}} */

/*
 * Allocates a block of card memory.
 *
 * Returns 0 if there was an error, otherwise the address (relative to the
 * start of card memory) on success.
 */
unsigned long tl880_alloc_memory(struct tl880_dev *tl880dev, size_t bytes) /* {{{ */
{
	if(CHECK_NULL(tl880dev) || CHECK_NULL(tl880dev->pool)) {
		return 0;
	}

	return gen_pool_alloc(tl880dev->pool, bytes);
} /* }}} */

/*
 * Frees a block of card memory.
 */
void tl880_free_memory(struct tl880_dev *tl880dev, unsigned long addr, size_t bytes) /* {{{ */
{
	if(CHECK_NULL(tl880dev) || CHECK_NULL(tl880dev->pool)) {
		return;
	}

	gen_pool_free(tl880dev->pool, addr, bytes);
} /* }}} */

EXPORT_SYMBOL(tl880_write_memory);
EXPORT_SYMBOL(tl880_read_memory);
EXPORT_SYMBOL(tl880_write_membits);
EXPORT_SYMBOL(tl880_read_membits);
EXPORT_SYMBOL(tl880_clear_sdram);

EXPORT_SYMBOL(tl880_alloc_memory);
EXPORT_SYMBOL(tl880_free_memory);

