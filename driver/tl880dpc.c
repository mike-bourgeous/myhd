/* 
 * Video mode stuff for TL880 cards
 *
 * (c) 2003-2007 Mike Bourgeous <nitrogen at users.sourceforge.net>
 *
 * $Log: tl880dpc.c,v $
 * Revision 1.20  2007/09/06 05:22:05  nitrogen
 * Improvements to audio support, documentation, and card memory management.
 *
 * Revision 1.19  2007/04/24 06:32:13  nitrogen
 * Changed most int/long types to explicit 32-bit sizes.  Fixed compilation and execution on 64-bit CPUs.
 *
 * Revision 1.18  2007/03/26 19:25:05  nitrogen
 * Added CVS log generation and updated copyrights and e-mail addresses.
 *
 */
#include "tl880.h"

unsigned long tl880_calc_dpc_pll_const(unsigned long a, unsigned char b, unsigned char c)
{
	return (((((a & 0x3ff) << 8) | (b & 0x1f)) << 4) | (c & 3)) << 4;
}


void tl880_set_dpc_pll_const(struct tl880_dev *tl880dev, unsigned long a, unsigned char b, unsigned char c)
{
	tl880_write_register(tl880dev, 0x5800, tl880_calc_dpc_pll_const(a, b, c));
}

/* This function has not been fully tested and may be incorrect */
/* I would probably discard this function and store these values as part of a mode array */
void tl880_set_dpc_clock(struct tl880_dev *tl880dev, unsigned long xres, unsigned long yres, long interlace)
{
	unsigned long val = 0;
	unsigned long gpio_val = 1;
	
	/* tagContext->0x264 is xres */

	switch(xres) {
		case 1024:
			val = 0x01151bb0;
			break;
		case 1440:
			val = 0x00e619b0;
			break;
		case 1280:
		case 1920:
			if(1 /* cJanus->0x16720 */) {
				val = 0x98280;
				gpio_val = 0;
			} else {
				val = tl880_calc_dpc_pll_const(0x14, 2, 0);
			}
			break;
		default:
			if(interlace) {
				val = 0x60070;
			} else if(1 /* cJanus->0x16720 != 1 */) {
				val = 0x60050;
			} else if(0 /* cJanus->0x16724 == 0x10 */) {
				val = 0x60050;
			} else {
				val = 0x60070;
			}
			break;
	}

	tl880_write_register(tl880dev, 0x5800, val);
	tl880_write_register(tl880dev, 0x5d14, 0);
	tl880_set_gpio(tl880dev, 6, gpio_val);
}


void tl880_init_dpc_pll(struct tl880_dev *tl880dev)
{
	int i;
	
	tl880_write_register(tl880dev, 0x10014, 0x80000000);

	tl880_set_dpc_clock(tl880dev, 1024, 768, 0);

	for(i = 0; i < 10; i++) {
		tl880_read_register(tl880dev, 0x10014);
	}

	tl880_write_register(tl880dev, 0x10014, 0);
}


