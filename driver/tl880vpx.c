/* 
 * VPX322x control for TL880-based cards (until vpx3220 driver has needed features)
 *
 * Functions based on vpx3220 driver and VPX3226F datasheet.
 *
 * (c) 2007 Mike Bourgeous <nitrogen at users.sourceforge.net>
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
		status = tl880_vpx_read(tl880dev, 0x29);

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
					tl880dev->vpx_addr, 0x26, __cpu_to_be16(fpaddr))) < 0) {
		printk(KERN_WARNING "tl880: Failed to write vpx FP read register: %d\n", result);
		return -1;
	}

	if(tl880_vpx_fp_status(tl880dev) < 0) {
		return -1;
	}

	/* Read the 16-bit data from the FPDAT register */
	data = tl880_i2c_read_word_data(&tl880dev->i2cbuses[tl880dev->vpx_i2cbus],
					tl880dev->vpx_addr, 0x28);
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
					tl880dev->vpx_addr, 0x27, __cpu_to_be16(fpaddr))) < 0) {
		printk(KERN_WARNING "tl880: Failed to write vpx FP write register: %d\n", result);
		return -1;
	}

	if(tl880_vpx_fp_status(tl880dev) < 0) {
		return -1;
	}

	/* Write the 16-bit data to the FPDAT register */
	if((result = tl880_i2c_write_word_data(&tl880dev->i2cbuses[tl880dev->vpx_i2cbus], 
					tl880dev->vpx_addr, 0x28, __cpu_to_be16(data))) < 0) {
		printk(KERN_WARNING "tl880: Failed to write vpx FP data: %d\n", result);
		return -1;
	}

	return 0;
}

int tl880_vpx_set_power_status(struct tl880_dev *tl880dev, int status)
{
	/* Set the VPX322x power mode:
	 *   register 0xAA
	 *   	bit[1:0]:	Low power mode
	 *   		  00	Active mode, outputs enabled
	 *   		  01	Outputs tri-stated; clock divided by 2; I2C full speed
	 *   		 *10	Outputs tri-stated; clock divided by 4; I2C full speed (< 100kbits/s for 3226F)
	 *   		  11	Outputs tri-stated; clock divided by 8; I2C < 100kbits/s
	 */
	return tl880_vpx_write(tl880dev, VPX_I2C_LLC, status);
}

