/* 
 * Driver for TL880-based cards (possibly also TL850)
 *
 * (c) 2003-2007 Mike Bourgeous <nitrogen at users.sourceforge.net>
 * (c) 2007 Jason P. Matthews
 *
 * $Log: tl880.h,v $
 * Revision 1.25  2007/03/26 19:23:56  nitrogen
 * Added GPIO patch by Jason P. Matthews.
 *
 */

#ifndef _TL880_H_
#define _TL880_H_

#ifdef __KERNEL__

/* Standard includes */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/dma-mapping.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/cdev.h>
#include <linux/major.h>
#include <linux/slab.h>
#include <linux/mm.h>
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
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/videodev.h>
#include <linux/i2c.h>
#include <linux/i2c-algo-bit.h>
#include <media/tuner.h>
#include <media/v4l2-common.h>
#include <media/tvaudio.h>
#include <../drivers/media/dvb/frontends/nxt200x.h>

/*** Driver definitions ***/

/* Compatibility with pre-2.6.19 kernels */
#ifndef IRQF_SHARED
#define PRE_2619
#define IRQF_SHARED SA_SHIRQ
#endif /* IRQF_SHARED */
#ifndef IRQF_DISABLED
#define IRQF_DISABLED SA_INTERRUPT
#endif /* IRQF_DISABLED */


/* Devices */
#define DEV_COUNT				64
#define DEV_MASK				0xfffc	/* Bits of minor number identifying specific TL880 card */
#define DEV_SHIFT				2	/* Bits to right-shift minor number to get card identifier */
#define FUNC_MASK				3	/* Card function identifying bits of minor number (i.e. mem/reg/unk) */

/* I2C */
#define I2C_CLIENTS_MAX				16
#define I2C_HW_B_TL880				0x1000dd

/* PCI */
#define PCI_VENDOR_ID_TERALOGIC			0x544c
#define PCI_DEVICE_ID_TERALOGIC_TL880		0x0350

#define PCI_SUBSYSTEM_VENDOR_ID_MIT		0x17a1
#define PCI_SUBSYSTEM_DEVICE_ID_MYHD		0x0001
#define PCI_SUBSYSTEM_DEVICE_ID_MYHD_MDP120	0x0120
#define PCI_SUBSYSTEM_DEVICE_ID_MYHD_MDP130	0x0130

#define PCI_SUBSYSTEM_VENDOR_ID_HAUPPAUGE	0x0070
#define PCI_SUBSYSTEM_DEVICE_ID_WINTV_HD	0x4600

#define PCI_SUBSYSTEM_VENDOR_ID_TELEMANN	0x1378
#define PCI_SUBSYSTEM_VENDOR_ID_ZERO		0x0000
#define PCI_SUBSYSTEM_DEVICE_ID_HIPIX		0x0000

/* We'll give each new manufacturer a 0x10 bump */
#define TL880_CARD_HIPIX		0x20

#define TL880_CARD_WINTV_HD		0x10

#define TL880_CARD_MYHD_MDP130		6
#define TL880_CARD_MYHD_MDP120		5
#define TL880_CARD_MYHD_MDP110		4
#define TL880_CARD_MYHD_MDP100		3
#define TL880_CARD_MYHD_MDP100A		2
#define TL880_CARD_JANUS		1
#define TL880_CARD_ZERO			0

/* Debug */
#define CHECK_NULL(a) ( (a) ? (0) : (printk(KERN_ERR "tl880: NULL %s in %s at %u\n", #a, __FUNCTION__, __LINE__), (1)) )
#define CHECK_NULL_W(a) ( (a) ? (0) : (printk(KERN_WARNING "tl880: NULL %s in %s at %u\n", #a, __FUNCTION__, __LINE__), (1)) )


