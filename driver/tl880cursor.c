/* 
 * Cursor overlay related functions for TL880-based cards
 *
 * (c) 2003 Mike Bourgeous <nitrogen@slimetech.com>
 */
#include "tl880.h"

unsigned long _g_idTracker = 0;

void tl880_add_cursor(struct tl880_dev *tl880dev, struct SOverlaySurface *surface);
unsigned long tl880_allocate_osd_memory(struct tl880_dev *tl880dev, unsigned long size, unsigned long align);
unsigned long tl880_get_osdmem_offset(struct tl880_dev *tl880dev, unsigned long addr);

struct SOverlaySurface *tl880_create_cursor(struct tl880_dev *tl880dev, struct _SURFACE_DESC *surface)
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

	if(!(new_surface->field_8 = tl880_allocate_osd_memory(tl880dev, 0x200, 0x10))) {
		kfree(new_surface);
		return NULL;
	}

	if((new_surface->field_c = tl880_get_osdmem_offset(tl880dev, new_surface->field_8)) == -1) {
		/* deallocateOSDMemory(new_surface->field_8); */
		kfree(new_surface);
		return NULL;
	}

	tl880_add_cursor(tl880dev, new_surface);

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
			write_regbits(tl880dev, 0x10100, 0x19, 4, surface->field_c / 16);
			write_regbits(tl880dev, 0x10108, 9, 8, surface->field_3c);
			write_regbits(tl880dev, 0x10108, 0x19, 0x18, surface->field_40);
			write_regbits(tl880dev, 0x10108, 7, 0, surface->field_14);
			write_regbits(tl880dev, 0x10108, 0x17, 0x10, surface->field_18);
		}
	}
	
	write_regbits(tl880dev, 0x10000, 3, 3, state);
}

struct SOverlaySurface *g_cursor_list = NULL;

void tl880_add_cursor(struct tl880_dev *tl880dev, struct SOverlaySurface *cursor)
{
	if(!surface) {
		printk(KERN_ERR "tl880: NULL surface in tl880_add_cursor\n");
		return;
	}

	cursor->next = g_cursor_list;
	g_cursor_list = cursor;
}

void tl880_delete_cursor(struct tl880_dev *tl880dev, struct SOverlaySurface *cursor)
{
	eax = g_cursor_list;
	esi = cursor;

	if(!tl880dev || !cursor) {
		printk(KERN_ERR "tl880: NULL parameter in tl880_delete_cursor\n");
		return;
	}

	if(esi == eax) {
		eax = esi->field_6c;
		g_cursor_list = eax;
		goto loc_3b31f;
	}

	goto loc_3b304;

loc_3b304:
	if(!eax) {
		goto loc_3b33a;
	}

loc_3b308:
	ecx = eax->field_6c;
	if(ecx == esi) {
		goto loc_3b315;
	}

	eax = ecx;
	if(eax) {
		goto loc_3b308;
	}

loc_3b315:
	if(!eax) {
		goto loc_3b33a;
	}

	ecx = esi->field_6c;
	eax->field_6c = ecx;

loc_3b31f:
	eax = esi->field_8;
	esi->field_6c = 0;
	if(!eax) {
		goto loc_3b330;
	}

	/* deallocateOSDMemory(eax); */

loc_3b330:
	esi->field_8 = 0;
	kfree(esi);

loc_3b33a:
	return;
}


