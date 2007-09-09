/* 
 * TL880 Audio Functionality
 *
 * (c) 2003-2007 Mike Bourgeous <nitrogen at users.sourceforge.net>
 *
 * $Log: tl880audio.c,v $
 * Revision 1.11  2007/09/09 06:16:48  nitrogen
 * Started an ALSA driver.  New iocread4reg tool.  Driver enhancements.
 *
 * Revision 1.10  2007/09/08 09:20:33  nitrogen
 * Fixed memory pool allocation.
 *
 * Revision 1.9  2007/09/06 06:23:50  nitrogen
 * Working on set_crossfade
 *
 * Revision 1.8  2007/09/06 05:22:05  nitrogen
 * Improvements to audio support, documentation, and card memory management.
 *
 * Revision 1.7  2007/04/24 06:32:13  nitrogen
 * Changed most int/long types to explicit 32-bit sizes.  Fixed compilation and execution on 64-bit CPUs.
 *
 * Revision 1.6  2007/03/26 19:25:05  nitrogen
 * Added CVS log generation and updated copyrights and e-mail addresses.
 *
 */
#include "tl880.h"


void tl880_disable_apu(struct tl880_dev *tl880dev)
{
	tl880_write_register(tl880dev, 0x3000, 0);
	tl880_write_register(tl880dev, 0x3004, 0);
}

void tl880_init_hardware_audio(struct tl880_dev *tl880dev, enum audio_mode_e audio_mode)
{
	if(CHECK_NULL(tl880dev)) {
		return;
	}

	tl880dev->audio_mode = audio_mode;

	if(tl880dev->iau_base == 0) {
		/*
		 * This used to be yGetTL850Memory(0x7800, 0) -- The second parameter seems to control
		 * where the memory comes from (top or bottom of pool).
		 */
		if(TL_ASSERT((tl880dev->iau_base = tl880_alloc_memory(tl880dev, 0x7800)) != 0)) {
			return;
		}
		/* tl880dev->iau_base = 0x196000; */
	}

	if(tl880dev->iec_buf == NULL) {
		/* g_IECbuf = ExAllocatePoolWithTag(NonPagedPool, 0x1800, 'W''d''m'' '); */
		/* Linux kernel memory is always non-paged */
		tl880dev->iec_buf = kmalloc(0x1800, GFP_KERNEL); // 0x1800 = 6144 - the size of an AC3-in-PCM frame
	}

	tl880_apu_init_ioc(tl880dev);
	tl880_apu_init_iau_reg(tl880dev);

	/*
	long iauP = 0;
	u32 iauCnt = 0;
	//SetSamplingClock(1);
	*/

	tl880_set_sampling_clock(tl880dev, 1);

	/*
	unsigned char startAC3 = 0;
	*/
}

void tl880_deinit_hardware_audio(struct tl880_dev *tl880dev)
{
	tl880_apu_stop_ioc(tl880dev);
	tl880_write_register(tl880dev, tl880dev->iau_iba_reg, 0);
	tl880_write_register(tl880dev, tl880dev->iau_iea_reg, 0);
	tl880_write_register(tl880dev, tl880dev->iau_ira_reg, 0);
	
	if(tl880dev->iau_base != 0) {
		tl880_free_memory(tl880dev, tl880dev->iau_base, 0x7800);
		tl880dev->iau_base = 0;
	}

	if(tl880dev->iec_buf != NULL) {
		/* ExFreePool(g_IECbuf); */
		kfree(tl880dev->iec_buf);
		tl880dev->iec_buf = NULL;
	}
}

/* Set card memory audio buffer location */
void tl880_apu_init_iau_reg(struct tl880_dev *tl880dev)
{
	if(tl880dev->audio_mode == 0) {
		tl880dev->iau_iba_reg = 0x3034;
		tl880dev->iau_iea_reg = 0x303c;
		tl880dev->iau_ira_reg = 0x3038;
	} else {
		if(tl880dev->audio_mode != 1) {
			return;
		}
		tl880dev->iau_iba_reg = 0x304c;
		tl880dev->iau_iea_reg = 0x3054;
		tl880dev->iau_ira_reg = 0x3050;
	}

	tl880_write_register(tl880dev, tl880dev->iau_iba_reg, tl880dev->iau_base);
	tl880_write_register(tl880dev, tl880dev->iau_iea_reg, tl880dev->iau_base + 0x77fc);
	tl880_write_register(tl880dev, tl880dev->iau_ira_reg, tl880dev->iau_base);
	tl880_clear_sdram(tl880dev, tl880dev->iau_base, tl880dev->iau_base + 0x7800, 0);
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
		tl880_write_register(tl880dev, 0x3004, 0x100001); // This is the dolby digital value
	}
	tl880_write_register(tl880dev, 0x3000, 0x01240000);

	tl880_audio_set_crossfade(tl880dev, 0, 0, 0xc3, 0, 0, 0, 0, 0);
	tl880_audio_set_crossfade(tl880dev, 0, 1, 0, 0xc3, 0, 0, 0, 0);
	tl880_audio_set_crossfade(tl880dev, 1, 0, 0, 0, 0xc3, 0, 0, 0);
	tl880_audio_set_crossfade(tl880dev, 1, 1, 0, 0, 0, 0xc3, 0, 0);
	tl880_audio_set_crossfade(tl880dev, 2, 0, 0, 0, 0, 0, 0xc3, 0);
	tl880_audio_set_crossfade(tl880dev, 2, 1, 0, 0, 0, 0, 0, 0xc3);

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

