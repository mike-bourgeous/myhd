/* 
 * Cursor overlay related functions for TL880-based cards
 *
 * (c) 2003 Mike Bourgeous <nitrogen@slimetech.com>
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

#if 0
int tl880_init_osdmem(struct tl880_dev *tl880dev, unsigned long addr, unsigned long size)
{
	edi = 0;

	if(_g_head) {
		tl880_deinit_osdmem(tl880dev);
	}

loc_3a2ee:
	if(!(_g_head = kmalloc(0x1c, 1))) {
		return 0;
	}

loc_3a305:
	eax->field_0 = 0;
	eax = _g_head;
	eax->field_4 = 0;
	eax = _g_head;
	eax->field_8 = 0;
	eax = _g_head;
	eax->field_c = 0;
	eax = _g_head;
	eax->field_10 = 0;
	eax = _g_idTracker_0;
	ecx = _g_head;
	eax++;
	_g_idTracker_0 = eax;
	eax &= 0xfffff;
	eax |= 0x52300000;
	ecx->field_14 = eax;
	eax = _g_head;
	eax->field_18 = 0;

	if(!m_pkmOSD) {
		eax = gpJanus;
		esi = cJanus->field_16870;
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

	sBoardInfo.virtual_address = eax;
	edx = cJanus->field_16874;
	sBoardInfo.memory_length = edx;
	ecx = ecx->field_16870;
	sBoardInfo.physical_address = ecx;
	ecx = _g_head;
	sBoardInfo.field_c = 0x80;
	_g_head->field_4 = eax;
	eax = _g_head;
	ecx = sBoardInfo.physical_address;
	_g_head->field_8 = ecx;
	eax = _g_head;
	ecx = sBoardInfo.memory_length;
	_g_head->field_c = ecx;

	return 1;
}
#endif	

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
			newmem->id = ++_g_idTracker_0;
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

