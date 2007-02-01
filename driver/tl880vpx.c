/* 
 * VPX322x control for TL880-based cards (until vpx3220 driver has needed features)
 *
 * Functions based on vpx3220 driver and VPX3226F datasheet.
 *
 * (c) 2007 Mike Bourgeous <nitrogen at users.sourceforge.net>
 */
#include "tl880.h"


/* TODO: store VPX address in tl880 struct instead of hard-coding in other functions */
int tl880_vpx_read(struct tl880_i2c_bus *i2cbus, unsigned short addr, unsigned char reg)
{
	return tl880_i2c_read_byte_data(i2cbus, addr, reg);
}

int tl880_vpx_write(struct tl880_i2c_bus *i2cbus, unsigned short addr, unsigned char reg, unsigned char value)
{
	/*
	 * Note - the vpx driver stores written data in an array, which will now be out of sync.
	 * So, the sooner all vpx code is moved over there, the better
	 */

	return tl880_i2c_write_byte_data(i2cbus, addr, reg, value);
}

unsigned short tl880_vpx_read_fp(struct tl880_i2c_bus *i2cbus, unsigned short addr, unsigned short fpaddr)
{
	int result;
	unsigned short data;

	/* Write the 16-bit address to the FPRD register */
	if((result = tl880_i2c_write_word_data(i2cbus, addr, 0x26, __cpu_to_be16(fpaddr))) < 0) {
		printk(KERN_WARNING "tl880: Failed to write vpx FP read register: %d\n", result);
		return -1;
	}

	/* TODO: check for FP busy status */
	/*
	if (vpx3220_fp_status(client) < 0) {
		return -1;
	}
	*/

	/* Read the 16-bit data from the FPDAT register */
	data = tl880_i2c_read_word_data(i2cbus, addr, 0x28);
	if(data == (unsigned short)-1) {
		printk(KERN_WARNING "tl880: Failed to read vpx FP data\n");
		return -1;
	}

	return __be16_to_cpu(data);
}

int tl880_vpx_write_fp(struct tl880_i2c_bus *i2cbus, unsigned short addr, unsigned short fpaddr, unsigned short data)
{
	int result;

	/* Write the 16-bit address to the FPWR register */
	if((result = tl880_i2c_write_word_data(i2cbus, addr, 0x27, __cpu_to_be16(fpaddr))) < 0) {
		printk(KERN_WARNING "tl880: Failed to write vpx FP write register: %d\n", result);
		return -1;
	}

	/* TODO: check for FP busy status */
	/*
	if (vpx3220_fp_status(client) < 0) {
		return -1;
	}
	*/

	/* Write the 16-bit data to the FPDAT register */
	if((result = tl880_i2c_write_word_data(i2cbus, addr, 0x28, __cpu_to_be16(data))) < 0) {
		printk(KERN_WARNING "tl880: Failed to write vpx FP data: %d\n", result);
		return -1;
	}

	return 0;
}

#ifdef WILLNOTCOMPILE
void tl880_config_vpx()
{
	// Load default VPX settings
	// LoadDefaultSettings()
	
	// Initialize the VPX chip
	tl880_vpx_init();

	// Set the initial video standard
	tl880_vpx_set_video_standard(7);

	// Set the initial video source
	tl880_vpx_set_video_source(2, 4);

	// Set the video region decoded (?)
	tl880_vpx_set_video_window(1, 22, 240, 240, 14, 720, 734, 3000, 0);
	tl880_vpx_set_video_window(2, 21,   1,   1,  0, 160, 160,    0, 0);
	tl880_vpx_set_video_window(3,  0,   0,   0,  0,   0,   0,    0, 0);

	// Set other video attributes
	tl880_vpx_set_video_attribute(1, 2, 144);
	tl880_vpx_set_video_attribute(1, 1, 112);
	tl880_vpx_set_video_attribute(1, 3, 192);
	tl880_vpx_set_video_attribute(1, 4, 128);
	tl880_vpx_set_video_attribute(1, 5, 128);
	tl880_vpx_set_video_attribute(1, 6, 128);
	tl880_vpx_set_video_attribute(1, 10, 180);
}

