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


