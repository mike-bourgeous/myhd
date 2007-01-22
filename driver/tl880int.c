/* 
 * tl880int.c - Interrupt Routines for TL880 cards
 *
 * (c) 2003 Mike Bourgeous <nitrogen@slimetech.com>
 */

#include "tl880.h"


void tl880_vpip_handler(struct tl880_dev *tl880dev)
{
	static unsigned int row_cnt = 0; /* evil - put in struct */
	static unsigned int field_cnt = 0; /* evil - put in struct */
	unsigned long bitsval = 0;
	
	/* cJanus->0x10f94 is a pointer to struct tagVpipProgList */
	
	/* esi = cJanus->0x10f94; */
	/* cJanus->0x10f98 = 1; // is this VPIP_in_middle_of_field? */

	if(row_cnt < 0x10 /* number of rows in vpip table? */) {
		tl880_write_regbits(tl880dev, 0x7000, 0x15, 0x10, 0x3d /* (cJanus->0x10f94)->0xd8[row_cnt * 4] */);
		tl880_write_register(tl880dev, 0x701c, 0x92 /* (cJanus->0x10f94)->0x10[row_cnt * 4] */);
		row_cnt++;
	} else if(field_cnt < 1 /* number of fields */) {
		/* 
		 * Example of registers 7000-703f:
		 * 7000 003DE310  00000001  00000000  0B0A0706      ..=.............
		 * 7010 00000000  00000000  000002D0  1B236491      .............d#.
		 * 7020 1E000010  00000000  00000000  00000000      ................
		 * 7030 0000003E  00000000  00000000  00000000      >...............
		 */
		unsigned long var_10 = 0;

		row_cnt = 1;
		set_bits(&bitsval, 0x702c, 0x16, 0xc, 0 /* (cJanus->0x10f94)->0xc */);
		
		if(tl880_read_regbits(tl880dev, 0x7000, 0x16, 0x16)) {
			tl880_write_regbits(tl880dev, 0x7000, 0x16, 0x16, 0);
		} else {
			tl880_write_regbits(tl880dev, 0x7000, 0x16, 0x16, 1);
		}
		
		tl880_write_register(tl880dev, 0x702c, var_10 | 0x80000000);
		tl880_write_register(tl880dev, 0x702c, var_10);
		
		tl880_write_regbits(tl880dev, 0x7000, 0x15, 0x10, 0x3d /* (cJanus->0x10f94)->0xd8 */);
		
		tl880_write_register(tl880dev, 0x701c, 0x92 /* (cJanus->0x10f94)->0x10 */);
		
		field_cnt++;
	} else {
	row_cnt = 1;
	field_cnt = 1;
	/* cJanus->0x10f98 = 0; */
	
	/* (cJanus->0x10388)->0xb70 = 1; */
	}
}


/* Each chip component's interrupt handler should return 1 if BH is needed, else 0 */
int tl880_vpip_int(struct tl880_dev *tl880dev)
{
	tl880dev->vpip_count++;
	tl880dev->vpip_type = tl880_read_register(tl880dev, 0x7008) & tl880_read_register(tl880dev, 0x7004);
	
	printk(KERN_DEBUG "tl880: vpip interrupt: 0x%08lx\n", tl880dev->vpip_type);


	if(tl880dev->vpip_type & 1 /* || !cJanus[0x10388][0x284] */) {
		tl880_vpip_handler(tl880dev);
	}

	tl880dev->vpip_type = 0;

	return 0;
}

void tl880_bh(unsigned long tl880_id)
{
	struct tl880_dev *tl880dev;

	if(!(tl880dev = find_tl880(tl880_id))) {
		printk(KERN_ERR "tl880: bottom half given invalid card id: %lu\n", tl880_id);
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

		tl880dev->tsd_type = 0;
		
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
			tl880dev->dpc_type);

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

	/* Re-enable interrupts */
	tl880_write_register(tl880dev, 4, tl880dev->int_mask);
}