int tl880_vpx_set_video_standard(struct tl880_dev *tl880dev, enum video_standard_e standard)
{
	static int cachefirst = 0; // Set to 1 once the chip has been initialized once
	unsigned short tmp;
	
	if(/* gwVPX_Cache && */ cachefirst) {
		if(/*gbStandard*/ tl880dev->vpx_video_standard == standard) {
			return 0;
		}
	} else {
		cachefirst = 1;
	}

	/* gbStandard = standard; */
	tl880dev->vpx_video_standard = standard;
	/*
	if(gwVPX_Type == 0x3350) { // vpx3226e/f
		goto vpx_3226;
	}
	// Removed code for older chip versions
vpx_3226:
	*/

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
	//tl880_vpx_set_video_source(tl880dev, 2, 4);

	// Set the video region decoded (tl880dev, ?)
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
	unsigned long a, b, c, d, arg;
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
		b = 0x10; // register 0xaa bit[4]: LLC2 polarity
	} else {
		// Funky optimization way of saying if(oldllc) then set bit 6
		b = (-gwVPX_Oldllc) & 0xffff;
		b -= b;
		b &= 0x40; // register 0xaa bit[6]: old llc timing with longer hold time
	}
	//i2cwrite8(VPX_byte_Adr, 0xaa, b); // LLC settings
	tl880_vpx_write(tl880dev, VPX_I2C_LLC, b);

	a = 0;
	a = gwVPX_Llc2en;	// LLC2 to TDO (if using JTAG in test-logic-reset state)
	a <<= 1;
	a |= gwVPX_Zen;	// HREF, VREF, FIELD, VACT, LLC, LLC2 enable
	a <<= 1;
	a |= gwVPX_Clkio;	// Pixclock output enable
	a <<= 1;
	a |= gwVPX_Ben;	// Video port B enable (disable = tristate)
	a <<= 1;
	a |= gwVPX_Aen;	// Video port A enable (disable = tristate)
	//i2cwrite8(VPX_byte_Adr, 0xf2, a); // Output enable register
	tl880_vpx_write(tl880dev, VPX_I2C_OENA, a);

	a = 0;
	a = gwVPX_Strpixclk;// additional PIXCLK driving strength (if needed different from below)
	a <<= 3;
	a |= gwVPX_Stra2;	// PIXCLK, LLC, VACT driving strength
	a <<= 3;
	a |= gwVPX_Stra1;	// Video port A driving strength
	//i2cwrite8(VPX_byte_Adr, 0xf8, a); // Driving strength register
	tl880_vpx_write(tl880dev, VPX_I2C_DRIVER_A, a);

	a = 0;
	a = gwVPX_Strb2;	// HREF, VREF, FIELD, LLC2 driving strength
	a <<= 3;
	a |= gwVPX_Strb1;	// Video port B driving strength
	//i2cwrite8(VPX_byte_Adr, 0xf9, a); // Driving strength register
	tl880_vpx_write(tl880dev, VPX_I2C_DRIVER_B, a);

	a = 0;
	a = gwVPX_Vlen;	// VREF pulse width (VPX adds two to this number)
	a <<= 1;
	a |= gwVPX_Vrefpol;	// VREF polarity (0 = active high)
	a <<= 1;
	a |= gwVPX_Hrefpol;	// HREF polarity (0 = active low)
	a <<= 1;
	a |= gwVPX_Prefpol;	// Polarity of field pin (0 = odd high, 1 = even high)
	tl880_vpx_write_fp(tl880dev, VPX_FP_REFSIG, a); // HREF and VREF control 

	switch(gwVPX_Format) {
		case 0:
		default:
			d = 0;
		case 8:
			d = 1;
		case 9:
			d = 2;
	}

	a = 0;
	a = gwVPX_Flagdel;	// Change ITU-R656 header flags in 0: SAV, 1: EAV
	c = 0;
	c = gwVPX_Hsup;		// Suppress ITU-R656 headers for blank lines
	a &= 1;
	a <<= 1;
	c &= 1;
	a |= c;
	c = 0;
	c = gwVPX_UVSwap;	// Specs describe this bit as "reserved"
	a <<= 1;
	c &= 1;
	a |= c;
	c = 0;
	c = gwVPX_SplitDis;	// Disable teletext splitting when enabled
	a <<= 1;
	c &= 1;
	a |= c;
	c = 0;
	c = gwVPX_Shuffler;	// Shuffler (0: Video Port A=Y, Port B=CbCr, 1: vice versa)
	a <<= 4;		// Skipping over VBI data range, transmission mode, pixclk selection
	c &= 1;
	a |= c;
	a <<= 2;
	a |= d;		// bits 1:0 YCbCr 422 format selection (2: bitstream, 1: ITU-R656, 0: ITU-R601)
	tl880_vpx_write_fp(tl880dev, VPX_FP_FORMAT_SEL, a); // Format selection register

	somevar = tl880_vpx_read_fp(tl880dev, VPX_FP_FORMAT_SEL); // Why?

	b = tl880dev->card_type;
	if(b == 1) {
		arg = 0x311; // 785 (multipurpose bits and double clock active, port B is 00010001)
		goto C;
	}

	if(b == 2) {
		arg = 0x300; // 768 (multipurpose bits and double clock active, port B output is 0)
		goto C;
	}

	if(b >= 3) {
		arg = 0x301; // 769 (multipurpose bits and double clock active, port B output is 00000001)
		goto C;
	}
	goto C;

C:
	tl880_vpx_write_fp(tl880dev, VPX_FP_OUTMUX, arg/*[0x301 or 0x311 or 0x300]*/); // Output multiplexer register

D:
	tl880_vpx_write_fp(tl880dev, VPX_FP_PVAL_START, 0);	// Start position of video active signal (chip default is 40)
	tl880_vpx_write_fp(tl880dev, VPX_FP_PVAL_STOP, 0x320);	// 800 - end position of video active (chip default is 720)

	b = gwVPX_VactMode;	// Mode for video active signal (programmable vs. window size)
	b |= 0x200;		// Latch: timing mode update enable
	b <<= 2;
	tl880_vpx_write_fp(tl880dev, VPX_FP_CONTROLWORD, b); // Control and latching register
	tl880_vpx_write_fp(tl880dev, VPX_FP_REG174, 4);	// Undocumented register? (not in specs)

	if(1 /*vpx type is 3226*/) {
		tl880_vpx_write_fp(tl880dev, VPX_FP_PEAKING1, 0x300); // Coring/peaking: swap chroma, peaking uses low frequency
		tl880_vpx_write_fp(tl880dev, VPX_FP_SDT, 0x21);   // Standard selection: NTSC M + 4H Comb filter (pg 50)
		tl880_vpx_write_fp(tl880dev, VPX_FP_COMB_UC, 0xfff);  // Comb filter: max peaked, max diagonal dot reduction,
								  // max horiz diff gain, min vert diff gain, max vertical peak
	}
}

