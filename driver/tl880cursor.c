/* 
 * Cursor overlay related functions for TL880-based cards
 *
 * (c) 2003 Mike Bourgeous <nitrogen@slimetech.com>
 */
#include "tl880.h"

struct _SURFACE_DESC {
	unsigned long field_0;
	unsigned long field_4;
	unsigned long field_8;
	unsigned long field_c;
	unsigned short field_10;
	unsigned short field_12;
	unsigned short field_14;
	unsigned short field_16;
	unsigned short field_18;
};

struct SOverlaySurface {
	unsigned long field_0;
	unsigned long field_4;
	unsigned long field_8;
	unsigned long field_c;
	unsigned long field_10;
	unsigned long field_14; // _SURFACE_DESC->field_4
	unsigned long field_18; // _SURFACE_DESC->field_8
	unsigned long field_1c;
	unsigned long field_20;
	unsigned long field_24;
	unsigned long field_28;
	unsigned long field_2c;
	unsigned long field_30; // _SURFACE_DESC->field_10
	unsigned long field_34; // _SURFACE_DESC->field_12
	unsigned long field_38;
	unsigned long field_3c;
	unsigned long field_40;
	unsigned long field_44;
	unsigned long field_48;
	unsigned long field_4c;
	unsigned long field_50;
	unsigned long field_54;
	unsigned long field_58;
	unsigned long field_5c;
	unsigned long field_60;
	unsigned long field_64;
	unsigned long surface_id;
	struct SOverlaySurface *next;
};

unsigned long _g_idTracker = 0;

void tl880_add_cursor(struct SOverlaySurface *surface);

struct SOverlaySurface *tl880_create_cursor(struct _SURFACE_DESC *surface)
{
	struct SOverlaySurface *new_surface;

	if(!surface) {
		return NULL;
	}

	if(surface->field_18 != 0x1c) { // dword
		return NULL;
	}

	if(surface->field_10 != 0x20) { // word
		return NULL;
	}

	if(surface->field_12 != 0x20) { // word
		return NULL;
	}

	if(!surface->field_14) { // word
		return NULL;
	}

	if(surface->field_14 >= 5) { // word
		return NULL;
	}

	if(!(new_surface = kmalloc(sizeof(struct SOverlaySurface), 1))) { // 112
		return NULL;
	}

	memset(new_surface, 0, sizeof(struct SOverlaySurface));

	new_surface->surface_id = (++_g_idTracker & 0xfffff) | 0x52300000;
	new_surface->next = NULL;

	new_surface->field_14 = surface->field_4;
	new_surface->field_18 = surface->field_8;
	new_surface->field_30 = surface->field_10;
	new_surface->field_34 = surface->field_12;
	
	new_surface->field_3c = (surface->field_14 != 4) ? surface->field_14 : 0;
	
	new_surface->field_40 = (surface->field_16 != 4) ? surface->field_16 : 0;

	new_surface->field_50 = surface->field_0;

	if(1 /* !(new_surface->field_8 = allocateOSDMemory_Aligned(0x200, 0x10)) */) {
		kfree(new_surface);
		return NULL;
	}

	if(1 /* (new_surface->field_c = getSDRAMoffset(new_surface->field_8)) == -1 */) {
		/* deallocateOSDMemory(new_surface->field_8); */
		kfree(new_surface);
		return NULL;
	}

	tl880_add_cursor(new_surface);

	return new_surface;
}


void tl880_set_cursor_position(struct tl880_dev *tl880dev, int x, int y)
{
	write_regbits(tl880dev, 0x10104, 0xa, 0, x);
	write_regbits(tl880dev, 0x10104, 0x1a, 0x10, y);
}

void tl880_show_cursor(struct tl880_dev *tl880dev, struct SOverlaySurface *surface, unsigned char state)
{
	if(state) {
		if(!surface) {
			printk(KERN_ERR "tl880: NULL surface in tl880_show_cursor\n");
		} else {
			write_regbits(tl880dev, 0x10100, 0x19, 4, surface->field_c >> 4);
			write_regbits(tl880dev, 0x10108, 9, 8, surface->field_3c);
			write_regbits(tl880dev, 0x10108, 0x19, 0x18, surface->field_40);
			write_regbits(tl880dev, 0x10108, 7, 0, surface->field_14);
			write_regbits(tl880dev, 0x10108, 0x17, 0x10, surface->field_18);
		}
	}
	
	write_regbits(tl880dev, 0x10000, 3, 3, state);
}

struct SOverlaySurface *g_cursor_list = NULL;

void tl880_add_cursor(struct SOverlaySurface *surface)
{
	if(!surface) {
		printk(KERN_ERR "tl880: NULL surface in tl880_add_cursor\n");
		return;
	}

	surface->next = g_cursor_list;
	g_cursor_list = surface;
}

struct OSDmemory {
	unsigned long field_0;
	unsigned long virt_addr;
	unsigned long phys_addr;
	unsigned long size;
	unsigned long active;
	unsigned long id;
	struct OSDmemory *next;
};

unsigned long _g_idTracker_0 = 0;

struct OSDmemory *_g_head = NULL;

/* This function merges any consecutive free areas to a single area */
int tl880_compact_desc_list()
{
	struct OSDmemory *listp1 = _g_head, *listp2;

	listp1 = _g_head;

	/*
loc_3a2a9:
	if(!esi || !(eax = esi->next)) {
		return 1;
	}

	if(esi->active || eax->active) {
		esi = eax;
		goto loc_3a2a9;
	}

	esi->size += eax->size;
	esi->next = eax->next;
	kfree(eax);

	goto loc_3a2a9;
	*/

	while(listp1 && listp1->next) {
		listp2 = listp1->next;
		if(!listp1->active && !listp2->active) {
			listp1->size += listp2->size;
			listp1->next = listp2->next;
			kfree(listp2);
		} else {
			listp1 = listp2;
		}
	}

	return 1;
}

unsigned long tl880_allocate_osd_memory(struct tl880_dev *tl880dev, unsigned long size, unsigned long align)
{
	struct OSDmemory *listp1, *listp2;
	unsigned long retval;
	unsigned long alignsize = align - 1;
	unsigned long mask = ~alignsize;
	unsigned long finalsize = size + alignsize;
	unsigned long active_size = 0, inactive_size = 0;
	
	listp1 = _g_head;

	if(!_g_head) {
		//initializeOSDMemory(sBoardInfo.osd_address, sBoardInfo.virt_addr);
	}

	//compactDescList();

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
			if(!(listp2 = kmalloc(0x1c, 1))) {
				return 0;
			}
			
			listp2->field_0 = 0;
			listp2->virt_addr = 0;
			listp2->phys_addr = 0;
			listp2->size = 0;
			listp2->active = 0;
			listp2->id = ++_g_idTracker_0;
			listp2->next = listp1->next;
			listp1->next = listp2;
			listp2->virt_addr = listp1->virt_addr + finalsize;
			listp2->phys_addr = listp1->phys_addr + finalsize;
			listp2->size = listp1->size - finalsize;
			listp1->size = finalsize;
			listp1->active = 1;
		}
	}

	/* Out of space; count active and inactive memory and return */
	listp2 = _g_head;
	while(listp2) {
		if(listp2->active) {
			active_size += listp2->size;
		} else {
			inactive_size += listp2->size;
		}
		listp2 = listp2->next;
	}
	printk(KERN_WARNING "tl880: out of OSD memory; active bytes: 0x%lx, inactive bytes: 0x%lx\n", active_size, inactive_size);
	return 0;
}


