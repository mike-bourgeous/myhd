/* 
 * tl880int.c - Interrupt Routines for TL880 cards
 *
 * (c) 2003 Mike Bourgeous <nitrogen@slimetech.com>
 */

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/major.h>
#include <linux/slab.h>
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
#include <asm/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/videodev.h>

#include "tl880.h"

void __init tl880_bh(void *dev_id)
{
	struct tl880_dev *tl880dev = (struct tl880_dev*)dev_id;

	if(!dev_id) {
		return;
	}
	
	printk(KERN_DEBUG "tl880: in bottom half for card %i, int type 0x%08lx\n", tl880dev->id, tl880dev->int_type);

	if(tl880dev->int_type & 0x80) {
		printk(KERN_DEBUG "tl880: vpip interrupt\n");

		tl880dev->int_type &= ~0x80;
		if(!tl880dev->int_type) {
			return;
		}
	}
	if(tl880dev->int_type & 0x400) {
		printk(KERN_DEBUG "tl880: tsd interrupt\n");

		tl880dev->tsd_data = 0;
		
		tl880dev->int_type &= ~0x400;
		if(!tl880dev->int_type) {
			return;
		}
	}
	if(tl880dev->int_type & 0x40) {
		printk(KERN_DEBUG "tl880: mce interrupt\n");

		tl880dev->int_type &= ~0x40;
		if(!tl880dev->int_type) {
			return;
		}
	}
	if(tl880dev->int_type & 0x8) {
		printk(KERN_DEBUG "tl880: dpc interrupt: 0x%04lx\n",
			tl880dev->dpc_data);

		write_register(tl880dev, 0x10008, tl880dev->dpc_mask);

		tl880dev->dpc_data = 0;
		tl880dev->dpc_mask = 0;

		tl880dev->int_type &= ~0x8;
		if(!tl880dev->int_type) {
			return;
		}
	}
	if(tl880dev->int_type & 0x10) {
		printk(KERN_DEBUG "tl880: vsc interrupt\n");

		tl880dev->int_type &= ~0x10;
		if(!tl880dev->int_type) {
			return;
		}
	}
	if(tl880dev->int_type & 0x1) {
		printk(KERN_DEBUG "tl880: apu interrupt\n");

		tl880dev->int_type &= ~0x1;
		if(!tl880dev->int_type) {
			return;
		}
	}
	if(tl880dev->int_type & 0x2) {
		printk(KERN_DEBUG "tl880: blt interrupt\n");

		tl880dev->int_type &= ~0x2;
		if(!tl880dev->int_type) {
			return;
		}
	}
	if(tl880dev->int_type & 0x100) {
		printk(KERN_DEBUG "tl880: hpip interrupt\n");

		tl880dev->int_type &= ~0x100;
		if(!tl880dev->int_type) {
			return;
		}
	}
	if(tl880dev->int_type & 0x200) {
		printk(KERN_DEBUG "tl880: mcu interrupt\n");

		tl880dev->int_type &= ~0x200;
		if(!tl880dev->int_type) {
			return;
		}
	}

	tl880dev->int_type = 0;
}

void __init tl880_irq(int irq, void *dev_id, struct pt_regs *regs)
{
	struct tl880_dev *tl880dev = (struct tl880_dev*)dev_id;
	unsigned long int_type, int_mask;

	int_type = read_register(tl880dev, 0);
	int_mask = read_register(tl880dev, 4);

	/* If no bits in type and mask match, then the interrupt was for some other device */
	if(!(int_type & int_mask)) {
		return;
	}

	/* If this card is already processing an interrupt, disable interrupts and return */
	if(tl880dev->int_type) {
		printk(KERN_DEBUG "tl880: already handling interrupt: 0x%04lx\n", tl880dev->int_type);
		write_register(tl880dev, 4, 0);
		return;
	}

	tl880dev->int_type = int_type & int_mask;

	/* Disable interrupts */
	/* write_register(tl880dev, 4, 0); */

	if(tl880dev->int_type & 0x80) {
		write_regbits(tl880dev, 4, 7, 7, 0);
		printk(KERN_DEBUG "tl880: vpip interrupt\n");
	}
	/* 
	 * If this is a ts demux interrupt, we have to remove the ts demux bit
	 * in the interrupt mask here; otherwise system hardlock is probable.
	 */
	if(tl880dev->int_type & 0x400) {
		write_regbits(tl880dev, 4, 0xa, 0xa, 0);	/* Clear tsd bit in the int register */
		printk(KERN_DEBUG "tl880: ts demux interrupt: 0x%08lx\n", 
			tl880dev->tsd_data = read_register(tl880dev, 0x27814));
	}
	if(tl880dev->int_type & 0x40) {
		write_regbits(tl880dev, 4, 6, 6, 0);
		printk(KERN_DEBUG "tl880: mce interrupt\n");
	}
	if(tl880dev->int_type & 0x8) {
		tl880dev->dpc_mask = read_register(tl880dev, 0x10008);
		write_regbits(tl880dev, 4, 3, 3, 0);
		printk(KERN_DEBUG "tl880: dpc interrupt: 0x%04lx\n", 
			tl880dev->dpc_data = (read_register(tl880dev, 0x1000c) &
			read_register(tl880dev, 0x10008)));

		write_register(tl880dev, 0x10008, 0);
	}
	if(tl880dev->int_type & 0x10) {
		write_regbits(tl880dev, 4, 4, 4, 0);
		printk(KERN_DEBUG "tl880: vsc interrupt\n");
	}
	if(tl880dev->int_type & 0x1) {
		write_regbits(tl880dev, 4, 0, 0, 0);
		printk(KERN_DEBUG "tl880: apu interrupt\n");
	}
	if(tl880dev->int_type & 0x2) {
		write_regbits(tl880dev, 4, 1, 1, 0);
		printk(KERN_DEBUG "tl880: blt interrupt\n");
	}
	if(tl880dev->int_type & 0x100) {
		write_regbits(tl880dev, 4, 8, 8, 0);
		printk(KERN_DEBUG "tl880: hpip interrupt\n");
	}
	if(tl880dev->int_type & 0x200) {
		write_regbits(tl880dev, 4, 9, 9, 0);
		printk(KERN_DEBUG "tl880: mcu interrupt\n");
	}
	if(tl880dev->int_type & 0x824) {
		printk(KERN_DEBUG "tl880: unknown interrupt 0x%08lx- disabling interrupts\n",
			tl880dev->int_type);
		write_register(tl880dev, 4, 0);
	}

	/* Queue the bottom half */
	queue_task(&tl880dev->bh, &tq_immediate);
	mark_bh(IMMEDIATE_BH);
}

void tl880_disable_interrupts(struct tl880_dev *tl880dev)
{
	read_register(tl880dev, 0);
	write_register(tl880dev, 0x4, 0);
	write_register(tl880dev, 0xc, 0);
	write_register(tl880dev, 0x6008, 0);
	write_register(tl880dev, 0x10008, 0);
	write_register(tl880dev, 0x4010, 0);
	write_register(tl880dev, 0x1008, 0);
}

