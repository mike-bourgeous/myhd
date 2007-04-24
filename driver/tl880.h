/* 
 * Driver for TL880-based cards (possibly also TL850)
 *
 * (c) 2003-2007 Mike Bourgeous <nitrogen at users.sourceforge.net>
 * (c) 2007 Jason P. Matthews
 *
 * $Log: tl880.h,v $
 * Revision 1.29  2007/04/24 06:32:13  nitrogen
 * Changed most int/long types to explicit 32-bit sizes.  Fixed compilation and execution on 64-bit CPUs.
 *
 * Revision 1.28  2007/03/29 09:27:39  nitrogen
 * Tweaked mkdev scripts, improved MSP init, new tool, improved tools makefile, more docs.
 *
 * Revision 1.27  2007/03/29 08:38:54  nitrogen
 * Initial MSP configuration support.
 *
 * Revision 1.26  2007/03/28 08:01:30  nitrogen
 * Initialization improvements, VPX improvements, minor comment and error message tweaks, better docs
 *
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
//#include <../drivers/media/dvb/frontends/nxt200x.h>

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
#define TL_ASSERT(a) ( (a) ? (0) : (printk(KERN_ERR "tl880: assertion '%s' failed in %s at %u\n", #a, __FUNCTION__, __LINE__), (1)) )
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
	u32 id;					/* Card number */
	u16 subsys_vendor_id;			/* Card vendor ID */
	u16 subsys_device_id;			/* Card device ID */
	u16 card_type;				/* Card type identifier */
	char name[64];				/* Card model name */
	struct semaphore *sem;			/* For reentry protection */

	/* Character device stuff */
	struct cdev *char_device;		/* Kernel character device handle */
	u32 major;				/* Major number for all tl880 devices (for convenience) */
	u32 minor;				/* First minor number for this card */


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

	u32 irq;				/* Interrupt line */

	int elseint;				/* Do we share an interrupt? */

	u32 old_mask;				/* Previously used interrupt mask */
	u32 int_mask;				/* Global interrupt enable mask */
	u32 int_type;				/* Type of interrupt to service */
	u32 int_count;				/* Number of interrupts received */

	u32 vsc_mask;				/* VSC interrupt mask */
	u32 vsc_type;				/* Type of VSC interrupt received */
	u32 vsc_count;				/* Number of VSC interrupts received */
	
	u32 apu_mask;				/* APU interrupt mask */
	u32 apu_type;				/* Type of APU interrupt received */
	u32 apu_count;				/* Number of APU interrupts received */

	u32 blt_mask;				/* BLT interrupt mask */
	u32 blt_type;				/* Type of BLT interrupt received */
	u32 blt_count;				/* Number of BLT interrupts received */

	u32 mce_mask;				/* MCE interrupt mask */
	u32 mce_type;				/* Type of MCE interrupt received */
	u32 mce_count;				/* Number of MCE interrupts received */

	u32 mcu_mask;				/* MCU interrupt mask */
	u32 mcu_type;				/* Type of MCU interrupt received */
	u32 mcu_count;				/* Number of MCU interrupts received */

	u32 vpip_mask;				/* VPIP interrupt mask */
	u32 vpip_type;				/* Type of VPIP interrupt received */
	u32 vpip_count;				/* Number of VPIP interrupts received */

	u32 hpip_mask;				/* HPIP interrupt mask */
	u32 hpip_type;				/* Type of HPIP interrupt received */
	u32 hpip_count;				/* Number of HPIP interrupts received */

	u32 dpc_mask;				/* DPC interrupt mask */
	u32 dpc_type;				/* Type of DPC interrupt received */
	u32 dpc_count;				/* Number of DPC interrupts received */

	u32 tsd_mask;				/* TSD interrupt mask */
	u32 tsd_type;				/* Type of TSD interrupt received */
	u32 tsd_count;				/* Number of TSD interrupts received */


	/** Audio (APU) State **/
	enum audio_mode_e { 
		ZERO = 0, ONE, TWO 
	} audio_mode;				/* Current audio mode */

	u32 iau_base;				/* Base card memory address for audio unit */
	u32 iau_iba_reg;			/* Register used for base address */
	u32 iau_iea_reg;			/* Register used for end address */
	u32 iau_ira_reg;			/* Register used for base address (2) */


	/*** Audio Chip (MSP) State ***/
	int msp_addr;				/* Set to 0 if no MSP */
	int msp_i2cbus;				/* Set to index into i2cbuses */
	int msp_i2cclient;			/* Set to index into i2cbuses.i2c_clients */

	
	/*** Video State ***/


	/*** Video Chip (VPX) State ***/
	int vpx_addr;				/* Set to 0 if no VPX */
	int vpx_i2cbus;				/* Set to index into i2cbuses */
	int vpx_i2cclient;			/* Set to index into i2cbuses.i2c_clients */
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

