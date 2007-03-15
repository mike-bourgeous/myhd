/* 
 * TL880 Audio Processing Unit
 *
 * (c) 2003 Mike Bourgeous <nitrogen@slimetech.com>
 */
#include "tl880.h"


void tl880_disable_apu(struct tl880_dev *tl880dev)
{
	tl880_write_register(tl880dev, 0x3000, 0);
	tl880_write_register(tl880dev, 0x3004, 0);
}

void tl880_init_hardware_audio(struct tl880_dev *tl880dev, enum audio_mode_e audio_mode)
{
	tl880dev->audio_mode = audio_mode;

	tl880dev->iau_base = 0x186000;
	/*
	if(tl880dev->iau_base == 0) {
		if((tl880dev->iau_base = yGetTL850Memory(7800, 0)) == 0) {
			return;
		}
	}

	if(g_IECBuf == 0) {
		g_IECbuf = ExAllocatePoolWithTag(0, 0x1800, 'W''d''m'' ');
	}
	*/

	tl880_apu_init_ioc(tl880dev);
	tl880_apu_init_iau_reg(tl880dev);
	/*
	long iauP = 0;
	unsigned long iauCnt = 0;
	//SetSamplingClock(1);
	unsigned char startAC3 = 0;
	*/
}

void tl880_deinit_hardware_audio(struct tl880_dev *tl880dev)
{
	tl880_apu_stop_ioc(tl880dev);
	/*
	tl880_write_register(tl880dev, tl880dev->iau_iba_reg, 0);
	tl880_write_register(tl880dev, tl880dev->iau_iea_reg, 0);
	tl880_write_register(tl880dev, tl880dev->iau_ira_reg, 0);
	if(tl880dev->iau_base != 0) {
		FreeTL850Memory(tl880dev->iau_base);
		tl880dev->iau_base = 0;
	}
	*/
	/*
	if(g_IECbuf != 0) {
		ExFreePool(g_IECbuf);
		g_IECbuf = 0;
	}
	*/
}

/* Set card memory capture address (?) */
void tl880_apu_init_iau_reg(struct tl880_dev *tl880dev)
{
	if(tl880dev->audio_mode == 0) {
		tl880dev->iau_iba_reg = 0x3034;
		tl880dev->iau_iea_reg = 0x303c;
		tl880dev->iau_ira_reg = 0x3038;
	} else {
		tl880dev->iau_iba_reg = 0x304c;
		tl880dev->iau_iea_reg = 0x3054;
		tl880dev->iau_ira_reg = 0x3050;
	}
	tl880_write_register(tl880dev, tl880dev->iau_iba_reg, 0x1000); /* 1000 used as test base (tl880dev->iau_base) */
							/* tl880dev->iau_base set to memory allocated with yGetTL850Memory in InitHardwareAudio */
	tl880_write_register(tl880dev, tl880dev->iau_iea_reg, 0x87fc); /* base + 77fc */
	tl880_write_register(tl880dev, tl880dev->iau_ira_reg, 0x1000); /* base */
	// SdramClear(tl880dev->iau_base, 0x7800);
}

void tl880_apu_init_ioc(struct tl880_dev *tl880dev)
{
	tl880_write_register(tl880dev, 0x3058, 0);
	tl880_write_register(tl880dev, 0x3008, 0);
	tl880_write_register(tl880dev, 0x3004, 0);
	tl880_write_register(tl880dev, 0x3000, 0);
	tl880_write_register(tl880dev, tl880dev->iau_iba_reg, 0);	// 0x3034 or 0x304c
	tl880_write_register(tl880dev, tl880dev->iau_iea_reg, 0);	// 0x303c or 0x3054
	tl880_write_register(tl880dev, tl880dev->iau_ira_reg, 0);	// 0x3038 or 0x3050
}

void tl880_apu_start_ioc(struct tl880_dev *tl880dev)
{
	if(tl880dev->audio_mode == 0) {
		tl880_write_register(tl880dev, 0x3058, 0xc301);
		tl880_write_register(tl880dev, 0x3004, 0x10001);
	} else if(tl880dev->audio_mode == 1) {
		tl880_write_register(tl880dev, 0x3058, 0xc3000010);
		tl880_write_register(tl880dev, 0x3004, 0x100001);
	}
	tl880_write_register(tl880dev, 0x3000, 0x01240000);

	/*
	TL850Audio_SetCrossFade(0, 0, 0xc3, 0, 0, 0, 0, 0);
	TL850Audio_SetCrossFade(0, 1, 0, 0xc3, 0, 0, 0, 0);
	TL850Audio_SetCrossFade(1, 0, 0, 0, 0xc3, 0, 0, 0);
	TL850Audio_SetCrossFade(1, 1, 0, 0, 0, 0xc3, 0, 0);
	TL850Audio_SetCrossFade(2, 0, 0, 0, 0, 0, 0xc3, 0);
	TL850Audio_SetCrossFade(2, 1, 0, 0, 0, 0, 0, 0xc3);
	*/

	if(tl880dev->audio_mode == 1) {
		tl880_write_register(tl880dev, 0x300c, 0x2000002);
		tl880_write_register(tl880dev, 0x3010, 0x2000002);
		tl880_write_register(tl880dev, 0x3008, 0x5479);
	}
}

void tl880_apu_stop_ioc(struct tl880_dev *tl880dev)
{
	tl880_apu_init_ioc(tl880dev);
}