/* Interrupt handler */
irqreturn_t tl880_irq(int irq, void *dev_id)
{
#if 0
	struct tl880_dev *tl880dev;
	unsigned long int_type, int_mask;

	if(CHECK_NULL(dev_id)) {
		return IRQ_NONE;
	}

	tl880dev = (struct tl880_dev *)dev_id;

	/* Store the current interrupt mask and type */
	int_type = tl880_read_register(tl880dev, 0);
	int_mask = tl880_read_register(tl880dev, 4);

	/* Disable interrupts while processing */
	tl880_write_register(tl880dev, 4, 0);


	/* If no bits in type and mask match, then the interrupt was for some other device */
	if(!(int_type & int_mask)) {
		if(tl880dev->elseint == 0) {
			printk(KERN_DEBUG "tl880: receiving someone else's interrupt(s)\n");
			tl880dev->elseint = 1;
		}
		/* tl880_write_register(tl880dev, 4, int_mask); */
		return IRQ_NONE;
	}

	printk(KERN_DEBUG "tl880: Processing interrupt\n");

	/* If this card is already processing an interrupt, return with interrupts disabled */
	if(tl880dev->int_type) {
		printk(KERN_DEBUG "tl880: already handling interrupt: 0x%04lx\n", tl880dev->int_type);
		tl880_disable_interrupts(tl880dev);
		return IRQ_HANDLED;
	}

	tl880dev->int_mask = int_mask;
	tl880dev->int_type = int_type & int_mask;

	/* 
	 * TODO: only schedule worker for tasks that take too long to do in interrupt time.
	 */

	if(tl880dev->int_type & 0x80) {
		/* tl880_write_regbits(tl880dev, 4, 7, 7, 0); */
		if(tl880_vpip_int(tl880dev)) {
			/* VPIP needs BH */
		} else {
			/* VPIP processing is done */
			tl880dev->int_type &= ~0x80;
		}
	}
	/* 
	 * If this is a ts demux interrupt, we have to remove the ts demux bit
	 * in the interrupt mask here; otherwise system hardlock is probable.
	 */
	if(tl880dev->int_type & 0x400) {
		tl880dev->int_mask &= ~0x400;	/* Clear TSD interrupt bit */
		printk(KERN_DEBUG "tl880: ts demux interrupt: 0x%08lx\n", 
			tl880dev->tsd_type = tl880_read_register(tl880dev, 0x27814));

		/* TSD always needs BH queued (KeInsertQueueDpc in Windows) */
	}
	if(tl880dev->int_type & 0x40) {
		tl880_write_regbits(tl880dev, 4, 6, 6, 0);
		printk(KERN_DEBUG "tl880: mce interrupt\n");
	}
	if(tl880dev->int_type & 0x8) {
		/* tl880_write_regbits(tl880dev, 4, 3, 3, 0); */
		if(tl880_dpc_int(tl880dev)) {
			/* DPC needs BH */
		} else {
			/* DPC processing is done */
			tl880dev->int_type &= ~0x8;
		}
	}
	if(tl880dev->int_type & 0x10) {
		tl880_write_regbits(tl880dev, 4, 4, 4, 0);
		printk(KERN_DEBUG "tl880: vsc interrupt\n");
	}
	if(tl880dev->int_type & 0x1) {
		tl880_write_regbits(tl880dev, 4, 0, 0, 0);
		printk(KERN_DEBUG "tl880: apu interrupt\n");
	}
	if(tl880dev->int_type & 0x2) {
		tl880_write_regbits(tl880dev, 4, 1, 1, 0);
		printk(KERN_DEBUG "tl880: blt interrupt\n");
	}
	if(tl880dev->int_type & 0x100) {
		tl880_write_regbits(tl880dev, 4, 8, 8, 0);
		printk(KERN_DEBUG "tl880: hpip interrupt\n");
	}
	if(tl880dev->int_type & 0x200) {
		tl880_write_regbits(tl880dev, 4, 9, 9, 0);
		printk(KERN_DEBUG "tl880: mcu interrupt\n");
	}
	if(tl880dev->int_type & 0x824) {
		printk(KERN_DEBUG "tl880: unknown interrupt 0x%08lx- disabling interrupts\n",
			tl880dev->int_type);
		tl880_write_register(tl880dev, 4, 0);
	}
	
	if(tl880dev->int_type) {
		/* Queue the deferred interrupt handler if necessary */
		/*
		queue_task(&tl880dev->bh, &tq_immediate)
		mark_bh(IMMEDIATE_BH);
		*/
		/*
		if(!queue_work(tl880dev->wqueue, &tl880dev->worker)) {
			printk(KERN_WARNING "tl880: Uh...  Interrupt handler already queued.  Is this bad?\n");
		}
		*/
		tasklet_schedule(&tl880dev->tasklet);
	} else {
		/* Re-enable interrupts */
		tl880_write_register(tl880dev, 4, tl880dev->int_mask);
	}
#endif

	return IRQ_HANDLED;
}

void tl880_disable_interrupts(struct tl880_dev *tl880dev)
{
	unsigned long oldmask;
	
	/* XXX: Should this handle pending interrupt requests instead? */
	tl880_read_register(tl880dev, 0);
	oldmask = tl880_read_register(tl880dev, 0x4);
	tl880_write_register(tl880dev, 0x4, 0);
	tl880_write_register(tl880dev, 0xc, 0);
	tl880_write_register(tl880dev, 0x6008, 0);
	tl880_write_register(tl880dev, 0x10008, 0);
	tl880_write_register(tl880dev, 0x4010, 0);
	tl880_write_register(tl880dev, 0x1008, 0);
}

