/* 
 * tl880int.c - Interrupt Routines for TL880 cards
 *
 * (c) 2003-2007 Mike Bourgeous <nitrogen at users.sourceforge.net>
 *
 * $Log: tl880int.c,v $
 * Revision 1.21  2008/06/28 02:12:40  nitrogen
 * Importing old changes.  See ChangeLog.
 *
 * Revision 1.20  2007/09/06 05:22:05  nitrogen
 * Improvements to audio support, documentation, and card memory management.
 *
 * Revision 1.19  2007/04/24 06:32:13  nitrogen
 * Changed most int/long types to explicit 32-bit sizes.  Fixed compilation and execution on 64-bit CPUs.
 *
 * Revision 1.18  2007/03/26 19:25:06  nitrogen
 * Added CVS log generation and updated copyrights and e-mail addresses.
 *
 */

#include "tl880.h"


void tl880_vpip_handler(struct tl880_dev *tl880dev)
{
	static unsigned int row_cnt = 0; /* evil - put in struct */
	static unsigned int field_cnt = 0; /* evil - put in struct */
	u32 bitsval = 0;
	
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
	tl880dev->vpip_type = tl880_read_register(tl880dev, 0x7008) & tl880_read_register(tl880dev, 0x7004);
	
	printk(KERN_DEBUG "tl880: vpip interrupt: 0x%08x\n", tl880dev->vpip_type);

	if(tl880dev->vpip_type & 1 /* || !cJanus[0x10388][0x284] */) {
		tl880_vpip_handler(tl880dev);
	}

	tl880dev->vpip_type = 0;

	return 0;
}

int tl880_add_int_callback(struct tl880_dev *tl880dev, struct int_func_desc **list, int_func func, void *private_data)
{
	struct int_func_desc *new, *p;

	if(CHECK_NULL(tl880dev) || CHECK_NULL(list) || CHECK_NULL(func)) {
		return -1;
	}

	/* TODO: Check for duplicate entries in the list */

	if(CHECK_NULL(new = kmalloc(sizeof(struct int_func_desc), GFP_KERNEL))) {
		return -1;
	}
	new->func = func;
	new->private_data = private_data;
	new->next = NULL;

	if(*list == NULL) {
		/* Empty list - insert as head element */
		*list = new;
	} else {
		/* Non-empty list */
		for(p = *list; p->next != NULL; p = p->next) ;
		p->next = new;
	}

	return 0;
}

void tl880_remove_int_callback(struct tl880_dev *tl880dev, struct int_func_desc **list, int_func func)
{
	struct int_func_desc *p, *p2;

	if(CHECK_NULL(tl880dev) || CHECK_NULL(list) || CHECK_NULL(func)) {
		return;
	}

	if(*list == NULL) {
		/* Empty list - do nothing */
		printk(KERN_DEBUG "tl880: Attempt to remove int callback from empty callback list\n");
		return;
	} else if((*list)->func == func) {
		/* Function is in list head */
		p = *list;
		*list = (*list)->next;
		kfree(p);
	} else {
		/* TODO: test this code, maybe add a mutex */
		for(p = *list; p->next != NULL && p->next->func != func; p = p->next) ;
		if(p->next != NULL && p->next->func == func) {
			p2 = p->next;
			p->next = p2->next;
			kfree(p2);
		} else {
			printk(KERN_DEBUG "tl880: Attempt to remove nonexistant int callback\n");
		}
	}

	return;
}

void tl880_run_callbacks(struct tl880_dev *tl880dev, struct int_func_desc *list)
{
	struct int_func_desc *p = list;

	if(CHECK_NULL(tl880dev)) {
		return;
	}

	while(p != NULL) {
		p->func(tl880dev, p->private_data);
		p = p->next;
	}
}

/* To be used by ALSA driver - returns zero on success, -1 on error */
int tl880_register_apu_th(struct tl880_dev *tl880dev, int_func func, void *private_data)
{
	if(CHECK_NULL(tl880dev) || CHECK_NULL(func)) {
		return -1;
	}
	return tl880_add_int_callback(tl880dev, &tl880dev->apu_th_funcs, func, private_data);
}
EXPORT_SYMBOL(tl880_register_apu_th);

void tl880_unregister_apu_th(struct tl880_dev *tl880dev, int_func func)
{
	if(CHECK_NULL(tl880dev) || CHECK_NULL(func)) {
		return;
	}
	tl880_remove_int_callback(tl880dev, &tl880dev->apu_th_funcs, func);
}
EXPORT_SYMBOL(tl880_unregister_apu_th);

int tl880_apu_int(struct tl880_dev *tl880dev)
{
	if(debug > 0 || tl880dev->apu_count == 1) {
		printk(KERN_DEBUG "tl880: apu interrupt\n");
	}

	tl880_run_callbacks(tl880dev, tl880dev->apu_th_funcs);

	if(tl880_read_register(tl880dev, 0x3018) &
			tl880_read_register(tl880dev, 0x3014) &
			0x1) {
		return 1;
	}

	return 0;
}

int tl880_hpip_int(struct tl880_dev *tl880dev)
{
	tl880_read_register(tl880dev, 0x800c);
	tl880_read_register(tl880dev, 0x8008);
	return 0;
}

int tl880_blt_int(struct tl880_dev *tl880dev)
{
	if(tl880_read_register(tl880dev, 0x4014) &
			tl880_read_register(tl880dev, 0x4010) &
	       		0x10) {
		return 1;
	}

	return 0;
}

int tl880_mcu_int(struct tl880_dev *tl880dev)
{
	tl880_read_register(tl880dev, 0x1d810);

	// gpJanus+10F90 = 2;
	
	return 1;
}