/*** Driver types ***/
struct tl880_dev;

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
	unsigned int id;			/* Card number */
	unsigned short subsys_vendor_id;	/* Card vendor ID */
	unsigned short subsys_device_id;	/* Card device ID */
	unsigned short card_type;		/* Card type identifier */
	char name[64];				/* Card model name */
	struct semaphore *sem;			/* For reentry protection */

	/* Character device stuff */
	struct cdev *char_device;		/* Kernel character device handle */
	unsigned int major;			/* Major number for all tl880 devices (for convenience) */
	unsigned int minor;			/* First minor number for this card */


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
	struct tasklet_struct tasklet;		/* Tasklet for interrupt work */

	unsigned char irq;			/* Interrupt line */

	int elseint;				/* Do we share an interrupt? */

	unsigned long old_mask;			/* Previously used interrupt mask */
	unsigned long int_mask;			/* Global interrupt enable mask */
	unsigned long int_type;			/* Type of interrupt to service */
	unsigned long int_count;		/* Number of interrupts received */

	unsigned long vsc_mask;			/* VSC interrupt mask */
	unsigned long vsc_type;			/* Type of VSC interrupt received */
	unsigned long vsc_count;		/* Number of VSC interrupts received */
	
	unsigned long apu_mask;			/* APU interrupt mask */
	unsigned long apu_type;			/* Type of APU interrupt received */
	unsigned long apu_count;		/* Number of APU interrupts received */

	unsigned long blt_mask;			/* BLT interrupt mask */
	unsigned long blt_type;			/* Type of BLT interrupt received */
	unsigned long blt_count;		/* Number of BLT interrupts received */

	unsigned long mce_mask;			/* MCE interrupt mask */
	unsigned long mce_type;			/* Type of MCE interrupt received */
	unsigned long mce_count;		/* Number of MCE interrupts received */

	unsigned long mcu_mask;			/* MCU interrupt mask */
	unsigned long mcu_type;			/* Type of MCU interrupt received */
	unsigned long mcu_count;		/* Number of MCU interrupts received */

	unsigned long vpip_mask;		/* VPIP interrupt mask */
	unsigned long vpip_type;		/* Type of VPIP interrupt received */
	unsigned long vpip_count;		/* Number of VPIP interrupts received */

	unsigned long hpip_mask;		/* HPIP interrupt mask */
	unsigned long hpip_type;		/* Type of HPIP interrupt received */
	unsigned long hpip_count;		/* Number of HPIP interrupts received */

	unsigned long dpc_mask;			/* DPC interrupt mask */
	unsigned long dpc_type;			/* Type of DPC interrupt received */
	unsigned long dpc_count;		/* Number of DPC interrupts received */

	unsigned long tsd_mask;			/* TSD interrupt mask */
	unsigned long tsd_type;			/* Type of TSD interrupt received */
	unsigned long tsd_count;		/* Number of TSD interrupts received */


	/** Audio (APU) State **/
	enum audio_mode_e { 
		ZERO = 0, ONE, TWO 
	} audio_mode;				/* Current audio mode */

	unsigned long iau_base;			/* Base card memory address for audio unit */
	unsigned long iau_iba_reg;		/* Register used for base address */
	unsigned long iau_iea_reg;		/* Register used for end address */
	unsigned long iau_ira_reg;		/* Register used for base address (2) */

	
	/*** Video State ***/


	/*** Video Chip (VPX) State ***/
	int vpx_addr;				/* Set to 0 if no VPX */
	int vpx_i2cbus;				/* Set to index of i2cbuses */
	enum video_standard_e {
		PAL_B = 0,
		PAL_G = 0,
		PAL_H = 0,
		PAL_I = 0,
		NTSC_M = 1,
		SECAM = 2,
		NTSC44 = 3,
		PAL_M = 4,
		PAL_N = 5,
		PAL_60 = 6,
		NTSC_COMB = 7,
		NTSC_COMP = 9,
		FIFTEEN = 15
	} vpx_video_standard;			/* Currently active video standard */
};

#endif /* __KERNEL__ */

/*** Userspace definitions such as ioctls ***/
/* 
 * Read register IOCTL - register is read from parameter, then the value read
 * is written back to the parameter.  Single unsigned long.
 */
#define TL880IOCREADREG		_IOWR(0xdd, 0, unsigned int *)

/*
 * Write register IOCTL - register is read from parameter[0], the value to
 * write is read from parameter[1].  Array [2] unsigned int.
 */
#define TL880IOCWRITEREG	_IOW(0xdd, 1, unsigned int *)

/*
 * VIP state IOCTL - parameter is a pointer to VIP state int (0 off, 1 on, 2
 * special?)
 */
#define TL880IOCSETVIP		_IOW(0xdd, 2, unsigned int *)

/*
 * Set cursor position IOCTL - cursor position is packed into unsigned int
 * as two shorts - i.e. (x & 0xffff) | (y << 16)
 */
#define TL880IOCSETCURSORPOS	_IOW(0xdd, 3, unsigned int *)


#ifdef __KERNEL__

/*** Driver variables ***/
extern unsigned int debug;
extern dev_t device_number;
extern int n_tl880s;
extern struct tl880_dev *tl880_list;

/*** Driver functions ***/
/* tl880util.c */
void set_bits(unsigned long *value, long reg, long high_bit, long low_bit, unsigned long setvalue);
struct tl880_dev *find_tl880(unsigned long tl880_id);
struct tl880_dev *find_tl880_pci(struct pci_dev *dev);

