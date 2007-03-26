/* 
 * VPX322x control for TL880-based cards (until vpx3220 driver has needed features)
 *
 * Functions primarily based on vpx3220 driver and VPX3226F datasheet.
 *
 * (c) 2007 Mike Bourgeous <nitrogen at users.sourceforge.net>
 *
 * $Log: tl880vpx.c,v $
 * Revision 1.12  2007/03/26 19:25:06  nitrogen
 * Added CVS log generation and updated copyrights and e-mail addresses.
 *
 */
#include "tl880.h"

static unsigned int gfAutoInit = 0;
static unsigned char gbVPX_Adr = 0x86;
static unsigned short gwVPX_Type = 0x7230;
static unsigned short gwVPX_Mode = 1;
static unsigned short gwVPX_Format = 8;
static unsigned short gwVPX_Vact = 0;
static unsigned short gwVPX_Hrefpol = 0;
static unsigned short gwVPX_Vrefpol = 0;
static unsigned short gwVPX_Prefpol = 0;
static unsigned short gwVPX_Keypol = 0;
static unsigned short gwVPX_Llc2pol = 0;
static unsigned short gwVPX_Vlen = 0;
static unsigned short gwVPX_Clkio = 1;
static unsigned short gwVPX_Slope = 0;
static unsigned short gwVPX_Delay = 0;
static unsigned short gwVPX_Pixclk = 1;
static unsigned short gwVPX_UVSwap = 1;
static unsigned short gwVPX_Shuffler = 0;
static unsigned short gwVPX_Byteorder = 0;
static unsigned short gwVPX_Prefsel = 0;
static unsigned short gwVPX_Twosq = 1;
static unsigned short gwVPX_Aen = 1;
static unsigned short gwVPX_Ben = 1;
static unsigned short gwVPX_Zen = 1;
static unsigned short gwVPX_Llcen = 1;
static unsigned short gwVPX_Llc2en = 0;
static unsigned short gwVPX_Ffres = 0;
static unsigned short gwVPX_Stra1 = 7;
static unsigned short gwVPX_Stra2 = 7;
static unsigned short gwVPX_Strb1 = 7;
static unsigned short gwVPX_Strb2 = 7;
static unsigned short gwVPX_Strpixclk = 3;
static unsigned short gwVPX_Hfull = 0x10;
static unsigned short gwVPX_Oldllc = 1;
static unsigned short gwVPX_Prefilter = 0;
static unsigned short gwVPX_Disquit = 0;
static unsigned short gwVPX_Dcen = 0;
static unsigned short gwVPX_Acen = 1;
static unsigned short gwVPX_Acaden = 1;
static unsigned short gwVPX_Fltaden = 1;
static unsigned short gwVPX_Syncslicer = 0x73;
static unsigned short gwVPX_Coeff = 7;
static unsigned short gwVPX_DataLevel = 0x40;
static unsigned short gwVPX_Hsup = 0;
static unsigned short gwVPX_Flagdel = 0;
static unsigned short gwVPX_SplitDis = 0;
static unsigned short gwVPX_DisLim = 0;
static unsigned short gwVPX_VactMode = 0;
static unsigned short gwVPX_Cache = 0;
static unsigned char gbStandard = 1;

int tl880_vpx_read(struct tl880_dev *tl880dev, unsigned char reg)
{
	if(CHECK_NULL(tl880dev)) {
		return -1;
	}

	return tl880_i2c_read_byte_data(&tl880dev->i2cbuses[tl880dev->vpx_i2cbus], tl880dev->vpx_addr, reg);
}

int tl880_vpx_write(struct tl880_dev *tl880dev, unsigned char reg, unsigned char value)
{
	/*
	 * Note - the vpx driver stores written data in an array, which will now be out of sync.
	 * So, the sooner all vpx code is moved over there, the better
	 */
	if(CHECK_NULL(tl880dev)) {
		return -1;
	}

	return tl880_i2c_write_byte_data(&tl880dev->i2cbuses[tl880dev->vpx_i2cbus], tl880dev->vpx_addr, reg, value);
}

