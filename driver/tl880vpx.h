/* 
 * TL880 driver header for VPX-related definitions
 *
 * (c) 2003-2007 Mike Bourgeous <nitrogen at users.sourceforge.net>
 *
 * $Log: tl880vpx.h,v $
 * Revision 1.4  2007/03/28 08:01:30  nitrogen
 * Initialization improvements, VPX improvements, minor comment and error message tweaks, better docs
 *
 * Revision 1.3  2007/03/26 19:25:06  nitrogen
 * Added CVS log generation and updated copyrights and e-mail addresses.
 *
 */

#ifndef _TL880VPX_H_
#define _TL880VPX_H_

#ifdef __KERNEL__

/*** Driver definitions ***/

/* VPX constants */
#define VPX_TIMEOUT_COUNT 	10	// From the DVB VPX driver

/* VPX I2C registers */
#define VPX_I2C_FPRD_OLD	0x26	// FP RAM read selection register (VPX3220 and older)
#define VPX_I2C_FPWR_OLD	0x27	// FP RAM write selection register (VPX3220 and older)
#define VPX_I2C_FPDAT_OLD	0x28	// FP RAM data read/write register (VPX3220 and older)
#define VPX_I2C_FPSTA_OLD	0x29	// FP RAM status register (VPX3220 and older)
#define VPX_I2C_FPSTA		0x35	// FP RAM status register
#define VPX_I2C_FPRD		0x36	// FP RAM read selection register
#define VPX_I2C_FPWR		0x37	// FP RAM write selection register
#define VPX_I2C_FPDAT		0x38	// FP RAM data read/write register
#define VPX_I2C_LLC		0xaa	// Low power mode, LLC settings register
#define VPX_I2C_OENA		0xf2	// Output enable register
#define VPX_I2C_DRIVER_A	0xf8	// TTL type A pads driving strength
#define VPX_I2C_DRIVER_B	0xf9	// TTL type B pads driving strength

/* VPX FP registers */
#define VPX_FP_SDT		0x20	// Standard selection register
#define VPX_FP_INSEL		0x21	// Input selection register
#define VPX_FP_COMB_UC		0x28	// Comb filter control register
#define VPX_FP_ACC_SAT		0x30	// Saturation (ACC reference level on VPX322xD)
#define VPX_FP_ACCB		0x32	// SECAM Cb component adjustment (VPX322xD)
#define VPX_FP_ACCR		0x33	// SECAM Cr component adjustment (VPX322xD)
#define VPX_FP_TINT		0xDC	// NTSC tint angle (+/- 512 == +/- pi/4)
#define VPX_FP_PEAKING1		0x126	// Peaking/coring selection 1
#define VPX_FP_BRIGHTNESS1	0x127	// Brightness 1
#define VPX_FP_CONTRAST1	0x128	// Contrast 1
#define VPX_FP_PEAKING2		0x130	// Peaking/coring selection 2
#define VPX_FP_BRIGHTNESS2	0x131	// Brightness 2
#define VPX_FP_CONTRAST2	0x132	// Contrast 2
#define VPX_FP_CONTROLWORD	0x140	// Control and latching register
#define VPX_FP_FORMAT_SEL	0x150	// Format selection / shifter / PIXCLK settings
#define VPX_FP_PVAL_START	0x151	// Video active start position
#define VPX_FP_PVAL_STOP	0x152	// Video active end position
#define VPX_FP_REFSIG		0x153	// HREF, VREF, FIELD signal length and polarity
#define VPX_FP_OUTMUX		0x154	// Output multiplexer / multipurpose output control
#define VPX_FP_REG174		0x174	// Unknown/undocumented (Macrovision?)


/*** Driver types ***/


#endif /* __KERNEL__ */

/*** Userspace definitions such as ioctls ***/


#ifdef __KERNEL__

/*** Driver variables ***/

/*** Driver functions ***/

/* tl880vpx.c */
int tl880_vpx_read(struct tl880_dev *tl880dev, unsigned char reg);
int tl880_vpx_write(struct tl880_dev *tl880dev, unsigned char reg, unsigned char value);
unsigned short tl880_vpx_read_fp(struct tl880_dev *tl880dev, unsigned short fpaddr);
int tl880_vpx_write_fp(struct tl880_dev *tl880dev, unsigned short fpaddr, unsigned short data);

int tl880_vpx_set_power_register(struct tl880_dev *tl880dev, int status);
int tl880_vpx_set_power_status(struct tl880_dev *tl880dev, int status);
int tl880_vpx_set_video_standard(struct tl880_dev *tl880dev, enum video_standard_e standard);
int tl880_vpx_set_video_source(struct tl880_dev *tl880dev, int luma_source, char chroma_source);

void tl880_vpx_config(struct tl880_dev *tl880dev);
void tl880_vpx_init(struct tl880_dev *tl880dev);


#endif /* __KERNEL__ */

#endif /* _TL880VPX_H_ */

