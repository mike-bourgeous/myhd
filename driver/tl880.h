/* 
 * Driver for TL880-based cards (possibly also TL850)
 *
 * (c) 2003 Mike Bourgeous <nitrogen@slimetech.com>
 */

#ifndef _TL880_H_
#define _TL880_H_

#ifdef __KERNEL__

/* Standard includes */
#include <linux/module.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/poll.h>
#include <linux/pci.h>
#include <linux/signal.h>
#include <asm/io.h>
#include <linux/ioport.h>
#include <asm/pgtable.h>
#include <asm/page.h>
#include <linux/sched.h>
#include <asm/types.h>
#include <linux/types.h>
#include <linux/wrapper.h>
#include <linux/interrupt.h>
#include <linux/tqueue.h>
#include <asm/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/videodev.h>
#include <linux/i2c-algo-bit.h>
#include "audiochip.h"
#include "msp3400.h"
#include "tuner.h"

/*** Driver definitions ***/
#define I2C_CLIENTS_MAX				16

#define PCI_VENDOR_ID_TERALOGIC			0x544c
#define PCI_DEVICE_ID_TERALOGIC_880		0x350

#define PCI_SUBSYSTEM_VENDOR_ID_MIT		0x17a1
#define PCI_SUBSYSTEM_DEVICE_ID_MYHD		0x0001
#define PCI_SUBSYSTEM_DEVICE_ID_MYHD_MDP120	0x0120

#define PCI_SUBSYSTEM_VENDOR_ID_HAUPPAUGE	0x0070
#define PCI_SUBSYSTEM_DEVICE_ID_WINTV_HD	0x4600

#define PCI_SUBSYSTEM_VENDOR_ID_TELEMANN	0x1378
#define PCI_SUBSYSTEM_DEVICE_ID_HIPIX		0xBEEF

/* We'll give each new manufacturer a 0x10 bump */
#define TL880_CARD_HIPIX		0x20

#define TL880_CARD_WINTV_HD		0x10

#define TL880_CARD_MYHD_MDP120		5
#define TL880_CARD_MYHD_MDP110		4
#define TL880_CARD_MYHD_MDP100		3
#define TL880_CARD_MYHD_MDP100A		2
#define TL880_CARD_JANUS		1
#define TL880_CARD_ZERO			0

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
struct tl880_dev;
struct tl880_i2c_bus;

struct tl880_i2c_bus {
	int busid;	/* bus access type */
	int busno;	/* card bus number */
	struct i2c_adapter		i2c_adap;
	struct i2c_algo_bit_data	i2c_algo;
	struct tl880_dev		*dev;
	struct i2c_client		*i2c_clients[I2C_CLIENTS_MAX];
};

struct tl880_dev {
	/** Housekeeping stuff **/
	struct tl880_dev *next;	/* Linked list pointer */
	int id;			/* Card number */
	unsigned short subsys_vendor_id;
	unsigned short subsys_device_id;
	unsigned short board_type;

	/** Card's I/O and memory space **/
	void *memspace;
	unsigned long memphys;
	unsigned long memlen;
	void *regspace;
	unsigned long regphys;
	unsigned long reglen;
	void *unkspace;
	unsigned long unkphys;
	unsigned long unklen;

	/** I2C stuff **/
	int minbus;
	int maxbus;
	struct tl880_i2c_bus *i2cbuses;

	/** Interrupt stuff **/
	struct tq_struct bh;	/* Device-specific info for interrupt bottom half */
	int irq;		/* Interrupt line */
	unsigned long int_type;	/* Type of interrupt to service */
	unsigned long tsd_data; /* Extra data for tsd interrupt */
	unsigned long dpc_data; /* Extra data for dpc interrupt */
	unsigned long dpc_mask;

	/** Devfs stuff **/
	devfs_handle_t devfs_device;
};

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

/*
 * VIP state IOCTL - parameter is a pointer to VIP state int (0 off, 1 on, 2
 * special?)
 */
#define TL880IOCSETVIP		_IOW(0xdd, 2, unsigned long *)

#ifdef __KERNEL__

/*** Driver variables ***/

