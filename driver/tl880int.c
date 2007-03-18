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
		//unsigned long var_10 = 0;  // Should var_10 be bitsval?

		row_cnt = 1;
		set_bits(&bitsval, 0x702c, 0x16, 0xc, 0 /* (cJanus->0x10f94)->0xc */);
		
		if(tl880_read_regbits(tl880dev, 0x7000, 0x16, 0x16)) {
			tl880_write_regbits(tl880dev, 0x7000, 0x16, 0x16, 0);
		} else {
			tl880_write_regbits(tl880dev, 0x7000, 0x16, 0x16, 1);
		}
		
		//tl880_write_register(tl880dev, 0x702c, var_10 | 0x80000000);
		//tl880_write_register(tl880dev, 0x702c, var_10);
		tl880_write_register(tl880dev, 0x702c, bitsval | 0x80000000);
		tl880_write_register(tl880dev, 0x702c, bitsval);
		
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

/* TODO: add separate _bh handlers for each Dpc in the Windows driver */
void tl880_bh(unsigned long tl880_id)
{
	struct tl880_dev *tl880dev;

	if(!(tl880dev = find_tl880(tl880_id))) {
		printk(KERN_ERR "tl880: bottom half given invalid card id: %lu\n", tl880_id);
		return;
	}
	
	printk(KERN_DEBUG "tl880: in bottom half for card %i, int type 0x%08lx\n", tl880dev->id, tl880dev->int_type);

	if(tl880dev->int_type & 0x80) {
		printk(KERN_DEBUG "tl880: vpip interrupt bh\n");

		tl880dev->int_type &= ~0x80;
		if(!tl880dev->int_type) {
			return;
		}
	}
	if(tl880dev->int_type & 0x400) {
		printk(KERN_DEBUG "tl880: tsd interrupt bh\n");

		tl880dev->tsd_type = 0;
		
		tl880dev->int_type &= ~0x400;
		if(!tl880dev->int_type) {
			return;
		}
	}
	if(tl880dev->int_type & 0x40) {
		printk(KERN_DEBUG "tl880: mce interrupt bh\n");

		tl880dev->int_type &= ~0x40;
		if(!tl880dev->int_type) {
			return;
		}
	}
	if(tl880dev->int_type & 0x8) {
		printk(KERN_DEBUG "tl880: dpc interrupt bh: 0x%04lx\n",
			tl880dev->dpc_type);

		tl880dev->int_type &= ~0x8;
		if(!tl880dev->int_type) {
			return;
		}
	}
	if(tl880dev->int_type & 0x10) {
		printk(KERN_DEBUG "tl880: vsc interrupt bh\n");

		tl880dev->int_type &= ~0x10;
		if(!tl880dev->int_type) {
			return;
		}
	}
	if(tl880dev->int_type & 0x1) {
		printk(KERN_DEBUG "tl880: apu interrupt bh\n");

		tl880dev->int_type &= ~0x1;
		if(!tl880dev->int_type) {
			return;
		}
	}
	if(tl880dev->int_type & 0x2) {
		printk(KERN_DEBUG "tl880: blt interrupt bh\n");

		tl880dev->int_type &= ~0x2;
		if(!tl880dev->int_type) {
			return;
		}
	}
	if(tl880dev->int_type & 0x100) {
		printk(KERN_DEBUG "tl880: hpip interrupt bh\n");

		tl880dev->int_type &= ~0x100;
		if(!tl880dev->int_type) {
			return;
		}
	}
	if(tl880dev->int_type & 0x200) {
		printk(KERN_DEBUG "tl880: mcu interrupt bh\n");

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
#ifdef PRE_2619
irqreturn_t tl880_irq(int irq, void *dev_id, struct pt_regs *regs)
#else /* PRE_2619 */
irqreturn_t tl880_irq(int irq, void *dev_id)
#endif /* PRE_2619 */
{
	struct tl880_dev *tl880dev;
	unsigned long int_type, int_mask;

	if(CHECK_NULL(dev_id)) {
		return IRQ_NONE;
	}

	tl880dev = (struct tl880_dev *)dev_id;

	/* Store the current interrupt mask and type */
	int_type = tl880_read_register(tl880dev, 0);
	int_mask = tl880_read_register(tl880dev, 4);

	/* Disable interrupts while processing (does the Windows driver do this?) */
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

	/* If this card is already processing an interrupt, return with interrupts disabled */
	if(tl880dev->int_type) {
		/* This should never get executed */
		printk(KERN_DEBUG "tl880: already handling interrupt: 0x%04lx - disabling interrupts\n", 
				tl880dev->int_type);
		tl880_disable_interrupts(tl880dev);
		return IRQ_HANDLED;
	}

	tl880dev->int_mask = int_mask;
	tl880dev->int_type = int_type & int_mask;
	tl880dev->int_count++;

	/* 
	 * TODO: only schedule worker for tasks that take too long to do in interrupt time.
	 */

	if(tl880dev->int_type & 0x80) {
		tl880dev->vpip_count++;
		/* tl880_write_regbits(tl880dev, 4, 7, 7, 0); */
		if(tl880_vpip_int(tl880dev)) {
			/* VPIP needs BH */
		} else {
			/* VPIP processing is done */
			tl880dev->int_type &= ~0x80;
		}
	}
	/* 
	 * If this is a ts demux interrupt, we may want to remove the ts demux
	 * bit in the interrupt mask here; otherwise system hardlock is possible
	 * due to significant interrupt frequency.
	 */
	if(tl880dev->int_type & 0x400) {
		tl880dev->tsd_count++;
		//tl880dev->int_mask &= ~0x400;	/* Clear TSD interrupt bit */
		if(debug > 0 || tl880dev->tsd_count == 1) {
			printk(KERN_DEBUG "tl880: tsd interrupt: 0x%08lx\n", 
					(tl880dev->tsd_type = tl880_read_register(tl880dev, 0x27814)));
		}

		/* TSD always needs BH queued (KeInsertQueueDpc in Windows) so don't clear interrupt type */
	}
	if(tl880dev->int_type & 0x40) {
		tl880dev->mce_count++;
		//tl880_write_regbits(tl880dev, 4, 6, 6, 0);
		if(debug > 0 || tl880dev->mce_count == 1) {
			printk(KERN_DEBUG "tl880: mce interrupt\n");
		}
	}
	if(tl880dev->int_type & 0x8) {
		tl880dev->dpc_count++;
		/* tl880_write_regbits(tl880dev, 4, 3, 3, 0); */
		if(tl880_dpc_int(tl880dev)) {
			/* DPC needs BH as indicated by return value of tl880_dpc_int */
		} else {
			/* DPC processing is done */
			tl880dev->int_type &= ~0x8;
		}
	}
	if(tl880dev->int_type & 0x10) {
		tl880dev->vsc_count++;
		//tl880_write_regbits(tl880dev, 4, 4, 4, 0);
		if(debug > 0 || tl880dev->vsc_count == 1) {
			printk(KERN_DEBUG "tl880: vsc interrupt\n");
		}
	}
	if(tl880dev->int_type & 0x1) {
		tl880dev->apu_count++;
		//tl880_write_regbits(tl880dev, 4, 0, 0, 0);
		if(debug > 0 || tl880dev->apu_count == 1) {
			printk(KERN_DEBUG "tl880: apu interrupt\n");
		}
	}
	if(tl880dev->int_type & 0x2) {
		tl880dev->blt_count++;
		//tl880_write_regbits(tl880dev, 4, 1, 1, 0);
		if(debug > 0 || tl880dev->blt_count == 1) {
			printk(KERN_DEBUG "tl880: blt interrupt\n");
		}
	}
	if(tl880dev->int_type & 0x100) {
		tl880dev->hpip_count++;
		//tl880_write_regbits(tl880dev, 4, 8, 8, 0);
		if(debug > 0 || tl880dev->hpip_count == 1) {
			printk(KERN_DEBUG "tl880: hpip interrupt\n");
		}
	}
	if(tl880dev->int_type & 0x200) {
		tl880dev->mcu_count++;
		//tl880_write_regbits(tl880dev, 4, 9, 9, 0);
		if(debug > 0 || tl880dev->mcu_count == 1) {
			printk(KERN_DEBUG "tl880: mcu interrupt\n");
		}
	}
	if(tl880dev->int_type & 0x824) {
		printk(KERN_WARNING "tl880: unknown interrupt 0x%08lx- disabling unknown interrupts\n",
			tl880dev->int_type);

		tl880dev->int_mask &= ~0x824;
	}
	
	if(tl880dev->int_type) {
		/* Queue the deferred interrupt handler if necessary */
		tasklet_schedule(&tl880dev->tasklet);
	} else {
		/* Re-enable interrupts */
		tl880_write_register(tl880dev, 4, tl880dev->int_mask);
	}

	return IRQ_HANDLED;
}

void tl880_disable_interrupts(struct tl880_dev *tl880dev)
{
	// TODO: mutex?
	
	tl880dev->old_mask = tl880_read_register(tl880dev, 0x4);
	tl880dev->int_mask = 0;

	tl880_read_register(tl880dev, 0);
	tl880_write_register(tl880dev, 0x4, 0);
	tl880_write_register(tl880dev, 0xc, 0);
	tl880_write_register(tl880dev, 0x6008, 0);
	tl880_write_register(tl880dev, 0x10008, 0);
	tl880_write_register(tl880dev, 0x4010, 0);
	tl880_write_register(tl880dev, 0x1008, 0);

	tl880_write_register(tl880dev, 0x3014, 0);
}