static int tl880_vpx_fp_status(struct tl880_dev *tl880dev)
{
	unsigned char status;
	unsigned int i;

	for(i = 0; i < VPX_TIMEOUT_COUNT; i++) {
		status = tl880_vpx_read(tl880dev, VPX_I2C_FPSTA);

		if(!(status & 4)) {
			return 0;
		}

		udelay(10);

		if(need_resched()) {
			cond_resched();
		}
	}

	return -1;
}

unsigned short tl880_vpx_read_fp(struct tl880_dev *tl880dev, unsigned short fpaddr)
{
	int result;
	unsigned short data;

	/* Write the 16-bit address to the FPRD register */
	if((result = tl880_i2c_write_word_data(&tl880dev->i2cbuses[tl880dev->vpx_i2cbus],
					tl880dev->vpx_addr, VPX_I2C_FPRD, __cpu_to_be16(fpaddr))) < 0) {
		printk(KERN_WARNING "tl880: Failed to write vpx FP read register: %d\n", result);
		return -1;
	}

	if(tl880_vpx_fp_status(tl880dev) < 0) {
		return -1;
	}

	/* Read the 16-bit data from the FPDAT register */
	data = tl880_i2c_read_word_data(&tl880dev->i2cbuses[tl880dev->vpx_i2cbus],
					tl880dev->vpx_addr, VPX_I2C_FPDAT);
	if(data == (unsigned short)-1) {
		printk(KERN_WARNING "tl880: Failed to read vpx FP data\n");
		return -1;
	}

	return __be16_to_cpu(data);
}

int tl880_vpx_write_fp(struct tl880_dev *tl880dev, unsigned short fpaddr, unsigned short data)
{
	int result;

	if(CHECK_NULL(tl880dev)) {
		return -1;
	}

	/* Write the 16-bit address to the FPWR register */
	if((result = tl880_i2c_write_word_data(&tl880dev->i2cbuses[tl880dev->vpx_i2cbus], 
					tl880dev->vpx_addr, VPX_I2C_FPWR, __cpu_to_be16(fpaddr))) < 0) {
		printk(KERN_WARNING "tl880: Failed to write vpx FP write register: %d\n", result);
		return -1;
	}

	if(tl880_vpx_fp_status(tl880dev) < 0) {
		return -1;
	}

	/* Write the 16-bit data to the FPDAT register */
	if((result = tl880_i2c_write_word_data(&tl880dev->i2cbuses[tl880dev->vpx_i2cbus], 
					tl880dev->vpx_addr, VPX_I2C_FPDAT, __cpu_to_be16(data))) < 0) {
		printk(KERN_WARNING "tl880: Failed to write vpx FP data: %d\n", result);
		return -1;
	}

	return 0;
}

int tl880_vpx_set_power_register(struct tl880_dev *tl880dev, int status)
{
	/* Set the VPX322x power mode:
	 *   register 0xAA
	 *   	bit[1:0]:	Low power mode
	 *   		  00	Active mode, outputs enabled
	 *   		  01	Outputs tri-stated; clock divided by 2; I2C full speed
	 *   		  10	Outputs tri-stated; clock divided by 4; I2C full speed (< 100kbits/s for 3226F)
	 *   		  11	Outputs tri-stated; clock divided by 8; I2C < 100kbits/s
	 */
	return tl880_vpx_write(tl880dev, VPX_I2C_LLC, status);
}

int tl880_vpx_set_power_status(struct tl880_dev *tl880dev, int status)
{
	/* XXX TODO XXX */
	/* Set the VPX322x power mode:
	 *   register 0xAA
	 *   	bit[1:0]:	Low power mode
	 *   		  00	Active mode, outputs enabled
	 *   		  01	Outputs tri-stated; clock divided by 2; I2C full speed
	 *   		  10	Outputs tri-stated; clock divided by 4; I2C full speed (< 100kbits/s for 3226F)
	 *   		  11	Outputs tri-stated; clock divided by 8; I2C < 100kbits/s
	 */
	return tl880_vpx_write(tl880dev, VPX_I2C_LLC, status);
}