void tl880_ntsc_audio_dpc(struct tl880_dev *tl880dev)
{
	int val1, val2, val3;
	
	val1 = tl880_read_register(tl880dev, 0x3030); // ebx
	val2 = tl880_read_register(tl880dev, 0x3020); // edi
	val3 = tl880_read_register(tl880dev, 0x3024); // eax

	if(val1 != val2) {
		tl880_write_register(tl880dev, 0x3030, val2);
	} else {
		tl880_write_register(tl880dev, 0x3030, (val2 + val3) >> 1);
	}
}

void tl880_set_ntsc_audio_clock(struct tl880_dev *tl880dev)
{
	// SetSamplingClock(3) // TODO: finish SetSamplingClock
	/* { */
	tl880_set_gpio(tl880dev, 9, 0);
	tl880_set_gpio(tl880dev, 8, 0);
	// Set something on the VPX
	/* } */
	
	tl880_write_register(tl880dev, 0x27810, 0xa000);
}

void tl880_init_ntsc_audio(struct tl880_dev *tl880dev)
{
	unsigned long bitsval = 0;
	
	tl880_set_ntsc_audio_clock(tl880dev);

	tl880_write_register(tl880dev, 0x3004, 0);
	tl880_write_register(tl880dev, 0x305c, 0xc30000c3);
	tl880_write_register(tl880dev, 0x3060, 0);
	tl880_write_register(tl880dev, 0x3064, 0);
	tl880_write_register(tl880dev, 0x3068, 0);
	tl880_write_register(tl880dev, 0x306c, 0xc30000c3);
	tl880_write_register(tl880dev, 0x3070, 0);
	tl880_write_register(tl880dev, 0x3074, 0);
	tl880_write_register(tl880dev, 0x3078, 0);
	tl880_write_register(tl880dev, 0x307c, 0xc30000c3);
	tl880_write_register(tl880dev, 0x3014, 0);
	tl880_write_register(tl880dev, 0x3020, 0x1000);
	tl880_write_register(tl880dev, 0x3024, 0x10fe0);

	/*
	 * eax = gpJanus;
	 * if([eax+b9c] == 0 && [eax+f00] == 0) {
	 *   eax = [gpJanus+167dc]; // 0x1c00
	 * } else {
	 *   eax = [gpJanus+167d8]; // 0x2800
	 * }
	 * eax += 0x1000;
	 * var_8 = eax;
	 */
	tl880_write_register(tl880dev, 0x3028, 0x3800 /* eax */);
	tl880_write_register(tl880dev, 0x302c, 0x1000);
	tl880_write_register(tl880dev, 0x3030, 0x9000);

	/*
	 * eax = [gpJanus+16758]
	 * edx = 4
loc_32C70:
	 * ecx = var_8
	 * [eax-4] = ecx
	 * [eax] = ecx
	 * eax += 0x10;
	 * edx--;
	 * if(edx != 0) goto loc_32C70
loc_32C87:
	 * ecx = var_8;
	 * [eax-4] = ecx
	 * [eax] = ecx
	 * eax += 8;
	 * edx--;
	 * if(edx != 0) goto loc_32C87;
	 */
	
	/*
	 * if(short[gpJanus+16878] != 0) goto loc_32D19
	 */
	tl880_write_register(tl880dev, 0x300c, 0x3000000);
	tl880_write_register(tl880dev, 0x3010, 0x3000000);
	
	set_bits(&bitsval, 0x3008, 0, 0, 1);
	set_bits(&bitsval, 0x3008, 4, 3, 1);
	set_bits(&bitsval, 0x3008, 6, 5, 1);
	set_bits(&bitsval, 0x3008, 0xa, 8, 0);
	set_bits(&bitsval, 0x3008, 0xe, 0xc, 1);
	tl880_write_register(tl880dev, 0x3008, bitsval);

	/*
	 * // Set up *this for clear_sdram
	 * esi = gpjanus
	 * ecx = esi
loc_32D19:
	 * clear_sdram(0x1000, 0x11000, 0);
	 */

	bitsval = 0;
	set_bits(&bitsval, 0x3000, 0, 0, 0);
	set_bits(&bitsval, 0x3000, 1, 1, 0);
	set_bits(&bitsval, 0x3000, 2, 2, 0);
	set_bits(&bitsval, 0x3000, 4, 3, 0);
	set_bits(&bitsval, 0x3000, 6, 5, 0);
	set_bits(&bitsval, 0x3000, 8, 7, 0);
	set_bits(&bitsval, 0x3000, 0xa, 9, 0);
	set_bits(&bitsval, 0x3000, 0xf, 0xf, 0);
	/*
	 * eax = 0
	 * if(gMspI2sMaster == 0) eax = 1
	 */
	set_bits(&bitsval, 0x3000, 0x11, 0x10, 1 /* eax */);
	/*
	 * eax = 0
	 * if(gMspI2sMaster == 0) eax = 1
	 */
	set_bits(&bitsval, 0x3000, 0x13, 0x12, 1 /* eax */);
	set_bits(&bitsval, 0x3000, 0x15, 0x14, 0);
	set_bits(&bitsval, 0x3000, 0x17, 0x16, 0);
	set_bits(&bitsval, 0x3000, 0x19, 0x18, 1);
	tl880_write_register(tl880dev, 0x3000, bitsval);

	bitsval = 0;
	set_bits(&bitsval, 0x3004, 0, 0, 1);
	set_bits(&bitsval, 0x3004, 6, 1, 3);
	set_bits(&bitsval, 0x3004, 7, 7, 1);
	set_bits(&bitsval, 0x3004, 8, 8, 1);
	set_bits(&bitsval, 0x3004, 0xa, 9, 1);
	tl880_write_register(tl880dev, 0x3004, bitsval);

	/*
	 * eax = gpJanus
	 * ecx = bitsval
	 * [gpJanus+167e8] = bitsval
	 * long[gpJanus+157ec] = 0xc
	 */
}

