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
	/* phys_addr: bus address plus current writing offset */
	unsigned long phys_addr;		/* ex: 1f54a280 */
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
	write_register(tl880dev, 0x25718, dmainfo->phys_addr >> 0x10);
	write_register(tl880dev, 0x2571c, dmainfo->phys_addr & 0xffff);
	write_register(tl880dev, 0x25720, dmainfo->field_c << 2);
	write_register(tl880dev, 0x25724, dmainfo->field_8 << 1);
	write_register(tl880dev, 0x25728, dmainfo->field_4);
}

void tl880_load_dma_descriptor(struct tl880_dev *tl880dev, int arg_0, int arg_4)
{
	struct tl880_dma_info vop_dma_info;
	struct tl880_dma_info tsd_dma_info;
	unsigned long eax = 0, ecx = 0;
	unsigned long edi = 0;
	unsigned long CDma[0x100];
	unsigned long aux_dmaphys;
	unsigned long tsdDmaMode = 0, tsdDmaActive = 0;
	unsigned long g_tsDMAenabled = 0;
	unsigned long tsDmaBufCnt = 0, tsDmaFullness = 0;
	unsigned long g_tsBufWrPtr = 0;		/* Offset within DMA buffer */

	/* eax = (unsigned long)&CDma[(arg_4 << 4) + 4]; */
	
	if(CDma[0x34] == 0 || CDma[0x3c] == 0) {
		/* aux_dmaphys = *(unsigned long *)(eax+4); */
		aux_dmaphys = CDma[arg_4 * 16 + 8];
	} else {
		/* aux_dmaphys = *(unsigned long *)(eax+8); */
		aux_dmaphys = CDma[arg_4 * 16 + 0xc];
	}

	if(g_tsDMAenabled && (arg_4 || !(CDma[0x38] || CDma[0x3c]))) {
		if(tsDmaBufCnt == 0) {
			/* GetRawStreamCounter(); */
		}
		
		/* 0x5e00 is 128 ts packets of 188 bytes each */
		eax = 0 /* RawStreamBufFullness() / 0x5e00 */;
		
		tsDmaFullness = 0 /* RawStreamBufFullness() / 0x5e00 */;

		if(tsDmaFullness == 0 || (tsDmaBufCnt == 0 && tsDmaFullness >= 7)) {
			return;
		}
		
		edi = tsDmaFullness * 0x5e00;
		
		ecx = 0x2f000 - 0 /* GetRawStreamRdPtr(1) */;
		
		if(ecx < edi) {
			edi = 0x2f000 - 0 /* GetRawStreamRdPtr(1) */;
		}
		
		tsd_dma_info.field_10 = 0xff04;
		
		tsd_dma_info.field_14 = 0 /* GetRawStreamRdPtr(0) */;
	
#if 0
		ecx = g_tsBufWrPtr;
		eax = tl880dev->dmaphys;	/* g_rawStreamPhyAddr */
		ecx += eax;
		tsd_dma_info.phys_addr = ecx;
#endif
		tsd_dma_info.phys_addr = tl880dev->dmaphys + g_tsBufWrPtr;
		
		tsdDmaMode = 0;
		tsd_dma_info.field_8 = 0x5e00;
		tsd_dma_info.field_c = 0x2f00;
		tsdDmaActive = 1;
		tsd_dma_info.field_4 = edi / 0x5e00;
		
		tl880_aux_load_dma_mbox(tl880dev, &tsd_dma_info);
	} else { 
		tsdDmaMode = 1;
		if(!aux_dmaphys) {
			if(CDma[0x38] != 0) { 
				if(CDma[0x3c]) {
					if(CDma[0x50] != 2 || arg_4 == 0) {
						CDma[0x28] = CDma[0x6c];
						CDma[0x2c] = CDma[0x70];
						CDma[0x5c] = CDma[0x74];
					} else {
						CDma[0x28] = CDma[0x60];
						CDma[0x2c] = CDma[0x64];
						CDma[0x5c] = CDma[0x68];
					}
					
					/* SetupAuxVideo(CDma[0x28], CDma[0x2c] + 3); */
				} else if(CDma[0x38] != 0) {
					ecx = CDma[0x60];
					if(CDma[0x28] != CDma[0x60] || CDma[0x2c] != CDma[0x64]) {
						CDma[0x28] = CDma[0x60];
						CDma[0x2c] = CDma[0x64];
						CDma[0x5c] = CDma[0x68];
						
						/* SetupAuxVideo(CDma[0x60], CDma[0x64] + 3); */
					}
				}
			} else if(CDma[0x3c] != 0) {
				if(CDma[0x28] != CDma[0x6c] || CDma[0x2c] != CDma[0x70]) {
					CDma[0x28] = CDma[0x6c];
					CDma[0x2c] = CDma[0x70];
					CDma[0x5c] = CDma[0x74];
					
					/* SetupAuxVideo(CDma[0x6c], CDma[0x70] + 3); */
				}
			}
		}
		
		vop_dma_info.phys_addr = aux_dmaphys;
		vop_dma_info.field_4 = CDma[0x2c] / CDma[0x5c];
		vop_dma_info.field_8 = CDma[0x28] * CDma[0x5c];
		vop_dma_info.field_c = (CDma[0x28] * CDma[0x5c]) / 2;
		vop_dma_info.field_10 = 0xff14;
		tsdDmaActive = 1;
		
		tl880_aux_load_dma_mbox(tl880dev, &vop_dma_info);
	}
}