int tl880_vpx_set_video_standard(struct tl880_dev *tl880dev, enum video_standard_e standard)
{
	static int cachefirst = 0; // Set to 1 once the chip has been initialized once
	unsigned short tmp;
	
	// If VPX mode cache enabled and chip has been initialized, check for different state
	if(gwVPX_Cache && cachefirst) {
		if(tl880dev->vpx_video_standard == standard) {
			// State hasn't changed
			return 0;
		}
	} else {
		// First run through
		cachefirst = 1;
	}

	tl880dev->vpx_video_standard = standard;

vpx3226:
	// Removed code for older chip versions

	/* Read the current VPX chip stae */
	tmp = tl880_vpx_read_fp(tl880dev, VPX_FP_SDT); // VPX standard select register
	if(tmp == (unsigned short)-1) {
		return -1;
	}
	tmp &= 0x7f0;	// Preserve standard select option bits, wipe out standard select bits,
			// clear latch bit (should be done after standard change according to doc)

	switch(standard) {
		case 0: // PAL B, G, H, I
		default:
			break;
		case 1: // NTSC M
			tmp |= 1;
			break;
		case 2: // SECAM
			tmp |= 2;
			break;
		case 3: // NTSC44
			tmp |= 3;
			break;
		case 4: // PAL M
			tmp |= 4;
			break;
		case 5: // PAL N
			tmp |= 5;
			break;
		case 6: // PAL 60
			tmp |= 6;
			break;
		case 7: // NTSC COMB
			tmp |= 7;
			break;
		case 9: // Compensated NTSC
			tmp |= 9;
			break;
		case 15:
			return 4;
	}

	/* Write the new state */
	return tl880_vpx_write_fp(tl880dev, VPX_FP_SDT, tmp); // VPX standard select register
}

// Bit 1 of chroma_source: Y/C vs CVBS mode; bit 4 of chroma_source: chroma input line
int tl880_vpx_set_video_source(struct tl880_dev *tl880dev, int luma_source, char chroma_source)
{
	int retval = 0;
	unsigned int tmp1 = 0;
	unsigned int tmp2 = 0;

	if(CHECK_NULL(tl880dev)) {
		return -1;
	}

	switch(luma_source) {
		case 1:
			tmp2 = 2; // Luma from VIN1
			break;
		case 2:
			tmp2 = 1; // Luma from VIN2
			break;
		case 3:
			tmp2 = 0; // Luma from VIN3
			break;
		default:
			printk(KERN_WARNING "tl880: invalid parameters to tl880_vpx_set_video_source: %d %d\n", luma_source, chroma_source);
			return 2;
	}

	switch(chroma_source) {
		case 1:
		case 4:
		case 5:
			break;
		default:
			printk(KERN_WARNING "tl880: invalid parameters to tl880_vpx_set_video_source: %d %d\n", luma_source, chroma_source);
			return 2;
	}

	tmp2 |= chroma_source & 4; // Input path for chroma (0 = VIN1 on old chips, CIN1 on 64-pin 3228F; 1 = CIN2)
	
vpx3226:
	// VPX3225 and later.  Code for older versions omitted
	tmp1 = tl880_vpx_read_fp(tl880dev, VPX_FP_INSEL);

	retval = tl880_vpx_write_fp(tl880dev, VPX_FP_INSEL, (tmp1 & 0x7f8) | tmp2);

	tmp1 = tl880_vpx_read_fp(tl880dev, VPX_FP_SDT);
	retval |= (tmp1 == (unsigned short)-1); // Checking status of read_fp
	tmp1 &= 0x7ff; // Clear the latch bit (telling the VPX to activate the new parameters)

	if(chroma_source & 1) {
		tmp1 |= 0x40; // S-video (Y/C)
	} else {
		tmp1 &= ~0x40; // Composite (CVBS)
	}

	retval |= tl880_vpx_write_fp(tl880dev, VPX_FP_SDT, tmp1);

	//retval |= tl880_vpx_latch_registers(tl880dev, 0x10);
	
	return retval;
}