/* TODO: add separate _bh handlers for each Dpc in the Windows driver */
void tl880_bh(unsigned long tl880_id)
{
	struct tl880_dev *tl880dev;

	if(!(tl880dev = find_tl880(tl880_id))) {
		printk(KERN_ERR "tl880: bottom half given invalid card id: %lu\n", tl880_id);
		return;
	}
	
	if(debug || tl880dev->int_count == 1) {
		printk(KERN_DEBUG "tl880: in bottom half for card %i, int type 0x%08x\n", tl880dev->id, tl880dev->int_type);
	}

	/* I think this is a legitimate use of goto (first time for me!) */
	if(tl880dev->int_type & 0x80) {
		if((debug && tl880dev->vpip_count % 100 == 0) || tl880dev->vpip_count == 1) {
			printk(KERN_DEBUG "tl880: vpip interrupt bh: count %d\n", tl880dev->vpip_count);
		}

		tl880dev->int_type &= ~0x80;
		if(!tl880dev->int_type) {
			goto done;
		}
	}
	if(tl880dev->int_type & 0x400) {
		if((debug && tl880dev->tsd_count % 100 == 0) || tl880dev->tsd_count == 1) {
			printk(KERN_DEBUG "tl880: tsd interrupt bh: count %d\n", tl880dev->tsd_count);
		}

		tl880dev->tsd_type = 0;
		
		tl880dev->int_type &= ~0x400;
		if(!tl880dev->int_type) {
			goto done;
		}
	}
	if(tl880dev->int_type & 0x40) {
		if((debug && tl880dev->mce_count % 100 == 0) || tl880dev->mce_count == 1) {
			printk(KERN_DEBUG "tl880: mce interrupt bh: count %d\n", tl880dev->mce_count);
		}

		tl880dev->int_type &= ~0x40;
		if(!tl880dev->int_type) {
			goto done;
		}
	}
	if(tl880dev->int_type & 0x8) {
		if((debug && tl880dev->dpc_count % 100 == 0) || tl880dev->dpc_count == 1) {
			printk(KERN_DEBUG "tl880: dpc interrupt bh: count %d type %08x\n", 
					tl880dev->dpc_count, tl880dev->dpc_type);
		}

		tl880dev->int_type &= ~0x8;
		if(!tl880dev->int_type) {
			goto done;
		}
	}
	if(tl880dev->int_type & 0x10) {
		if((debug && tl880dev->vsc_count % 100 == 0) || tl880dev->vsc_count == 1) {
			printk(KERN_DEBUG "tl880: vsc interrupt bh: count %d\n", tl880dev->vsc_count);
		}

		tl880dev->int_type &= ~0x10;
		if(!tl880dev->int_type) {
			goto done;
		}
	}
	if(tl880dev->int_type & 0x1) {
		if((debug && tl880dev->apu_count % 100 == 0) || tl880dev->apu_count == 1) {
			printk(KERN_DEBUG "tl880: apu interrupt bh: count %d\n", tl880dev->apu_count);
		}

		tl880_ntsc_audio_dpc(tl880dev);

		tl880dev->int_type &= ~0x1;
		if(!tl880dev->int_type) {
			goto done;
		}
	}
	if(tl880dev->int_type & 0x2) {
		if((debug && tl880dev->blt_count % 100 == 0) || tl880dev->blt_count == 1) {
			printk(KERN_DEBUG "tl880: blt interrupt bh: count %d\n", tl880dev->blt_count);
		}

		tl880dev->int_type &= ~0x2;
		if(!tl880dev->int_type) {
			goto done;
		}
	}
	if(tl880dev->int_type & 0x100) {
		if((debug && tl880dev->hpip_count % 100 == 0) || tl880dev->hpip_count == 1) {
			printk(KERN_DEBUG "tl880: hpip interrupt bh: count %d\n", tl880dev->hpip_count);
		}

		tl880dev->int_type &= ~0x100;
		if(!tl880dev->int_type) {
			goto done;
		}
	}
	if(tl880dev->int_type & 0x200) {
		if((debug && tl880dev->mcu_count % 100 == 0) || tl880dev->mcu_count == 1) {
			printk(KERN_DEBUG "tl880: mcu interrupt bh: count %d\n", tl880dev->mcu_count);
		}

		tl880dev->int_type &= ~0x200;
		if(!tl880dev->int_type) {
			goto done;
		}
	}

	tl880dev->int_type = 0;

done:
	/* Re-enable interrupts */
	tl880_write_register(tl880dev, 4, tl880dev->int_mask);

	if(debug) {
		printk(KERN_DEBUG "tl880: leaving bh with int_mask %08x\n", tl880dev->int_mask);
	}
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
	tl880dev->int_mask = int_mask = tl880_read_register(tl880dev, 4);

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
		printk(KERN_DEBUG "tl880: already handling interrupt: 0x%04x - disabling interrupts\n", 
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

	/*
	 * TODO: split up bh by chip function, as in the Windows driver (i.e. APU BH, DPC BH, etc.)
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
			printk(KERN_DEBUG "tl880: tsd interrupt: 0x%08x\n", 
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
		if(tl880_apu_int(tl880dev)) {
			/* Need BH */
		} else {
			/* Don't need BH */
			tl880dev->int_type &= ~0x1;
		}
	}
	if(tl880dev->int_type & 0x2) {
		tl880dev->blt_count++;
		//tl880_write_regbits(tl880dev, 4, 1, 1, 0);
		if(debug > 0 || tl880dev->blt_count == 1) {
			printk(KERN_DEBUG "tl880: blt interrupt\n");
		}
		if(tl880_blt_int(tl880dev)) {
			// Schedule BH
		} else {
			// Don't schedule BH
			tl880dev->int_type &= ~0x2;
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
		printk(KERN_WARNING "tl880: unknown interrupt 0x%08x- disabling unknown interrupts\n",
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