#else /* __KERNEL__ */

/*** Userspace-exclusive definitions and declarations ***/

/* Userspace includes */
#include <linux/types.h>

#endif /* __KERNEL__ */

/*** Userspace/kernel space shared definitions such as ioctls ***/

/* ioctl definitions are subject to change */

/* 
 * Read register IOCTL - register is read from parameter, then the value read
 * is written back to the parameter.  Single unsigned long.
 */
#define TL880IOCREADREG		_IOWR(0xdd, 0, __u32 *)

/*
 * Write register IOCTL - register is read from parameter[0], the value to
 * write is read from parameter[1].  Array [2] __u32.
 */
#define TL880IOCWRITEREG	_IOW(0xdd, 1, __u32 *)

/*
 * VIP state IOCTL - parameter is a pointer to VIP state int (0 off, 1 on, 2
 * special?)
 */
#define TL880IOCSETVIP		_IOW(0xdd, 2, __u32 *)

/*
 * Set cursor position IOCTL - cursor position is packed into __u32
 * as two shorts - i.e. (x & 0xffff) | (y << 16)
 */
#define TL880IOCSETCURSORPOS	_IOW(0xdd, 3, __u32 *)

/*
 * Set card GPIO state - pass two __u32s.
 * parameter[0] = gpio_line
 * parameter[1] = state
 */
#define TL880IOCSETGPIO		_IOW(0xdd, 4, __u32 *)


#ifdef __KERNEL__

/*** Driver variables ***/
extern int debug;
extern dev_t device_number;
extern int n_tl880s;
extern struct tl880_dev *tl880_list;

/*** Driver functions ***/
/* tl880util.c */
void set_bits(u32 *value, u32 reg, int high_bit, int low_bit, u32 setvalue);
struct tl880_dev *find_tl880(unsigned long tl880_id);
struct tl880_dev *find_tl880_pci(struct pci_dev *dev);

/* tl880i2c.c */
int tl880_init_i2c(struct tl880_dev *tl880dev);
void tl880_deinit_i2c(struct tl880_dev *tl880dev);
int tl880_call_i2c_clients(struct tl880_dev *tl880dev, u32 cmd, void *arg);
/* I2C access functions for features not handled by external chip drivers */
int tl880_i2c_read_byte(struct tl880_i2c_bus *i2cbus, u16 addr);
int tl880_i2c_write_byte(struct tl880_i2c_bus *i2cbus, u16 addr, u8 value);
int tl880_i2c_read_byte_data(struct tl880_i2c_bus *i2cbus, u16 addr, u8 command);
int tl880_i2c_write_byte_data(struct tl880_i2c_bus *i2cbus, u16 addr, u8 command, u8 value);
int tl880_i2c_read_word_data(struct tl880_i2c_bus *i2cbus, u16 addr, u8 command);
int tl880_i2c_write_word_data(struct tl880_i2c_bus *i2cbus, u16 addr, u8 command, u16 value);

/* tl880init.c */
void tl880_init_myhd(struct tl880_dev *tl880dev);
void tl880_init_wintv_hd(struct tl880_dev *tl880dev);
void tl880_init_chip(struct tl880_dev *tl880dev);
void tl880_init_dev(struct tl880_dev *tl880dev);

/* tl880gpio.c */
u8 tl880_set_gpio(struct tl880_dev *tl880dev, u32 gpio_line, u8 state);
void tl880_write_gpio1_wintv_hd(struct tl880_dev *tl880dev, u8 state, u8 b, int c);

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
void tl880_set_video_source(struct tl880_dev *tl880dev, u8 a, u8 b);
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
u32 tl880_aux_dma_allocate(void);
void tl880_aux_dma_free(u32 dma);
extern unsigned long dma_bitmask;

int tl880_dma_vop_on(struct tl880_dev *tl880dev);

/* tl880proc.c */
int tl880_create_proc_entry(void);
void tl880_remove_proc_entry(void);

/* tl880mem.c */
u32 tl880_read_memory(struct tl880_dev *tl880dev, u32 mem);
void tl880_write_memory(struct tl880_dev *tl880dev, u32 mem, u32 value);
u32 tl880_read_membits(struct tl880_dev *tl880dev, u32 mem, int high_bit, int low_bit);
void tl880_write_membits(struct tl880_dev *tl880dev, u32 mem, int high_bit, int low_bit, u32 value);
void tl880_clear_sdram(struct tl880_dev *tl880dev, u32 start_addr, u32 end_addr, u32 value);

#endif /* __KERNEL__ */


/*** Driver local includes ***/

#include "tl880reg.h"
#include "tl880dpc.h"
#include "tl880osd.h"
#include "tl880vpx.h"
#include "tl880msp.h"

#endif /* _TL880_H_ */

