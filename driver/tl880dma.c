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
	/* field_0: bus address plus current writing offset */
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

#if 0
void VopLoadDescriptor(int arg_0, int arg_4)
{
	struct tl880_dma_info vop_dma_info;
	unsigned long eax = 0, ebx = 0, ecx = 0, edx = 0;
	unsigned long esi = 0, edi = 0;
	unsigned long CDma[0x100];
	unsigned long var_4;
	
	eax = arg_4;
	esi = (unsigned long)CDma;
	eax <<= 4;
	edi = 0;
	eax = eax+esi+4;
	
	if(CDma[0x34] == 0) {
		goto loc_25a3c;
	}

	if(CDma[0x3c] == 0) {
		goto loc_25a3c;
	}

	eax = *(unsigned long *)(eax+8);

	goto loc_25a3f;

loc_25a3c:
	eax = *(unsigned long *)(eax+4);

loc_25a3f:
	var_4 = eax;
	eax = g_tsDMAenabled;
	if(eax == 0) {
		goto loc_25b29;
	}

	if(arg_4 != 0) {
		goto loc_25a66;
	}

	if(CDma[0x38] != 0 || CDma[0x3c] != 0) {
		goto loc_25b29;
	}

loc_25a66:
	if(_tsDmaBufCnt != 0) {
		goto loc_25a73;
	}

	eax = GetRawStreamCounter();
	
loc_25a73:
	eax = RawStreamBufFullness();

	esi = 0x5e00;
	
	edx = 0;
	ecx = esi;
	eax /= ecx;

	_tsDmaFullness = eax;
	if(eax == 0) {
		goto loc_25c08;
	}

	if(_tsDmaBufCnt != 0) {
		goto loc_25aa1;
	}

	if(eax >= 7) {
		goto loc_25c08;
	}

loc_25aa1:
	eax *= 0x5e00;
	edi = eax;

	eax = GetRawStreamRdPtr(1);

	ebx = 0x2f000;
	ecx = ebx;
	ecx -= eax;
	if(ecx >= 0) {
		goto loc_25ac8;
	}

	eax = GetRawStreamRdPtr(1);

	edi = ebx;
	edi -= eax;

loc_25ac8:
	_tsDmaDesc.field_10 = 0xff04;
	
	eax = GetRawStreamRdPtr(0);

	ecx = g_tsBufWrPtr;
	_tsDmaDesc.field_14 = eax;
	eax = g_rawStreamPhyAddr;
	ecx += eax;
	_tsDmaDesc.field_0 = ecx;
	edx = 0;
	eax = edi;
	ecx = esi;
	eax /= ecx;

	_tsdDmaMode = 0;
	_tsDmaDesc.field_8 = esi;
	_tsDmaDesc.field_c = 0x2f00;
	_tsdDmaActive = 1;
	_tsDmaDesc.field_4 = eax;

	tl880_aux_load_dma_mbox(&_tsDmaDesc);
	return;

loc_25b29:
	ebx = 1;
	_tsdDmaMode = 1;
	if(eax != 0) {
		goto loc_25bd2;
	}

	eax = CDma->0x38;
	if(CDma[0x38] == 0) {
		goto loc_25ba2;
	}

	if(CDma[0x3c] == 0) {
		goto loc_25b80;
	}

	if(CDma[0x50] != 2) {
		goto loc_25b62;
	}

	if(arg_4 == 0) {
		goto loc_25b62;
	}

	eax = CDma->0x60;
	CDma[0x28] = eax;
	eax = CDma->0x64;
	CDma[0x2c] = eax;
	eax = CDma->0x68;
	goto loc_25b71;

loc_25b62:
	eax = CDma->0x6c;
	CDma[0x28] = eax;
	eax = CDma->0x70;
	CDma[0x2c] = eax;
	eax = CDma->0x74;

loc_25b71:
	CDma[0x5c] = eax;
	eax = CDma->0x2c;
	eax += 3;

	goto loc_25bcb; // SetupAuxVideo(CDma->0x28, eax);

loc_25b80:
	if(eax == 0) {
		goto loc_25ba2;
	}

	ecx = CDma->0x60;
	if(CDma[0x28] != ecx) {
		goto loc_25b94;
	}

	eax = CDma->0x2c;
	if(eax == CDma->0x64) {
		goto loc_25bd2;
	}

loc_25b94:
	eax = CDma->0x64;
	edx = CDma->0x68;
	CDma[0x28] = ecx;
	CDma[0x2c] = eax;
	goto loc_25bc3;

loc_25ba2:
	if(CDma[0x3c] == 0) {
		goto loc_25bd2;
	}

	ecx = CDma->0x6c;
	if(CDma[0x28] != ecx) {
		goto loc_25bb7;
	}

	eax = CDma->0x2c;
	if(eax == CDma->0x70) {
		goto loc_25bd2;
	}

loc_25bb7:
	eax = CDma->0x70;
	edx = CDma->0x74;
	CDma[0x28] = ecx;
	CDma[0x2c] = eax;

loc_25bc3:
	eax += 3;
	CDma[0x5c] = edx;

	next call (ecx, eax);

loc_25bcb:
	SetupAuxVideo(blah1, blah2);

loc_25bd2:
	eax = var_4;
	ecx = CDma->0x5c;
	vop_dma_info.field_0 = eax;
	eax = CDma->0x2c;
	/* edx = (eax < 0) ? -1 : 0; // cdq */
	eax /= ecx;
	esi = CDma->0x28;
	esi *= ecx;
	vop_dma_info.field_8 = esi;
	esi >>= 1;
	vop_dma_info.field_10 = 0xff14;
	vop_dma_info.field_c = esi;
	vop_dma_info.field_4 = eax;
	_tsdDmaActive = ebx;

loc_25bff:
	/* set up parameters */

loc_25c03:
	tl880_aux_load_dma_mbox(&vop_dma_info);
}

#endif

