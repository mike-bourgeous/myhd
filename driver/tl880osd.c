/* 
 * Cursor overlay related functions for TL880-based cards
 *
 * (c) 2003-2007 Mike Bourgeous <nitrogen at users.sourceforge.net>
 *
 * $Log: tl880osd.c,v $
 * Revision 1.7  2007/03/26 19:25:06  nitrogen
 * Added CVS log generation and updated copyrights and e-mail addresses.
 *
 */
#include "tl880.h"

unsigned long _g_idTracker_0 = 0;

struct OSDmemory *_g_head = NULL;

/* This function merges any consecutive free areas to a single area */
int tl880_compact_osdmem(struct tl880_dev *tl880dev)
{
	struct OSDmemory *listp1 = _g_head, *listp2;

	listp1 = _g_head;

	/* Walk through the linked list */
	while(listp1 && listp1->next) {
		listp2 = listp1->next;
		if(!listp1->active && !listp2->active) {
			/* Two consecutive areas are inactive, merge them */
			listp1->size += listp2->size;
			listp1->next = listp2->next;
			kfree(listp2);
		} else {
			/* One or both areas are active, move on */
			listp1 = listp2;
		}
	}

	return 1;
}

int tl880_init_osdmem(struct tl880_dev *tl880dev, unsigned long addr, unsigned long size)
{
	if(_g_head) {
		/* tl880_deinit_osdmem(tl880dev); */
		return 0;
	}

	if(!(_g_head = kmalloc(0x1c, 1))) {
		return 0;
	}

	_g_head->field_0 = 0;		// ex: 00000000
	_g_head->virt_addr = 0;		// ex: 01f10000
	_g_head->phys_addr = 0;		// ex: 001d8000
	_g_head->size = 0;		// ex: 00000517
	_g_head->active = 0;		// ex: 00000001
	_g_head->id = (++_g_idTracker_0 & 0xfffff) | 0x52300000;	// ex: 52300002
	_g_head->next = 0;		// ex: e27a8388

#if 0
	/* The next section of code maps to userspace */
	if(!m_pkmOSD) {
		eax = gpJanus;
		esi = cJanus->field_16870;	/* base mem address + 0x186000 ? */
		esi += cJanus->field_8;
		if(!(eax = __imp__ExAllocatePoolWithTag(0, 8, ' mdW'))) {
			eax = 0;
		} else {
			ecx = gpJanus;
			ecx = eax;
			KMemory::KMemory(esi, cJanus->field_16874, 0);
		}
	} else {
		goto loc_3a39e;
	}

loc_3a38f:
	if(!(m_pkmOSD = eax)) {
		_DbgBreakPoint();
	}

loc_3a39e:
	ecx = m_pkmOSD;
	eax = m_pkmOSD.MapToUserSpace();
	
	ecx = gpJanus;
#endif
	
	/*
	sBoardInfo.virtual_address = eax; // result from MapToUserSpace (ex. tl880dev->memspace + 0x186000)
	sBoardInfo.memory_length = cJanus->field_16874;
	sBoardInfo.physical_address = cJanus->field_16870;
	sBoardInfo.field_c = 0x80;
	*/
	_g_head->virt_addr = (unsigned long)(tl880dev->memspace + 0x186000);
	/* _g_head->field_8 = sBoardInfo.physical_address; */
	/* _g_head->field_8 = cJanus->field_16870; */
	_g_head->phys_addr = tl880dev->memphys + 0x186000;
	/* _g_head->size = sBoardInfo.memory_length; */
	_g_head->size = 0x1000000;

	return 1;
}


unsigned long tl880_allocate_osdmem(struct tl880_dev *tl880dev, unsigned long size, unsigned long align)
{
	struct OSDmemory *listp1, *newmem;
	unsigned long retval;
	unsigned long alignsize = align - 1;
	unsigned long mask = ~alignsize;
	unsigned long finalsize = size + alignsize;
	unsigned long active_size = 0, inactive_size = 0;
	
	listp1 = _g_head;

	if(!_g_head) {
		//initializeOSDMemory(sBoardInfo.osd_address, sBoardInfo.virt_addr);
	}

	tl880_compact_osdmem(tl880dev);

	/* Search for an inactive block of the correct size */
	listp1 = _g_head;
	while(listp1 != NULL) {
		if(!listp1->active && listp1->size == finalsize) {
			/* An inactive block was found; set it active */
			listp1->active = 1;
			retval = (alignsize + listp1->virt_addr) & mask;
			listp1->field_0 = retval - listp1->virt_addr;
			
			return retval;
		}
		listp1 = listp1->next;
	}

	/* Search for an inactive block greater than the correct size */
	listp1 = _g_head;
	while(listp1) {
		if(!listp1->active && listp1->size > finalsize) {
			if(!(newmem = kmalloc(0x1c, 1))) {
				return 0;
			}
			
			newmem->field_0 = 0;
			newmem->virt_addr = 0;
			newmem->phys_addr = 0;
			newmem->size = 0;
			newmem->active = 0;
			newmem->id = (++_g_idTracker_0 & 0xfffff) | 0x52300000;
			newmem->next = listp1->next;
			listp1->next = newmem;
			newmem->virt_addr = listp1->virt_addr + finalsize;
			newmem->phys_addr = listp1->phys_addr + finalsize;
			newmem->size = listp1->size - finalsize;
			listp1->size = finalsize;
			listp1->active = 1;
		}
	}

	/* Out of space; count active and inactive memory and return */
	listp1 = _g_head;
	while(listp1) {
		if(listp1->active) {
			active_size += listp1->size;
		} else {
			inactive_size += listp1->size;
		}
		listp1 = listp1->next;
	}
	printk(KERN_WARNING "tl880: out of OSD memory; active bytes: 0x%lx, inactive bytes: 0x%lx\n", active_size, inactive_size);
	return 0;
}

unsigned long tl880_get_osdmem_offset(struct tl880_dev *tl880dev, unsigned long addr)
{
	struct OSDmemory *listp = _g_head;

	/* Walk linked list */
	while(listp) {
		/* Check for invalid memory block ID (sign of memory corruption or bug) */
		if((listp->id & 0xfff00000) != 0x52300000) {
			printk(KERN_ERR "tl880: invalid ID in tl880_get_osdmem_offset: 0x%08lx (should be 0x523xxxxx)\n", listp->id);
			break;
		}

		/* If this block matches, return its address */
		if(listp->active && listp->virt_addr + listp->field_0 == addr) {
			return listp->phys_addr + listp->field_0;
		}

		listp = listp->next;
	}

	return -1;
}

int tl880_deallocate_osdmem(struct tl880_dev *tl880dev, unsigned long addr)
{
	struct OSDmemory *listp = _g_head;

	/* Walk the list */
	while((listp != NULL) && ((listp->id & 0xfff00000) == 0x52300000)) {
		/* If the address matches, set the block inactive */
		if(listp->virt_addr + listp->field_0 == addr) {
			listp->active = 0;
			listp->field_0 = 0;
			return 1;
		}
		listp = listp->next;
	}

	return 0;
}

