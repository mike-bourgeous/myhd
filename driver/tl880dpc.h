/* 
 * TL880 driver header for DPC-related definitions
 *
 * (c) 2003 Mike Bourgeous <nitrogen@slimetech.com>
 */

#ifndef _TL880_DPC_H_
#define _TL880_DPC_H_

#ifdef __KERNEL__

/*** Driver definitions ***/

/*** Driver types ***/
struct tl880_mode_def {
	/* 0x10014 */
	unsigned char a:1;
	unsigned char b:1;
	unsigned char c:1;
	unsigned char prog_scan:1;	/* prog_scan */
	unsigned short e:10;
	unsigned short xres:11;		/* xres */
	unsigned char g:1;
	unsigned char h:1;
	unsigned char i:1;
	unsigned char j:1;

	/* 0x10018 */
	unsigned short h_backporch:9;	/* h_backporch */
	unsigned char inv_hsync:1;	/* inv_hsync */
	unsigned char inv_vsync:1;	/* inv_vsync */
	unsigned char h_synclen:7;	/* h_synclen */
	unsigned short h_frontporch:9;	/* h_frontporch */

	/* 0x1001c - first field */
	unsigned char v_synclen_0:3;	/* v_synclen_0 */
	unsigned char v_frontporch_0:8;	/* v_frontporch_0 */
	unsigned short yres_0:11;	/* yres_0 */
	unsigned char v_backporch_0:4;	/* v_backporch_0 */
	unsigned char ntsc_flag:1;	/* ntsc_flag */

	/* 0x10020 - second field */
	unsigned char v_synclen_1:3;	/* v_synclen_1 */
	unsigned char v_frontporch_1:8;	/* v_frontporch_1 */
	unsigned short yres_1:11;	/* yres_1 */
	unsigned char v_backporch_1:4;	/* v_backporch_1 */

	/* 0x10024 */
	unsigned long y:22;
	unsigned char z:1;
	unsigned char aa:4;

	/* 0x10028 */
	unsigned short bb:12;
	unsigned char cc:1;

	/* 0x5800 */
	unsigned long dd;

};


#endif /* __KERNEL__ */


/*** Userspace ioctls ***/

/*
 * VIP state IOCTL - parameter is a pointer to VIP state int (0 off, 1 on, 2
 * special?)
 */
#define TL880IOCSETVIP		_IOW(0xdd, 2, unsigned long *)


#ifdef __KERNEL__

/*** Driver variables ***/

/*** Driver functions ***/

/* tl880dpc.c */
unsigned long tl880_calc_dpc_pll_const(unsigned long a, unsigned char b, unsigned char c);
void tl880_set_dpc_pll_const(struct tl880_dev *tl880dev, unsigned long a, unsigned char b, unsigned char c);
void tl880_set_dpc_clock(struct tl880_dev *tl880dev, unsigned long xres, unsigned long yres, long interlace);
void tl880_init_dpc_pll(struct tl880_dev *tl880dev);
int tl880_dpc_int(struct tl880_dev *tl880dev);

/* tl880modes.c */

#endif /* __KERNEL__ */

#endif /* _TL880_DPC_H_ */