/*** Driver functions ***/
/* tl880util.c */
void set_bits(unsigned long *value, long reg, long high_bit, long low_bit, unsigned long setvalue);

/* tl880reg.c */
unsigned long read_register(struct tl880_dev *tl880dev, unsigned long reg);
void write_register(struct tl880_dev *tl880dev, unsigned long reg, unsigned long value);
unsigned long read_regbits(struct tl880_dev *tl880dev, unsigned long reg, long high_bit, long low_bit);
void write_regbits(struct tl880_dev *tl880dev, unsigned long reg, long high_bit, long low_bit, unsigned long value);

/* tl880i2c.c */
int tl880_init_i2c(struct tl880_dev *tl880dev);
void tl880_deinit_i2c(struct tl880_dev *tl880dev);
void tl880_call_i2c_clients(struct tl880_dev *tl880dev, unsigned int cmd, void *arg);

/* tl880ni2c.c */
int do_i2c_read_byte(struct tl880_i2c_bus *i2cbus, unsigned char *data);
int do_i2c_read(struct tl880_i2c_bus *i2cbus, unsigned char addr, int bregsel, unsigned char reg, unsigned char *data, unsigned int count);
int do_i2c_write_byte_and_wait(struct tl880_i2c_bus *i2cbus, unsigned char data);
int do_i2c_write_byte(struct tl880_i2c_bus *i2cbus, unsigned char data);
int do_i2c_write_bit(struct tl880_i2c_bus *i2cbus, int bit);
void do_i2c_stop_bit(struct tl880_i2c_bus *i2cbus);
void do_i2c_start_bit(struct tl880_i2c_bus *i2cbus);
int do_i2c_write(struct tl880_i2c_bus *i2cbus, unsigned char addr, int bregsel, unsigned char reg, unsigned char *data, unsigned int count);
int i2c_read16(struct tl880_i2c_bus *i2cbus, unsigned char addr, int reg, unsigned short *data);
int i2c_read8(struct tl880_i2c_bus *i2cbus, unsigned char addr, int reg, unsigned char *data);
int i2c_write16(struct tl880_i2c_bus *i2cbus, unsigned char addr, int reg, unsigned short data);
int i2c_write8(struct tl880_i2c_bus *i2cbus, unsigned char addr, int reg, unsigned char data);
int i2c_write(struct tl880_i2c_bus *i2cbus, unsigned char addr, int bregsel, unsigned char reg, unsigned char *data, unsigned int count);
int i2c_read(struct tl880_i2c_bus *i2cbus, unsigned char addr, int bregsel, unsigned char reg, unsigned char *data, unsigned int count);

/* tl880init.c */
void tl880_init_myhd(struct tl880_dev *tl880dev);
void tl880_init_wintv_hd(struct tl880_dev *tl880dev);
void tl880_init_chip(struct tl880_dev *tl880dev);
void tl880_init_dev(struct tl880_dev *tl880dev);

/* tl880gpio.c */
unsigned char tl880_set_gpio(struct tl880_dev *tl880dev, unsigned int gpio_line, unsigned char state);
void tl880_write_gpio1_wintv_hd(struct tl880_dev *tl880dev, unsigned char state, unsigned char b, int c);

/* tl880int.c */
void __init tl880_irq(int irq, void *dev_id, struct pt_regs *regs);
void __init tl880_bh(void *dev_id);
void tl880_disable_interrupts(struct tl880_dev *tl880dev);

/* tl880vip.c */
void tl880_set_vip(struct tl880_dev *tl880dev, unsigned long vip_mode);

/* tl880input.c */
void tl880_set_video_source(struct tl880_dev *tl880dev, unsigned char a, unsigned char b);
void tl880_set_ntsc_input(struct tl880_dev *tl880dev, int input);

/* tl880audio.c */
void tl880_disable_apu(struct tl880_dev *tl880dev);
void tl880_ntsc_audio_dpc(struct tl880_dev *tl880dev);
void tl880_set_ntsc_audio_clock(struct tl880_dev *tl880dev);
void tl880_init_ntsc_audio(struct tl880_dev *tl880dev);

#endif /* __KERNEL__ */

#endif /* _TL880_H_ */
