/* 
 * TL880 driver header for register-related functions
 *
 * (c) 2003-2007 Mike Bourgeous <nitrogen at users.sourceforge.net>
 *
 * $Log: tl880reg.h,v $
 * Revision 1.6  2007/04/24 06:32:14  nitrogen
 * Changed most int/long types to explicit 32-bit sizes.  Fixed compilation and execution on 64-bit CPUs.
 *
 * Revision 1.5  2007/03/26 19:25:06  nitrogen
 * Added CVS log generation and updated copyrights and e-mail addresses.
 *
 */

#ifndef _TL880REG_H_
#define _TL880REG_H_

#ifdef __KERNEL__

/* Known register ranges on the TL880 chip */
#define HIF_REG_MIN		0x0
#define HIF_REG_MAX		0x20
#define HIF_REG_CNT		(HIF_REG_MAX - HIF_REG_MIN)

#define VSC_REG_MIN		0x1000
#define VSC_REG_MAX		0x1030
#define VSC_REG_CNT		(VSC_REG_MAX - VSC_REG_MIN)

#define APU_REG_MIN		0x3000
#define APU_REG_MAX		0x3080
#define APU_REG_CNT		(APU_REG_MAX - APU_REG_MIN)

#define BLT_REG_MIN		0x4000
#define BLT_REG_MAX		0x4060
#define BLT_REG_CNT		(BLT_REG_MAX - BLT_REG_MIN)

#define MCE_REG_MIN		0x6000
#define MCE_REG_MAX		0x6040
#define MCE_REG_CNT		(MCE_REG_MAX - MCE_REG_MIN)

#define VPIP_REG_MIN		0x7000
#define VPIP_REG_MAX		0x7030
#define VPIP_REG_CNT		(VPIP_REG_MAX - VPIP_REG_MIN)

#define HPIP_REG_MIN		0x4000
#define HPIP_REG_MAX		0x4060
#define HPIP_REG_CNT		(HPIP_REG_MAX - HPIP_REG_MIN)

#define DPC_REG_MIN		0x10000
#define DPC_REG_MAX		0x10200
#define DPC_REG_CNT		(DPC_REG_MAX - DPC_REG_MIN)

#define MIF_REG_MIN		0x28000
#define MIF_REG_MAX		0x280e0
#define MIF_REG_CNT		(MIF_REG_MAX - MIF_REG_MIN)


/*** Driver types ***/
#endif /* __KERNEL__ */

/*** Userspace definitions such as ioctls ***/


#ifdef __KERNEL__

/*** Driver variables ***/

/*** Driver functions ***/

/* tl880reg.c */
u32 tl880_read_register(struct tl880_dev *tl880dev, u32 reg);
void tl880_write_register(struct tl880_dev *tl880dev, u32 reg, u32 value);
u32 tl880_read_regbits(struct tl880_dev *tl880dev, u32 reg, int high_bit, int low_bit);
void tl880_write_regbits(struct tl880_dev *tl880dev, u32 reg, int high_bit, int low_bit, u32 value);

#endif /* __KERNEL__ */


#endif /* _TL880REG_H_ */