void tl880_set_mode(struct tl880_dev *tl880dev, struct tl880_mode_def *mode)
{
	u32 reg;
	u32 savereg;
	u32 value = 0;

	if(CHECK_NULL(tl880dev) || CHECK_NULL(mode)) {
		printk(KERN_ERR "tl880: NULL value passed to tl880_set_mode\n");
		return;
	}

	savereg = tl880_read_register(tl880dev, 0x10000);
	tl880_write_register(tl880dev, 0x10000, 0x80);

	reg = 0x10014;
	set_bits(&value, reg, 0, 0, mode->a);
	set_bits(&value, reg, 1, 1, mode->b);
	set_bits(&value, reg, 2, 2, mode->c);
	set_bits(&value, reg, 3, 3, mode->prog_scan);
	set_bits(&value, reg, 0xf, 6, mode->e);
	set_bits(&value, reg, 0x1a, 0x10, mode->xres);
	set_bits(&value, reg, 0x1b, 0x1b, mode->g);
	set_bits(&value, reg, 0x1c, 0x1c, mode->h);
	set_bits(&value, reg, 0x1d, 0x1d, mode->i);
	set_bits(&value, reg, 0x1e, 0x1e, mode->j);
	tl880_write_register(tl880dev, reg, value);

	reg = 0x10018;
	value = 0;
	set_bits(&value, reg, 8, 0, mode->h_backporch); 
	set_bits(&value, reg, 9, 9, mode->inv_hsync);
	set_bits(&value, reg, 0xa, 0xa, mode->inv_vsync);
	set_bits(&value, reg, 0x12, 0xc, mode->h_synclen);
	set_bits(&value, reg, 0x1c, 0x14, mode->h_frontporch);
	tl880_write_register(tl880dev, reg, value);

	reg = 0x1001c;
	value = 0;
	set_bits(&value, reg, 2, 0, mode->v_synclen_0);
	set_bits(&value, reg, 0xb, 4, mode->v_frontporch_0);
	set_bits(&value, reg, 0x16, 0xc, mode->yres_0);
	set_bits(&value, reg, 0x1b, 0x18, mode->v_backporch_0);
	set_bits(&value, reg, 0x1c, 0x1c, mode->ntsc_flag);
	tl880_write_register(tl880dev, reg, value);

	reg = 0x10020;
	value = 0;
	set_bits(&value, reg, 2, 0, mode->v_synclen_1);
	set_bits(&value, reg, 0xb, 4, mode->v_frontporch_1);
	set_bits(&value, reg, 0x16, 0xc, mode->yres_1);
	set_bits(&value, reg, 0x1b, 0x18, mode->v_backporch_1);
	tl880_write_register(tl880dev, reg, value);
	
	reg = 0x10028;
	value = 0;
	set_bits(&value, reg, 0xb, 0, mode->bb);
	set_bits(&value, reg, 0x18, 0x18, mode->cc);
	tl880_write_register(tl880dev, reg, value);
	
	reg = 0x10024;
	value = 0;
	set_bits(&value, reg, 0x15, 0, mode->y);
	set_bits(&value, reg, 0x18, 0x18, mode->z);
	set_bits(&value, reg, 0x1f, 0x1c, mode->aa);
	tl880_write_register(tl880dev, reg, value);

	tl880_write_register(tl880dev, 0x5800, mode->dd);

	tl880_write_register(tl880dev, 0x10000, savereg);
}