/* tl880i2c.c */
int tl880_init_i2c(struct tl880_dev *tl880dev);
void tl880_deinit_i2c(struct tl880_dev *tl880dev);
int tl880_call_i2c_clients(struct tl880_dev *tl880dev, unsigned int cmd, void *arg);
/* I2C access functions for features not handled by external chip drivers */
int tl880_i2c_read_byte(struct tl880_i2c_bus *i2cbus, unsigned short addr);
int tl880_i2c_write_byte(struct tl880_i2c_bus *i2cbus, unsigned short addr, unsigned char value);
int tl880_i2c_read_byte_data(struct tl880_i2c_bus *i2cbus, unsigned short addr, unsigned char command);
int tl880_i2c_write_byte_data(struct tl880_i2c_bus *i2cbus, unsigned short addr, unsigned char command, unsigned char value);
int tl880_i2c_read_word_data(struct tl880_i2c_bus *i2cbus, unsigned short addr, unsigned char command);
int tl880_i2c_write_word_data(struct tl880_i2c_bus *i2cbus, unsigned short addr, unsigned char command, unsigned short value);

/* tl880init.c */
void tl880_init_myhd(struct tl880_dev *tl880dev);
void tl880_init_wintv_hd(struct tl880_dev *tl880dev);
void tl880_init_chip(struct tl880_dev *tl880dev);
void tl880_init_dev(struct tl880_dev *tl880dev);

/* tl880gpio.c */
unsigned char tl880_set_gpio(struct tl880_dev *tl880dev, unsigned int gpio_line, unsigned char state);
void tl880_write_gpio1_wintv_hd(struct tl880_dev *tl880dev, unsigned char state, unsigned char b, int c);

/* tl880int.c */
#ifdef PRE_2619 /* Pre-2.6.19 compatibility */
#warning Using old-style interrupt routine
irqreturn_t tl880_irq(int irq, void *dev_id, struct pt_regs *regs);
#else /* PRE_2619 */
irqreturn_t tl880_irq(int irq, void *dev_id);
#endif /* PRE_2619 */
void tl880_bh(unsigned long tl880_id);
void tl880_disable_interrupts(struct tl880_dev *tl880dev);

/* tl880vip.c */
void tl880_set_vip(struct tl880_dev *tl880dev, unsigned long vip_mode);

/* tl880input.c */
void tl880_set_video_source(struct tl880_dev *tl880dev, unsigned char a, unsigned char b);
void tl880_set_ntsc_input(struct tl880_dev *tl880dev, int input);

/* tl880audio.c */
void tl880_disable_apu(struct tl880_dev *tl880dev);
void tl880_apu_init_iau_reg(struct tl880_dev *tl880dev);
void tl880_apu_init_ioc(struct tl880_dev *tl880dev);
void tl880_apu_start_ioc(struct tl880_dev *tl880dev);
void tl880_apu_stop_ioc(struct tl880_dev *tl880dev);

void tl880_init_hardware_audio(struct tl880_dev *tl880dev, enum audio_mode_e audio_mode);

void tl880_init_ntsc_audio(struct tl880_dev *tl880dev);
void tl880_ntsc_audio_dpc(struct tl880_dev *tl880dev);
void tl880_set_ntsc_audio_clock(struct tl880_dev *tl880dev);

/* tl880demux.c */
unsigned long tl880_demux_init(struct tl880_dev *tl880dev);

/* tl880dma.c */
unsigned int tl880_aux_dma_allocate(void);
void tl880_aux_dma_free(unsigned int dma);
extern unsigned long dma_bitmask;

int tl880_dma_vop_on(struct tl880_dev *tl880dev);

/* tl880proc.c */
int tl880_create_proc_entry(void);
void tl880_remove_proc_entry(void);

/* tl880mem.c */
unsigned long tl880_read_memory(struct tl880_dev *tl880dev, unsigned long mem);
void tl880_write_memory(struct tl880_dev *tl880dev, unsigned long mem, unsigned long value);
unsigned long tl880_read_membits(struct tl880_dev *tl880dev, unsigned long mem, long high_bit, long low_bit);
void tl880_write_membits(struct tl880_dev *tl880dev, unsigned long mem, long high_bit, long low_bit, unsigned long value);
void tl880_clear_sdram(struct tl880_dev *tl880dev, unsigned long start_addr, unsigned long end_addr, unsigned long value);

#endif /* __KERNEL__ */


/*** Driver local includes ***/

#include "tl880reg.h"
#include "tl880dpc.h"
#include "tl880osd.h"
#include "tl880vpx.h"

#endif /* _TL880_H_ */

