/* 
 * DMA related functions for TL880-based cards
 *
 * (c) 2003 Mike Bourgeous <nitrogen@slimetech.com>
 */
#include "tl880.h"

unsigned long dma_bitmask = 0;

/* cSemaphore *DevAllocateSema; */

unsigned int tl880_aux_dma_allocate()
{
	unsigned long dma;	/* ? */
	unsigned long bit;	/* bit for &/| */
	unsigned long c;	/* counter */
	
	/*
	if(DevAllocateSema) {
		DevAllocateSema->Wait(0, 0);
	}
	*/

	dma = 0x20;
	bit = 1;
	c = 0;

	for(dma = 0x20, bit = 1, c = 0; c < 0xc; bit <<= 1, dma++, c++) {
		if(!(dma_bitmask & bit)) {
			dma_bitmask |= bit;
			/*
			if(DevAllocateSema) {
				DevAllocateSema->Wait(8, 1);
			}
			*/
			return dma;
		}
	}

	/*
	if(DevAllocateSema) {
		DevAllocateSema->Wait(8, 1);
	}
	*/
	return 0;

}

void tl880_aux_dma_free(unsigned int dma)
{
	/*
	if(DevAllocateSema) {
		DevAllocateSema->Wait(0, 0);
	}
	*/

	if(dma < 0x20 || dma >= 0x2c) {
		printk(KERN_ERR "tl880: AuxDMAFree: bad DMA in %s line %u\n", __FILE__, __LINE__);
	}

	dma = 1 << (dma - 0x20);
	
	if(!(dma_bitmask & dma)) {
		printk(KERN_ERR "tl880: AuxDmaFree: Already freed in %s line %u\n", __FILE__, __LINE__);
	}

	dma_bitmask &= ~dma;

	/*
	if(DevAllocateSema) {
		DevAllocateSema->Signal(8, 1);
	}
	*/
}

struct tl880_dma_info {
	unsigned long field_0;		/* ex: 1f54a280 */
	unsigned long field_4;		/* ex: 0000000c */
	unsigned long field_8;		/* ex: 00005000 */
	unsigned long field_c;		/* ex: 00002800 */
	unsigned long field_10;		/* ex: 0000ff14 */
	unsigned long field_14;
};

void tl880_aux_load_dma_mbox(struct tl880_dev *tl880dev, struct tl880_dma_info *dmainfo)
{
	/*
	 * Example register writes for these registers:
	 * 25758 0000FF14
	 * 25718 00001F54
	 * 2571C 0000A280
	 * 25720 0000A000
	 * 25724 0000A000
	 * 25728 0000000C
	 */


	if(!tl880dev || !dmainfo) {
		printk(KERN_ERR "tl880: NULL pointer to tl880_aux_load_dma_mbox\n");
	}

	if(dmainfo->field_10 != 0xff14) {
		/* Write upper word */
		write_register(tl880dev, 0x25750, dmainfo->field_14 >> 0x10);

		/* Write lower word */
		write_register(tl880dev, 0x25754, dmainfo->field_14 & 0xffff);
	}

	write_register(tl880dev, 0x25758, dmainfo->field_10);
	write_register(tl880dev, 0x25718, dmainfo->field_0 >> 0x10);
	write_register(tl880dev, 0x2571c, dmainfo->field_0 & 0xffff);
	write_register(tl880dev, 0x25720, dmainfo->field_c << 2);
	write_register(tl880dev, 0x25724, dmainfo->field_8 << 1);
	write_register(tl880dev, 0x25728, dmainfo->field_4);
}


