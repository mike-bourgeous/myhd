/* 
 * Demux control for TL880-based cards
 *
 * (c) 2003 Mike Bourgeous <nitrogen@slimetech.com>
 */
#include "tl880.h"


struct demux_dma_control {
	unsigned long field_0;
	unsigned long field_4;
	unsigned long field_8;
	unsigned long reg;
};

struct demux_dev {
	unsigned long field_0;
	unsigned long field_4;
	unsigned long field_8;
	unsigned long field_c;
	unsigned long field_10;
	unsigned long field_14;
	unsigned long field_18;
	unsigned long field_1c;
	unsigned long field_20;
	unsigned long field_24;
	unsigned long field_28;
	unsigned long field_2c;
	unsigned long field_30;
	unsigned long field_34;
	unsigned long field_38;
	unsigned long field_3c;
	unsigned long field_40;
	unsigned long field_44;
	unsigned long field_48;
	unsigned long field_4c;
	unsigned long field_50;
	unsigned long field_54;
	unsigned long field_58;
	unsigned long field_5c;
	unsigned long field_60;
	unsigned long field_64;
	unsigned long reg;
	unsigned long field_6c;
	unsigned long field_70;
	unsigned long field_74;
};

unsigned long dev_bitmask = 0;
unsigned char demux_inited = 0;
unsigned int demux_pcr_pid = 0;
struct demux_dev *demux_pcr_dev = NULL;
unsigned int crc_table[256];

unsigned long tsd_last_queued_interrupt = 0;

struct demux_dma_control dma_chan[44];
struct demux_dev demux_dev_list[32];

unsigned long tl880_demux_init(struct tl880_dev *tl880dev)
{
	unsigned long crcval;	/* crc: eax */
	unsigned long i;	/* crc: esi */
	int bit;		/* crc: ecx */
	unsigned long reg;
	/* unsigned long retval; */

	//Demux_stop();

	dma_bitmask = 0;
	dev_bitmask = 0;

	/* DemuxCommandTableWriteSema = OS_CreateMutexSemaphore("CMDTableWrite"); */
	
	/* DemuxCommandTableExecSema = OS_CreateCountingSemaphore("CMDTableExec"); */

	/* DevAllocateSema = OS_CreateMutexSemaphore("devAllocate"); */

	/*
	if(!DemuxCommandTableWriteSema || 
	   !DemuxCommandTableExecSema ||
	   !DevAllocateSema) {
		return 0xe77;
	}
	*/

	demux_inited = 1;
	demux_pcr_pid = 0xff;
	// demux_pcr_dev = NULL;

	/* Upload TSD Firmware */
	/* Upload TSD Data */
	/*
	ebp = 0x20000;			// firmware register start
	esi = &tsd_instructions;	// firmware location in memory
	if(tsd_instructions_len) {
		// tl880_tsd_firmupload();
	}
	*/
	

	/* Initialize dma_chan */
	for(reg = 0x247e8, i = 0; reg < 0x24d68; reg += 0x20, i++) {
		dma_chan[i].field_0 = 0;
		dma_chan[i].field_4 = 0;
		dma_chan[i].field_8 = 0;
		dma_chan[i].reg = reg;
	}


	/* Initialize demux_dev_list */
	for(reg = 0x24568, i = 0; i < 32; i++, reg += 0x14) {
		demux_dev_list[i].field_0 = 0;
		demux_dev_list[i].field_4 = -1;
		demux_dev_list[i].field_8 = 0;
		demux_dev_list[i].field_c = 0;
		demux_dev_list[i].field_10 = -1;
		demux_dev_list[i].field_14 = 0;
		
		demux_dev_list[i].field_64 = 0;
		demux_dev_list[i].reg = reg;
		demux_dev_list[i].field_70 = 0;
		demux_dev_list[i].field_6c = 0;
		demux_dev_list[i].field_50 = 0;
		demux_dev_list[i].field_60 = 0;

		write_register(tl880dev, reg, 0x3fff);
		write_register(tl880dev, reg + 4, 0xa0);
		write_register(tl880dev, reg + 8, 0);
		write_register(tl880dev, reg + 0xc, 0);
		write_register(tl880dev, reg + 0x10, 0);
	}

	
	/* Calculate CRC table */
	for(i = 0; i < 256; i++) {
		for(bit = 7, crcval = 0; bit >= 0; bit--) {
			if(((i >> bit) & 1) ^ (crcval >> 0x1f)) {
				crcval += crcval;
				crcval ^= 0x4c11db7;
			} else {
				crcval <<= 1;
			}
		}
		
		crc_table[i] = crcval;
	}

	/*
	if((retval = InitPSI())) {
		return retval;
	}
	*/

	/* Zero some registers */
	for(i = 0x25718; i <= 0x25764; i += 4) {
		write_register(tl880dev, i, 0);
	}
	for(i = 0x257f0; i <= 0x257fc; i += 4) {
		write_register(tl880dev, i, 0);
	}

	write_register(tl880dev, 0x27810, 0x5000);

	tsd_last_queued_interrupt = read_register(tl880dev, 0x27814) & 0xf;
	/* SetTL850Interrupt(0x144, tsdIsq) */

	/* SetTL850Interrupt(0x14a, tsdCommandExecIsr) */

	do {
		write_register(tl880dev, 0x26804, 0);
		write_register(tl880dev, 0x26800, 6);
		
		/* Wait on these registers (yes the while loop is empty) */
		while(read_register(tl880dev, 0x26804) == 0 && read_register(tl880dev, 0x26800) != 5);
	} while(read_register(tl880dev, 0x26804) != 1);

	write_register(tl880dev, 0x26800, 0);

	return 0;
}


