/* 
 * TL880 driver header for register-related functions
 *
 * (c) 2003 Mike Bourgeous <nitrogen@slimetech.com>
 */

#ifndef _TL880_REG_H_
#define _TL880_REG_H_

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

/* 
 * Read register IOCTL - register is read from parameter, then the value read
 * is written back to the parameter.  Single unsigned long.
 */
#define TL880IOCREADREG		_IOWR(0xdd, 0, unsigned long *)

/*
 * Write register IOCTL - register is read from parameter[0], the value to
 * write is read from parameter[1].  Array [2] unsigned long.
 */
#define TL880IOCWRITEREG	_IOW(0xdd, 1, unsigned long *)

#ifdef __KERNEL__

/*** Driver variables ***/

/*** Driver functions ***/

/* tl880reg.c */
unsigned long read_register(struct tl880_dev *tl880dev, unsigned long reg);
void write_register(struct tl880_dev *tl880dev, unsigned long reg, unsigned long value);
unsigned long read_regbits(struct tl880_dev *tl880dev, unsigned long reg, long high_bit, long low_bit);
void write_regbits(struct tl880_dev *tl880dev, unsigned long reg, long high_bit, long low_bit, unsigned long value);

#endif /* __KERNEL__ */


#endif /* _TL880_REG_H_ */

