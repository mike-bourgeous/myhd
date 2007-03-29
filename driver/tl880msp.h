/* 
 * TL880 driver header for MSP-related definitions
 *
 * (c) 2003-2007 Mike Bourgeous <nitrogen at users.sourceforge.net>
 *
 * $Log: tl880msp.h,v $
 * Revision 1.1  2007/03/29 08:38:54  nitrogen
 * Initial MSP configuration support.
 *
 * Revision 1.4  2007/03/28 08:01:30  nitrogen
 * Initialization improvements, MSP improvements, minor comment and error message tweaks, better docs
 *
 * Revision 1.3  2007/03/26 19:25:06  nitrogen
 * Added CVS log generation and updated copyrights and e-mail addresses.
 *
 */

#ifndef _TL880MSP_H_
#define _TL880MSP_H_

#ifdef __KERNEL__

/*** Driver definitions ***/


/*** Driver types ***/


#endif /* __KERNEL__ */

/*** Userspace definitions such as ioctls ***/


#ifdef __KERNEL__

/*** Driver variables ***/

/*** Driver functions ***/

/* tl880msp.c */
void tl880_msp_config(struct tl880_dev *tl880dev);
unsigned short tl880_msp_init(struct tl880_dev *tl880dev, unsigned short *msp_type);
int tl880_msp_write(struct tl880_dev *tl880dev, int dev, int addr, int val);

#endif /* __KERNEL__ */

#endif /* _TL880MSP_H_ */