/* Original function returned enum ErrCode */
int tl880_audio_set_crossfade(struct tl880_dev *tl880dev, s32 a, s32 b, u8 c, u8 d,
							  u8 e, u8 f, u8 g, u8 h)
{
	u32 reg305c;
	u32 reg3060;
	u32 reg3064;
	u32 reg3068;
	u32 reg306c;
	u32 reg3070;
	u32 reg3074;
	u32 reg3078;
	u32 reg307c;

	if(CHECK_NULL(tl880dev)) {
		return -1;
	}

	switch(a) {
		case 0:
			if(b == 0) {
				reg305c = tl880_read_register(tl880dev, 0x305c);
				reg3060 = tl880_read_register(tl880dev, 0x3060);
				reg3064 = tl880_read_register(tl880dev, 0x3064);

				tl880_write_register(tl880dev, 0x305c,
						(reg305c & 0xffff0000) |
						(d << 8) | c);

				tl880_write_register(tl880dev, 0x3060,
						(reg3060 & 0xffff0000) |
						(f << 8) | e);

				tl880_write_register(tl880dev, 0x3064,
						(reg3064 & 0xffff0000) |
						(h << 8) | g);

				break;
			}
			if(b != 1) {
				break;
			}

			reg305c = tl880_read_register(tl880dev, 0x305c);
			reg3060 = tl880_read_register(tl880dev, 0x3060);
			reg3064 = tl880_read_register(tl880dev, 0x3064);

			tl880_write_register(tl880dev, 0x305c, 
					(d << 24) | 
					(c << 16) | 
					(reg305c & 0xffff));

			tl880_write_register(tl880dev, 0x3060, 
					(f << 24) | 
					(e << 16) | 
					(reg3060 & 0xffff));

			tl880_write_register(tl880dev, 0x3064, 
					(h << 24) | 
					(g << 16) |
					(reg3064 & 0xffff));
			break;
			
		case 1:
			if(b == 0) {
				reg3068 = tl880_read_register(tl880dev, 0x3068);
				reg306c = tl880_read_register(tl880dev, 0x306c);
				reg3070 = tl880_read_register(tl880dev, 0x3070);

				tl880_write_register(tl880dev, 0x3068,
						(reg3068 & 0xffff0000) |
						(d << 8) | c);

				tl880_write_register(tl880dev, 0x306c,
						(reg306c & 0xffff0000) |
						(f << 8) | e);

				tl880_write_register(tl880dev, 0x3070, 
						(reg3070 & 0xffff0000) |
						(h << 8) | g);

				break;
			}

			if(b != 1) {
				break;
			}

			reg3068 = tl880_read_register(tl880dev, 0x3068);
			reg306c = tl880_read_register(tl880dev, 0x306c);
			reg3070 = tl880_read_register(tl880dev, 0x3070);

			tl880_write_register(tl880dev, 0x3068, 
					(d << 24) |
					(c << 16) |
					(reg3068 & 0xffff));

			tl880_write_register(tl880dev, 0x306c, 
					(f << 24) |
					(e << 16) |
					(reg306c & 0xffff));

			tl880_write_register(tl880dev, 0x3070,
					(h << 24) |
					(g << 16) |
					(reg3070 & 0xffff));
			break;

		case 2:
			if(b == 0) {
				reg3074 = tl880_read_register(tl880dev, 0x3074);
				reg3078 = tl880_read_register(tl880dev, 0x3078);
				reg307c = tl880_read_register(tl880dev, 0x307c);

				tl880_write_register(tl880dev, 0x3074, 
						(reg3074 & 0xffff0000) |
						(d << 8) | c);

				tl880_write_register(tl880dev, 0x3078, 
						(reg3078 & 0xffff0000) |
						(f << 8) | e);

				tl880_write_register(tl880dev, 0x307c, 
						(reg307c & 0xffff0000) |
						(h << 8) | g);

				break;
			}
			if(b != 1) {
				break;
			}

			reg3074 = tl880_read_register(tl880dev, 0x3074);
			reg3078 = tl880_read_register(tl880dev, 0x3078);
			reg307c = tl880_read_register(tl880dev, 0x307C);

			tl880_write_register(tl880dev, 0x3074, 
					((d << 24) | 
					 (c << 16)) | 
					(reg3074 & 0xffff));

			tl880_write_register(tl880dev, 0x3078, 
					(f << 24) | 
					(e << 16) | 
					(reg3078 & 0xffff));

			tl880_write_register(tl880dev, 0x307C, 
					(h << 24) | 
					(g << 16) | 
					(reg307c & 0xffff));

			break;

		default:
			break;
	}

	return 0;
}	