// A commentary on the initialization procedure of the VPX chip
void tl880_init_vpx()
{
	// Detect the VPX hardware version
	tl880_vpx_detect_device();
	
	// Detect the VPX firmware version
	short version = 0xcc;
	tl880_vpx_get_firmware_version(&version);

	// Disable power-saving mode
	tl880_vpx_set_power_status(0);

vpx3226:
	if(VPX_short_Llc2pol != 0) { // llc2pol is set to 0 in LoadDefaultSettings
		b = 0x10; // register 0xaa bit[4]: LLC2 polarity
	} else {
		// Funky optimization way of saying if(oldllc) then set bit 6
		b = (-VPX_short_Oldllc) & 0xffff;
		b -= b;
		b &= 0x40; // register 0xaa bit[6]: old llc timing with longer hold time
	}
	i2cwrite8(VPX_byte_Adr, 0xaa, b); // LLC settings

	a = 0;
	a = VPX_short_Llc2en;	// LLC2 to TDO (if using JTAG in test-logic-reset state)
	a <<= 1;
	a |= VPX_short_Zen;	// HREF, VREF, FIELD, VACT, LLC, LLC2 enable
	a <<= 1;
	a |= VPX_short_Clkio;	// Pixclock output enable
	a <<= 1;
	a |= VPX_short_Ben;	// Video port B enable (disable = tristate)
	a <<= 1;
	a |= VPX_short_Aen;	// Video port A enable (disable = tristate)
	i2cwrite8(VPX_byte_Adr, 0xf2, a); // Output enable register

	a = 0;
	a = VPX_short_Strpixclk;	// additional PIXCLK driving strength (if needed different from below)
	a <<= 3;
	a |= VPX_short_Stra2;	// PIXCLK, LLC, VACT driving strength
	a <<= 3;
	a |= VPX_short_Stra1;	// Video port A driving strength
	i2cwrite8(VPX_byte_Adr, 0xf8, a); // Driving strength register

	a = 0;
	a = VPX_short_Strb2;	// HREF, VREF, FIELD, LLC2 driving strength
	a <<= 3;
	a |= VPX_short_Strb1;	// Video port B driving strength
	i2cwrite8(VPX_byte_Adr, 0xf9, a); // Driving strength register

	a = 0;
	a = VPX_short_Vlen;	// VREF pulse width (VPX adds two to this number)
	a <<= 1;
	a |= VPX_short_Vrefpol;	// VREF polarity (0 = active high)
	a <<= 1;
	a |= VPX_short_Hrefpol;	// HREF polarity (0 = active low)
	a <<= 1;
	a |= VPX_short_Prefpol;	// Polarity of field pin (0 = odd high, 1 = even high)
	tl880_vpx_write_fp(0x153, a); // HREF and VREF control 

	switch(VPX_short_Format) { // Set to zero in LoadDefaultSettings and never changed
		case 0:
		default:
			edi = 0;
		case 8:
			edi = 1;
		case 9:
			edi = 2;
	}

	a = 0;
	a = VPX_short_Flagdel;	// Change ITU-R656 header flags in 0: SAV, 1: EAV
	c = 0;
	c = VPX_short_Hsup;	// Suppress ITU-R656 headers for blank lines
	a &= 1;
	a <<= 1;
	c &= 1;
	a |= c;
	c = 0;
	c = VPX_short_UVSwap;	// Specs describe this bit as "reserved"
	a <<= 1;
	c &= 1;
	a |= c;
	c = 0;
	c = VPX_short_SplitDis;	// Disable teletext splitting when enabled
	a <<= 1;
	c &= 1;
	a |= c;
	c = 0;
	c = VPX_short_Shuffler;	// Shuffler (0: Video Port A=Y, Port B=CbCr, 1: vice versa)
	a <<= 4;		// Skipping over VBI data range, transmission mode, pixclk selection
	c &= 1;
	a |= c;
	a <<= 2;
	a |= edi;		// bits 1:0 YCbCr 422 format selection (2: bitstream, 1: ITU-R656, 0: ITU-R601)
	tl880_vpx_write_fp(0x150, a); // Format selection register

	int somevar = 0xcc;
	tl880_vpx_read_fp(0x150, &somevar); // Why?

	b = tl880dev->card_type;
	if(b != 1) {
		goto A;
	}
	arg = 0x311; // 785 (multipurpose bits and double clock active, port B is 00010001)
	goto C;

A:
	if(b != 2) {
		goto B;
	}
	arg = 0x300; // 768 (multipurpose bits and double clock active, port B output is 0)
	goto C;

B:
	if(b < 3) {
		goto D;
	}
	arg = 0x301; // 769 (multipurpose bits and double clock active, port B output is 00000001)

C:
	tl880_vpx_write_fp(0x154, [0x301 or 0x311 or 0x300]); // Output multiplexer register

D:
	tl880_vpx_write_fp(0x151, 0);		// Start position of video active signal (chip default is 40)
	tl880_vpx_write_fp(0x152, 0x320); // 800 - end position of video active (chip default is 720)

	b = VPX_short_VactMode;	// Mode for video active signal (programmable vs. window size)
	b |= 0x200;		// Latch: timing mode update enable
	b <<= 2;
	tl880_vpx_write_fp(0x140, b); // Control and latching register
	tl880_vpx_write_fp(0x174, 4);	// Undocumented register? (not in specs)

	if(vpx type is 3226) {
		tl880_vpx_write_fp(0x126, 0x300); // Coring/peaking: swap chroma, peaking uses low frequency
		tl880_vpx_write_fp(0x20, 0x21);   // Standard selection: NTSC M + 4H Comb filter (pg 50)
		tl880_vpx_write_fp(0x28, 0xfff);  // Comb filter: max peaked, max diagonal dot reduction,
						  // max horiz diff gain, min vert diff gain, max vertical peak
	}
}
#endif /* WILLNOTCOMPILE */