#ifdef WILLNOTCOMPILE
int tl880_vpx_set_video_attribute(unsigned long arg_0, unsigned long arg_4, unsigned char value)
{
vpx3226:
	// Code for other chips omitted
	switch(arg_4 - 1)
	{
		default:
			return 2;
		case 0: // Brightness 
			if(arg_0 == 1) {
				fpreg = 0x127;
				fplatch = 0x20;
			} else if(arg_0 == 2) {
				fpreg = 0x131;
				fplatch = 0x40;
			} else {
				return 2
			}

			var_c = value - 0x80;
			retval_esi = tl880_vpx_write_fp(tl880dev, fpreg, value - 0x80);

			//retval_esi |= tl880_vpx_latch_registers(fplatch);
			return retval_esi;
		case 1: // Contrast
			if(arg_0 == 1) {
				fpreg = 0x128;
				fplatch = 0x20;
			} else if(arg_0 == 2) {
				fpreg = 0x132;
				fplatch = 0x40;
			} else {
				return 2;
			}

			var_C = tl880_vpx_read_fp(tl880dev, fpreg);
			retval_esi = (var_C == (unsigned short)-1);
			eax = (value >> 2) | (var_C & 0xfc0);

			retval_esi |= tl880_vpx_write_fp(tl880dev, fpreg, eax);

			//retval_esi |= tl880_vpx_latch_registers(fplatch);
			return retval_esi;
		case 2: // Saturation
			if(value == 0) {
				retval_esi = tl880_vpx_write_fp(tl880dev, 0x30, 0);
				retval_esi |= tl880_vpx_write_fp(tl880dev, 0x33, 0x700);
				retval_esi |= tl880_vpx_write_fp(tl880dev, 0x32, 0x700);
				return retval_esi;
			} else if(tl880dev->vpx_video_standard != 2 && value != 0) {
				arg_4 = eax;

				return tl880_vpx_write_fp(tl880dev, 0x30, value << 4);
			}

			if(value > 0x80) {
				fplatch = 3; // var_4 = 3
				fpreg = (value - 0x80) * 0x83 / 0x100 + 0x41;
				ebx = (value - 0x80) * 0xd8 / 0x100 + 0x6C;
				value = 4;
			} else {
				fplatch = 4; // var_4 = 4
				fpreg = (value & 0x83) / 0x80;
				ebx = (value & 0xd8) / 0x80;
				value = 5;
			}

			/*
			 * VPX 3224/3225:
			 * ACC reference level to adjust Cr, Cb levels on picture bus
			 * A value of 0 disables the ACC, chroma gain can be adjusted via
			 * ACCb / ACCr register. The setting is updated when 'sdt' register is
			 * updated.  Default 2070 decimal.
			 *
			 * VPX3226:
			 * Saturation control
			 * bit[11:0] 0..4094dec (2070 corresponds to 100% saturation)
			 *           4095dec disabled (test mode only)
			 *
			 */
			retval_esi = tl880_vpx_write_fp(tl880dev, 0x30, 0);
			
			/*
			 * ACC multiplier value for SECAM Dr chroma comp. to adjust
			 * Cr on pict. bus
			 */
			eax = (value << 8) | ebx;
			retval_esi |= tl880_vpx_write_fp(tl880dev, 0x33, eax);

			/*
			 * ACC multiplier value for SECAM Db chroma comp. to adjust
			 * Cb on pict. bus
			 */
			eax = (fplatch << 8) | fpreg;
			retval_esi |= tl880_vpx_write_fp(tl880dev, 0x32, eax);
			return retval_esi;
		case 3: // Hue
			arg_4 = value * 4 - 512; // lea eax, ds:0fffffe00h[eax*4]

			tl880_vpx_write_fp(tl880dev, 0xdc, arg_4); // NTSC tint angle

			retval_esi = eax;
			return retval_esi;
		case 9: // Sharpness
		case 4:
			if(arg_0 == 1) {
				fpreg = 0x126;
				fplatch = 0x20;
			}
			if(arg_0 == 2) {
				fpreg = 0x130;
				fplatch = 0x40;
			} else {
				return 2;
			}

			arg_4 = tl880_vpx_read_fp(fpreg, eax);
			retval_esi = (arg_4 == (unsigned short)-1);

			arg_0 = value >> 5;
			arg_4 = (arg_0 << 2) | (arg_4 & 0xfe3);

			retval_esi |= tl880_vpx_write_fp(tl880dev, fpreg, eax);

			//tl880_vpx_latch_registers(fplatch);
			retval_esi |= eax;
			return retval_esi;
		case 5:
			if(arg_0 == 1) {
				fpreg = 0x126;
				fplatch = 0x20;
			} else if(arg_0 == 2) {
				fpreg = 0x130;
				fplatch = 0x40;
			} else {
				return 2;
			}

			arg_4 = tl880_vpx_read_fp(tl880dev, fpreg);
			retval_esi = (arg_4 == (unsigned short)-1);

			arg_4 = (arg_4 & 0xffc) | (value >> 6);

			retval_esi |= tl880_vpx_write_fp(tl880dev, fpreg, arg_4);

			//tl880_vpx_latch_registers(fplatch);
			retval_esi |= eax;
			return retval_esi;
		case 6:
			return 4;
		case 7:
			arg_4 = tl880_vpx_read_fp(tl880dev, 0x21);
			retval_esi = (arg_4 == (unsigned short)-1);

			arg_0 = value >> 6;;
			arg_4 = (arg_0 << 5) | (arg_4 & 0x79f);

			retval_esi |= tl880_vpx_write_fp(tl880dev, 0x21, arg_4);
			return retval_esi;
		case 8:
			if(arg_0 == 1) {
				fpreg = 0x128;
				fplatch = 0x20;
			} else if(arg_0 == 2) {
				fpreg = 0x132;
				fplatch = 0x40;
			} else {
				return 2;
			}

			arg_4 = tl880_vpx_read_fp(tl880dev, fpreg);
			retval_esi = (arg_4 == (unsigned short)-1);
			arg_0 = value >> 6;

			/* Is this toggling something */
			if(arg_0 == 0) {
				arg_0 = 1; // arg_0 is a pointer?
			} else if(arg_0 == 1) {
				arg_0 = 0;
			}

			arg_4 = (arg_0 << 6) | (arg_4 & 0xf3f);

			retval_esi |= tl880_vpx_write_fp(tl880dev, fpreg, arg_4);

			//tl880_vpx_latch_registers(fplatch);
			retval_esi |= eax;
	}
	return retval_esi;
}
#endif /* WILLNOTCOMPILE */

