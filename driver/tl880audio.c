/* 
 * TL880 Audio Processing Unit
 *
 * (c) 2003 Mike Bourgeous <nitrogen@slimetech.com>
 */
#include "tl880.h"

void tl880_disable_apu(struct tl880_dev *tl880dev)
{
	write_register(tl880dev, 0x3000, 0);
	write_register(tl880dev, 0x3004, 0);
}

void tl880_ntsc_audio_dpc(struct tl880_dev *tl880dev)
{
	int val1, val2, val3;
	
	val1 = read_register(tl880dev, 0x3030); // ebx
	val2 = read_register(tl880dev, 0x3020); // edi
	val3 = read_register(tl880dev, 0x3024); // eax

	if(val1 != val2) {
		write_register(tl880dev, 0x3030, val2);
	} else {
		write_register(tl880dev, 0x3030, (val2 + val3) >> 1);
	}
}

void tl880_set_ntsc_audio_clock(struct tl880_dev *tl880dev)
{
	// SetSamplingClock(3)
	/* { */
	tl880_set_gpio(tl880dev, 9, 0);
	tl880_set_gpio(tl880dev, 8, 0);
	/* } */
	
	write_register(tl880dev, 0x27810, 0xa000);
}

void tl880_init_ntsc_audio(struct tl880_dev *tl880dev)
{
	unsigned long bitsval = 0;
	
	tl880_set_ntsc_audio_clock(tl880dev);

	write_register(tl880dev, 0x3004, 0);
	write_register(tl880dev, 0x305c, 0xc30000c3);
	write_register(tl880dev, 0x3060, 0);
	write_register(tl880dev, 0x3064, 0);
	write_register(tl880dev, 0x3068, 0);
	write_register(tl880dev, 0x306c, 0xc30000c3);
	write_register(tl880dev, 0x3070, 0);
	write_register(tl880dev, 0x3074, 0);
	write_register(tl880dev, 0x3078, 0);
	write_register(tl880dev, 0x307c, 0xc30000c3);
	write_register(tl880dev, 0x3014, 0);
	write_register(tl880dev, 0x3020, 0x1000);
	write_register(tl880dev, 0x3024, 0x10fe0);

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
	write_register(tl880dev, 0x3028, 0x3800 /* eax */);
	write_register(tl880dev, 0x302c, 0x1000);
	write_register(tl880dev, 0x3030, 0x9000);

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
	write_register(tl880dev, 0x300c, 0x3000000);
	write_register(tl880dev, 0x3010, 0x3000000);
	
	set_bits(&bitsval, 0x3008, 0, 0, 1);
	set_bits(&bitsval, 0x3008, 4, 3, 1);
	set_bits(&bitsval, 0x3008, 6, 5, 1);
	set_bits(&bitsval, 0x3008, 0xa, 8, 0);
	set_bits(&bitsval, 0x3008, 0xe, 0xc, 1);
	write_register(tl880dev, 0x3008, bitsval);

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
	write_register(tl880dev, 0x3000, bitsval);

	bitsval = 0;
	set_bits(&bitsval, 0x3004, 0, 0, 1);
	set_bits(&bitsval, 0x3004, 6, 1, 3);
	set_bits(&bitsval, 0x3004, 7, 7, 1);
	set_bits(&bitsval, 0x3004, 8, 8, 1);
	set_bits(&bitsval, 0x3004, 0xa, 9, 1);
	write_register(tl880dev, 0x3004, bitsval);

	/*
	 * eax = gpJanus
	 * ecx = bitsval
	 * [gpJanus+167e8] = bitsval
	 * long[gpJanus+157ec] = 0xc
	 */
}