void tl880_dpc_video_sync_dvd(struct tl880_dev *tl880dev)
{
	static unsigned long val = 1;
	static unsigned long field_select = 0;
	static unsigned long delay_cnt = 0;
	static unsigned long dpc_hsync_cnt = 0;
	unsigned long var_10 = 0;
	
	/*
	edi = gpJanus;
	esi = cJanus->0x10388;
	ebx = 0;
	*/
	dpc_hsync_cnt++;
	if(0 /* (cJanus->0x10388)->0x26c != 0 */) {
		goto loc_294ea;
loc_294ea:
		/* ebp = 1; */
		
		/* field_select xor= ebp */
		field_select ^= 1;
		
		tl880_write_regbits(tl880dev, 0x10000, 0, 0, field_select);
		
		if(field_select == 0) {
			var_10 = 0;
			
			goto loc_295c9;
		}
		
		var_10 = 1;
		
		goto loc_29511;
	}

	/* val = (cJanus->0x10388)->0x270; */
	val ^= 1;
	
	if(tl880_read_regbits(tl880dev, 0x10004, 2, 2)) {
		if(val == 3 || val == 1) {
			tl880_write_regbits(tl880dev, 0x10000, 0, 0, 1);
		} else {
			tl880_write_regbits(tl880dev, 0x10000, 0, 0, 0);
		}
		
		var_10 = 1;
		
		goto loc_29511;
	}

	if(val == 3 || val == 1) {
		tl880_write_regbits(tl880dev, 0x10000, 0, 0, 0);
	} else {
		tl880_write_regbits(tl880dev, 0x10000, 0, 0, 1);
	}

	var_10 = 0;

	goto loc_295c9;

loc_29511:
	/* ebp = &(cJanus->0xf4); */
	if(0 /* cJanus->0xf4 != 0 && cJanus->0xf8 != 0 */) {
		/* StartMcu(); */
		
		//cJanus->0xf4 = 0;
		if(0 /* (cJanus->0x10388)->0x27c != 0 */) {
			if(0 /* (cJanus->0x10388)->0x284 != 0 */) {
				/* StartVpip(cJanus->0x10388) */
				
				/*
				(cJanus->0x10388)->0xb70 = 0;
				*/
			}
		} else {
			if(0 /* (cJanus->0x10388)->0x284 != 0 */) {
				/*
				(cJanus->0x10388)->0xb70 = 1;
				*/
			}
		}
		
		if(0 /* (cJanus->0x10388)->0xd4 == 3 */) {
			/* (cJanus->0x10388)->0xb68 = 2; */
		} else {
			/* (cJanus->0x10388)->0xb68 += 1; */
		}
		
		delay_cnt = 0;
		return;
	}

	if(delay_cnt <= 0) {
		/* ebp = &(cJanus->0x10377); */
		if(0 /* cJanus->0x10377 != 0 */) /* byte */ {
			tl880_enable_bvdo(tl880dev);
			
			//(cJanus->0x10377)->0 = 0; /* byte */
		}
	
		/* ebp = &(cJanus->0x10364); */
		if(0 /* cJanus->0x10364 != 0 */) /* byte */ {
			tl880_enable_aux(tl880dev);
			
			//(cJanus->0x10364) = 0; /* byte */
		}
	} else {
		delay_cnt--;
	}

loc_295c9:
	if(0 /* (cJanus->0x10388)->0xb6c == 0 || cJanus->0xf8 != 0 */) {
		return;
	}

	/*
	ecx = (cJanus->0x10388)->0xba0;
	eax = &((cJanus->0x10388)->0xba0);
	*/
	if(0 /* (cJanus->0x10388)->0xba0 > 0 */) {
		/*
		ecx -= 1;
		[eax] = ecx;
		*/
		/*
		(cJanus->0x10388)->0xba0 -= 1;
		*/
		
		return;
	}

	if(1 /* (cJanus->0x10388)->0x27c != 0 */) {
		if(0 /*DpcSync(&(cJanus->0x10388), var_10)*/) {
			return;
		}
		
		/* UpdateDispBufReg((cJanus->0x10388)->0x1d4, (cJanus->0x10388)->0x1d8); */
		
		if(0 /* (cJanus->0x10388)->0x284 != 0 && (cJanus->0x10388)->0xb70 != 0 */) {
			/* UpdateVpipBufReg(&(cJanus->0x10388)); */
			
			/* StartVpip(&(cJanus->0x10388)); */
			
			/*
			(cJanus->0x10388)->0 = 0;
			*/
		}
	} else {
		if(0 /* (cJanus->0x10388)->0x28c != 0 && (cJanus->0x10388)->0x284 != 0 */) {
			if(0 /* DeinterlaceSync(&(cJanus->0x10388), var_10); */) {
				return;
			}
		} else {
			if(0 /* DpcSync(&(cJanus->0x10388), var_10) */) {
				return;
			}
			
			/* UpdateDispBufReg((cJanus->0x10388)->0x1d4, (cJanus->0x10388)->0x1d8); */
		}
	}

	/* eax = devGetSTCRaw(); // Demux_GetSTC() */

	/*
	ecx = 0x5a;
	edx = 0;
	eax /= 0x5a;
	(cJanus->0x10388)->0xba4 = eax;
	*/

	/* StartMcu(); */

	/* (cJanus->0x10388)->0xb6c = 0; */

	/* This is to make gcc shut up: */
	var_10 = 0;
}

void tl880_dpc_video_sync(struct tl880_dev *tl880dev)
{
	unsigned int playing_dvd = 0;

	static unsigned long dpc_sync_count = 0;
	static unsigned long last_jiffies = 0;
	unsigned long this_jiffies;

	this_jiffies = jiffies;
	if(debug > 0 || dpc_sync_count == 2) {
		printk(KERN_DEBUG "tl880: dpc video sync interrupt - count %lu, time ~%lums\n", dpc_sync_count, (this_jiffies - last_jiffies) * 1000 / HZ);
	}
	last_jiffies = this_jiffies;

	if(playing_dvd) {
		tl880_dpc_video_sync_dvd(tl880dev);
	} else {
		// tl880_dpc_video_sync_tv(tl880dev);
	}
}

void tl880_dpc_field0(struct tl880_dev *tl880dev) /* {{{ */
{
	static unsigned long dpc_eof0_count = 0;
	static unsigned long last_jiffies = 0;
	unsigned long this_jiffies;

	dpc_eof0_count++;

	/*
	CDma = (cJanus->0)->0x110;
	*/

	/* VopOn returns bit 4 of 0x10000 */
	if(!tl880_dma_vop_on(tl880dev)) {
		return;
	}

	/* eax = cJanus->0x10388; */
	if(0 /* (cJanus->0x10388)->0x26c == 0 */) {
		/* CDma::VopIsrOdd(); */
		return;
	}

	/* eax = cJanus->0; */
	/* ecx = (cJanus->0)->0x110; */
	if(1 /* (cJanus->0x10388)->0xbb0 == 1 */) {
		/* CDma::VopIsrEven(); */
	} else {
		/* CDma::VopIsrOdd(); */
	}

	this_jiffies = jiffies;
	if(debug > 0 || dpc_eof0_count == 2) { 
		printk(KERN_DEBUG "tl880: dpc field0 interrupt - count %lu, time ~%lums\n", dpc_eof0_count, (this_jiffies - last_jiffies) * 1000 / HZ);
	}
	last_jiffies = this_jiffies;
} /* }}} */