void tl880_vpx_config(struct tl880_dev *tl880dev)
{
	// Load default VPX settings
	// LoadDefaultSettings()
	
	printk(KERN_DEBUG "tl880: Configuring the VPX chip\n");
	
	// Initialize the VPX chip
	tl880_vpx_init(tl880dev);

	// Set the initial video standard
	tl880_vpx_set_video_standard(tl880dev, NTSC_COMB);

	// Set the initial video source
	tl880_vpx_set_video_source(tl880dev, 2, 4);

	// Set the video region decoded (?)
	//tl880_vpx_set_video_window(tl880dev, 1, 22, 240, 240, 14, 720, 734, 3000, 0);
	//tl880_vpx_set_video_window(tl880dev, 2, 21,   1,   1,  0, 160, 160,    0, 0);
	//tl880_vpx_set_video_window(tl880dev, 3,  0,   0,   0,  0,   0,   0,    0, 0);

	// Set other video attributes
	//tl880_vpx_set_video_attribute(tl880dev, 1, 2, 144);
	//tl880_vpx_set_video_attribute(tl880dev, 1, 1, 112);
	//tl880_vpx_set_video_attribute(tl880dev, 1, 3, 192);
	//tl880_vpx_set_video_attribute(tl880dev, 1, 4, 128);
	//tl880_vpx_set_video_attribute(tl880dev, 1, 5, 128);
	//tl880_vpx_set_video_attribute(tl880dev, 1, 6, 128);
	//tl880_vpx_set_video_attribute(tl880dev, 1, 10, 180);
}

