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
	unsigned long esi;

	if(!_g_head) {
		return 0;
	}

loc_3a5aa:
	esi = listp->virt_addr;
	esi += listp->field_0;

	if(esi == addr) {
		goto loc_3a5cb;
	}

	esi = listp->id;
	esi &= 0xfff00000;
	if(esi != 0x52300000) {
		return 0;
	}

	listp = listp->next;

	if(listp) {
		goto loc_3a5aa;
	}

loc_3a5cb:
	if(!listp) {
		return 0;
	}

	esi = listp->id;
	esi &= 0xfff00000;
	if(esi != 0x52300000) {
		return 0;
	}

	listp->active = 0;
	listp->field_0 = 0;

	return 1;
}