/*
 * Register 0x3030 is toggled between the start point of the audio buffer and
 * the midway point.  Perhaps register 3030 is the point at which an audio
 * interrupt is triggered, and this toggling serves as a kind of double
 * buffering mechanism to make it easier to keep the audio fed.
 */
void tl880_ntsc_audio_dpc(struct tl880_dev *tl880dev)
{
	u32 trigger, buf_start, buf_end;
	
	trigger = tl880_read_register(tl880dev, 0x3030);
	buf_start = tl880_read_register(tl880dev, 0x3020);
	buf_end = tl880_read_register(tl880dev, 0x3024);

	if(trigger == buf_start) {
		// Set the interrupt trigger to the middle of the buffer
		// since it just passed the start of the buffer.  This
		// would probably be the point where the upper half of the
		// buffer is read or written.
		tl880_write_register(tl880dev, 0x3030, (buf_start + buf_end) / 2);
	} else {
		// Set the interrupt trigger to the start of the buffer
		// since it just passed the middle of the buffer.  This
		// would probably be the point where the lower half of the
		// buffer is read or written.
		tl880_write_register(tl880dev, 0x3030, buf_start);
	}
}

void tl880_set_ntsc_audio_clock(struct tl880_dev *tl880dev)
{
	if(CHECK_NULL(tl880dev)) {
		return;
	}

	tl880_set_sampling_clock(tl880dev, 3);
	tl880_write_register(tl880dev, 0x27810, 0xa000);
}

void tl880_init_ntsc_audio(struct tl880_dev *tl880dev)
{
	u32 bitsval = 0;
	
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
	//tl880_write_register(tl880dev, 0x3028, 0x3800 /* eax */);
	tl880_write_register(tl880dev, 0x3028, 0);
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

/* loc_32D19: */
	// Clear 64KB of memory
	tl880_clear_sdram(tl880dev, 0x1000, 0x11000, 0);

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
	//set_bits(&bitsval, 0x3004, 7, 7, 1); // Enables I2S recording
	set_bits(&bitsval, 0x3004, 8, 8, 1);
	set_bits(&bitsval, 0x3004, 0xa, 9, 1);
	tl880_write_register(tl880dev, 0x3004, bitsval);

	/*
	 * eax = gpJanus
	 * ecx = bitsval
	 * [gpJanus+167e8] = bitsval
	 * dword[gpJanus+157ec] = 0xc
	 */
}


/*
 * Rate is an enum, not the actual rate
 */
void tl880_set_sampling_clock(struct tl880_dev *tl880dev, int rate)
{
	if(CHECK_NULL(tl880dev)) {
		return;
	}
#if 0
	if(gpJanus == NULL) {
		return;
	}

	cVsbDemod::SpawnVsbTask();
#endif

	/* TODO: MDP-120 and MDP-130 might use tl880_vpx_write_fp as well */
	switch(rate) {
		case 0:
		case 1: 
			if(tl880dev->card_type == TL880_CARD_MYHD_MDP100A) {
				tl880_vpx_write_fp(tl880dev, VPX_FP_OUTMUX, 0x302); // VPX_FP_OUTMUX=0x154
			} else {
				tl880_set_gpio(tl880dev, 9, 2);
				tl880_set_gpio(tl880dev, 8, 0);
			}
			break;
		case 2: 
			if(tl880dev->card_type == TL880_CARD_MYHD_MDP100A) {
				tl880_vpx_write_fp(tl880dev, VPX_FP_OUTMUX, 0x301); // VPX_FP_OUTMUX=0x154
			} else {
				tl880_set_gpio(tl880dev, 9, 0);
				tl880_set_gpio(tl880dev, 8, 2);
			}
			break;
		case 3: // 32kHz
			if(tl880dev->card_type == TL880_CARD_MYHD_MDP100A) {
				tl880_vpx_write_fp(tl880dev, VPX_FP_OUTMUX, 0x300); // VPX_FP_OUTMUX=0x154
			} else {
				tl880_set_gpio(tl880dev, 9, 0);
				tl880_set_gpio(tl880dev, 8, 0);
			}
			break;
	}

#if 0
	cVsbDemod::SpawnVsbTask();
#endif
	return;
}