// A commentary on the initialization procedure of the VPX chip
void tl880_vpx_init(struct tl880_dev *tl880dev)
{
	unsigned long tmp1, tmp2, tmp3, tmp4;
	int somevar = 0xcc;
	short version = 0xcc;

	// Detect the VPX hardware version
	//tl880_vpx_detect_device(tl880dev);
	
	// Detect the VPX firmware version
	//tl880_vpx_get_firmware_version(tl880dev,&version);

	// Disable power-saving mode
	tl880_vpx_set_power_status(tl880dev, 0);

vpx3226: // Other VPX revisions have a bunch of stuff written to I2C above here
	if(gwVPX_Llc2pol != 0) { // llc2pol is set to 0 in LoadDefaultSettings
		tmp2 = 0x10; // register 0xaa bit[4]: LLC2 polarity
	} else {
		// Funky optimization way of saying if(oldllc) then set bit 6
		tmp2 = (-gwVPX_Oldllc) & 0xffff;
		tmp2 -= tmp2;
		tmp2 &= 0x40; // register 0xaa bit[6]: old llc timing with longer hold time
	}
	tl880_vpx_write(tl880dev, VPX_I2C_LLC, tmp2); // LLC settings

	tmp1 = gwVPX_Llc2en;	// LLC2 to TDO (if using JTAG in test-logic-reset state)
	tmp1 <<= 1;
	tmp1 |= gwVPX_Zen;		// HREF, VREF, FIELD, VACT, LLC, LLC2 enable
	tmp1 <<= 1;
	tmp1 |= gwVPX_Clkio;	// Pixclock output enable
	tmp1 <<= 1;
	tmp1 |= gwVPX_Ben;		// Video port B enable (disable = tristate)
	tmp1 <<= 1;
	tmp1 |= gwVPX_Aen;		// Video port A enable (disable = tristate)
	tl880_vpx_write(tl880dev, VPX_I2C_OENA, tmp1); // Output enable register

	tmp1 = gwVPX_Strpixclk;	// additional PIXCLK driving strength (if needed different from below)
	tmp1 <<= 3;
	tmp1 |= gwVPX_Stra2;	// PIXCLK, LLC, VACT driving strength
	tmp1 <<= 3;
	tmp1 |= gwVPX_Stra1;	// Video port A driving strength
	tl880_vpx_write(tl880dev, VPX_I2C_DRIVER_A, tmp1); // Driving strength register

	tmp1 = gwVPX_Strb2;	// HREF, VREF, FIELD, LLC2 driving strength
	tmp1 <<= 3;
	tmp1 |= gwVPX_Strb1;	// Video port B driving strength
	tl880_vpx_write(tl880dev, VPX_I2C_DRIVER_B, tmp1); // Driving strength register

	tmp1 = gwVPX_Vlen;	// VREF pulse width (VPX adds two to this number)
	tmp1 <<= 1;
	tmp1 |= gwVPX_Vrefpol;	// VREF polarity (0 = active high)
	tmp1 <<= 1;
	tmp1 |= gwVPX_Hrefpol;	// HREF polarity (0 = active low)
	tmp1 <<= 1;
	tmp1 |= gwVPX_Prefpol;	// Polarity of field pin (0 = odd high, 1 = even high)
	tl880_vpx_write_fp(tl880dev, VPX_FP_REFSIG, tmp1); // HREF and VREF control 

	switch(gwVPX_Format) {
		case 0:
		default:
			tmp3 = 0;
		case 8:
			tmp3 = 1;
		case 9:
			tmp3 = 2;
	}

	tmp1 = gwVPX_Flagdel & 1;	// Change ITU-R656 header flags in 0: SAV, 1: EAV
	tmp1 <<= 1;
	tmp1 |= gwVPX_Hsup & 1;		// Suppress ITU-R656 headers for blank lines
	tmp1 <<= 1;
	tmp1 |= gwVPX_UVSwap & 1;	// Specs describe this bit as "reserved"
	tmp1 <<= 1;
	tmp1 |= gwVPX_SplitDis & 1;	// Disable teletext splitting when enabled
	tmp1 <<= 4;			// Skipping over VBI data range, transmission mode, pixclk selection
	tmp1 |= gwVPX_Shuffler & 1;	// Shuffler (0: Video Port A=Y, Port B=CbCr, 1: vice versa)
	tmp1 <<= 2;
	tmp1 |= tmp3;			// bits 1:0 YCbCr 422 format selection (2: bitstream, 1: ITU-R656, 0: ITU-R601)
	tl880_vpx_write_fp(tl880dev, VPX_FP_FORMAT_SEL, tmp1); // Format selection register

	somevar = tl880_vpx_read_fp(tl880dev, VPX_FP_FORMAT_SEL); // Why?

 	// Set output multiplexer register
 	switch(tl880dev->card_type) {
		case TL880_CARD_JANUS:
			// multipurpose bits and double clock active, port B is 00010001
			tl880_vpx_write_fp(tl880dev, VPX_FP_OUTMUX, 0x311);
			break;
		case TL880_CARD_MYHD_MDP100A:
			// multipurpose bits and double clock active, port B output is 0
			tl880_vpx_write_fp(tl880dev, VPX_FP_OUTMUX, 0x300);
			break;
		case TL880_CARD_MYHD_MDP100:
		case TL880_CARD_MYHD_MDP110:
		case TL880_CARD_MYHD_MDP120: // TODO: Need to confirm 120 and 130
		case TL880_CARD_MYHD_MDP130:
			// multipurpose bits and double clock active, port B output is 00000001
			tl880_vpx_write_fp(tl880dev, VPX_FP_OUTMUX, 0x301);
			break;
		default:
			printk(KERN_WARNING "tl880: VPX init not yet finished for this card type\n");
			break;
	}

	tl880_vpx_write_fp(tl880dev, VPX_FP_PVAL_START, 0);	// Start position of video active signal (chip default is 40)
	tl880_vpx_write_fp(tl880dev, VPX_FP_PVAL_STOP, 0x320);	// 800 - end position of video active (chip default is 720)

	// Mode for video active signal (programmable vs. window size) and timing mode update latch
	tl880_vpx_write_fp(tl880dev, VPX_FP_CONTROLWORD, gwVPX_VactMode << 2 | 0x800); // Control and latching register
	tl880_vpx_write_fp(tl880dev, VPX_FP_REG174, 4);	// Undocumented register? (not in specs)

	if(1 /*vpx type is 3226*/) {
		tl880_vpx_write_fp(tl880dev, VPX_FP_PEAKING1, 0x300); // Coring/peaking: swap chroma, peaking uses low frequency
		tl880_vpx_write_fp(tl880dev, VPX_FP_SDT, 0x21);   // Standard selection: NTSC M + 4H Comb filter (pg 50)
		tl880_vpx_write_fp(tl880dev, VPX_FP_COMB_UC, 0xfff);  // Comb filter: max peaked, max diagonal dot reduction,
								  // max horiz diff gain, min vert diff gain, max vertical peak
	}
}

