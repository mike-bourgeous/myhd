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

/* I2C */
#define I2C_CLIENTS_MAX				16

/* PCI */
#define PCI_VENDOR_ID_TERALOGIC			0x544c
#define PCI_DEVICE_ID_TERALOGIC_880		0x350

#define PCI_SUBSYSTEM_VENDOR_ID_MIT		0x17a1
#define PCI_SUBSYSTEM_DEVICE_ID_MYHD		0x0001
#define PCI_SUBSYSTEM_DEVICE_ID_MYHD_MDP120	0x0120

#define PCI_SUBSYSTEM_VENDOR_ID_HAUPPAUGE	0x0070
#define PCI_SUBSYSTEM_DEVICE_ID_WINTV_HD	0x4600

#define PCI_SUBSYSTEM_VENDOR_ID_TELEMANN	0x1378
#define PCI_SUBSYSTEM_VENDOR_ID_ZERO		0x0000
#define PCI_SUBSYSTEM_DEVICE_ID_HIPIX		0x0000

/* We'll give each new manufacturer a 0x10 bump */
#define TL880_CARD_HIPIX		0x20

#define TL880_CARD_WINTV_HD		0x10

#define TL880_CARD_MYHD_MDP120		5
#define TL880_CARD_MYHD_MDP110		4
#define TL880_CARD_MYHD_MDP100		3
#define TL880_CARD_MYHD_MDP100A		2
#define TL880_CARD_JANUS		1
#define TL880_CARD_ZERO			0


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
	struct tl880_dev *next;			/* Linked list pointer */
	struct pci_dev *pcidev;			/* PCI device structure */
	int id;					/* Card number */
	unsigned short subsys_vendor_id;	/* Card vendor ID */
	unsigned short subsys_device_id;	/* Card device ID */
	unsigned short board_type;		/* Card type identifier */

	/** Card's I/O and memory space **/
	void *memspace;				/* Mapped memory space */
	unsigned long memphys;			/* Physical memory address */
	unsigned long memlen;			/* Memory space length */
	void *regspace;				/* Mapped register space */
	unsigned long regphys;			/* Physical register address */
	unsigned long reglen;			/* Register space length */
	void *unkspace;				/* Mapped unknown space */
	unsigned long unkphys;			/* Physical unknown address */
	unsigned long unklen;			/* Unknown space length */

	/** DMA buffer **/
	void *dmavirt;				/* System-side DMA buffer pointer */
	dma_addr_t dmaphys;			/* Bus-side DMA buffer pointer */

	/** I2C stuff **/
	int minbus;				/* Lower bus-type ID for this card */
	int maxbus;				/* Upper bus-type ID for this card */
	struct tl880_i2c_bus *i2cbuses;		/* Array of I2C buses on this card */

	/** Interrupt stuff **/
	struct tq_struct bh;			/* Device-specific info for interrupt bottom half */
	unsigned int irq;			/* Interrupt line */

	unsigned long int_mask;			/* Global interrupt enable mask */
	unsigned long int_type;			/* Type of interrupt to service */
	unsigned long int_count;		/* Number of interrupts received */

	unsigned long vsc_mask;			/* VSC interrupt mask */
	unsigned long vsc_type;			/* Type of VSC interrupt received */
	unsigned long vsc_count;		/* Number of VSC interrupts */
	
	unsigned long apu_mask;			/* APU interrupt mask */
	unsigned long apu_type;			/* Type of APU interrupt received */
	unsigned long apu_count;		/* Number of APU interrupts */

	unsigned long blt_mask;			/* BLT interrupt mask */
	unsigned long blt_type;			/* Type of BLT interrupt received */
	unsigned long blt_count;		/* Number of BLT interrupts */

	unsigned long mce_mask;			/* MCE interrupt mask */
	unsigned long mce_type;			/* Type of MCE interrupt received */
	unsigned long mce_count;		/* Number of MCE interrupts */

	unsigned long vpip_mask;		/* VPIP interrupt mask */
	unsigned long vpip_type;		/* Type of VPIP interrupt received */
	unsigned long vpip_count;		/* Number of VPIP interrupts */

	unsigned long hpip_mask;		/* HPIP interrupt mask */
	unsigned long hpip_type;		/* Type of HPIP interrupt received */
	unsigned long hpip_count;		/* Number of HPIP interrupts */

	unsigned long dpc_mask;			/* DPC interrupt mask */
	unsigned long dpc_type;			/* Type of DPC interrupt received */
	unsigned long dpc_count;		/* Number of DPC interrupts */

	unsigned long tsd_mask;			/* TSD interrupt mask */
	unsigned long tsd_type;			/* Type of TSD interrupt received */
	unsigned long tsd_count;		/* Number of TSD interrupts */

	/** Devfs stuff **/
	devfs_handle_t devfs_device;		/* Devfs information */
};

#endif /* __KERNEL__ */

/*** Userspace definitions such as ioctls ***/

#ifdef __KERNEL__

/*** Driver variables ***/

/*** Driver functions ***/
/* tl880util.c */
void set_bits(unsigned long *value, long reg, long high_bit, long low_bit, unsigned long setvalue);

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

/* tl880demux.c */
unsigned long tl880_demux_init(struct tl880_dev *tl880dev);

/* tl880dma.c */
unsigned int tl880_aux_dma_allocate();
void tl880_aux_dma_free(unsigned int dma);
extern unsigned long dma_bitmask;

#endif /* __KERNEL__ */


/*** Driver local includes ***/

#include "tl880reg.h"
#include "tl880dpc.h"
#include "tl880osd.h"

#endif /* _TL880_H_ */