void tl880_dpc_field1(struct tl880_dev *tl880dev) /* {{{ */
{
	static unsigned long dpc_eof1_count = 0;
	static unsigned long last_jiffies = 0;
	unsigned long this_jiffies;

	dpc_eof1_count++;

	if(0 /* !CDma::VopOn(); */) {
		return;
	}

	/* eax = cJanus->0; */
	/* ecx = (cJanus->0)->0x110; */
	/* CDma::VopIsrEven(); */

	this_jiffies = jiffies;
	if(debug > 0 || dpc_eof1_count == 2) {
		printk(KERN_DEBUG "tl880: dpc field1 interrupt - count %lu, time ~%lums\n", dpc_eof1_count, (this_jiffies - last_jiffies) * 1000 / HZ);
	}
	last_jiffies = this_jiffies;
} /* }}} */

/*
 * DPC interrupt handler
 */
int tl880_dpc_int(struct tl880_dev *tl880dev) /* {{{ */
{
	static unsigned long first_jiffies = 0;

	if(!first_jiffies) {
		first_jiffies = jiffies;
	}

	tl880dev->dpc_type = tl880_read_register(tl880dev, 0x1000c) & tl880_read_register(tl880dev, 0x10008);

	/*
	printk(KERN_DEBUG "tl880: dpc interrupt: 0x%04lx - count %lu\n", 
		tl880dev->dpc_type, tl880dev->dpc_count);
	*/

	if(tl880dev->dpc_type & 4) {
		tl880_dpc_video_sync(tl880dev);
	}

	if(tl880dev->dpc_type & 0x20) {
		tl880_dpc_field0(tl880dev);
	}

	if(tl880dev->dpc_type & 0x40) {
		tl880_dpc_field1(tl880dev);
	}

	tl880dev->dpc_type = 0;

#if 0
	/* Since this interrupt isn't fully handled yet, disable it after a while */
	if(tl880dev->dpc_count >= 600) {
		tl880_write_register(tl880dev, 0x10008, 0);
		printk(KERN_DEBUG "tl880: ~%lu dpc interrupts per second\n", tl880dev->dpc_count * HZ / (jiffies - first_jiffies));
	}
#endif /* 0 */

	if((debug > 0 && tl880dev->dpc_count % 60 == 0) || tl880dev->dpc_count % 1200 == 0) {
		printk(KERN_DEBUG "tl880: ~%lu dpc interrupts per second\n", tl880dev->dpc_count * HZ / (jiffies - first_jiffies));
	}

	return 0;
} /* }}} */


/* Video display enable */
void tl880_enable_bvdo(struct tl880_dev *tl880dev) /* {{{ */
{
	tl880_write_regbits(tl880dev, 0x10000, 1, 1, 1);
} /* }}} */

/* Video display disable */
void tl880_disable_bvdo(struct tl880_dev *tl880dev) /* {{{ */
{
	tl880_write_regbits(tl880dev, 0x10000, 1, 1, 0);
} /* }}} */


/* Overlay video enable */
void tl880_enable_aux(struct tl880_dev *tl880dev) /* {{{ */
{
	tl880_write_regbits(tl880dev, 0x10000, 4, 4, 1);
} /* }}} */

/* Overlay video disable */
void tl880_disable_aux(struct tl880_dev *tl880dev) /* {{{ */
{
	tl880_write_regbits(tl880dev, 0x10000, 4, 4, 0);
} /* }}} */

#if 0
/* Overlay video enable */
void tl880_enable_aux(struct tl880_dev *tl880dev) /* {{{ */
{
	tl880_write_regbits(tl880dev, 0x10000, 4, 4, 1);
} /* }}} */

/* Overlay video disable */
void tl880_disable_aux(struct tl880_dev *tl880dev) /* {{{ */
{
	tl880_write_regbits(tl880dev, 0x10000, 4, 4, 0);
} /* }}} */
#endif

