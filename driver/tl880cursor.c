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

	if(!(new_surface = kmalloc(0x70, 1))) { // 112
		return NULL;
	}

	memset(new_surface, 0, 0x70);

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
	unsigned long field_4;
	unsigned long field_8;
	unsigned long field_c;
	unsigned long active;
	unsigned long id;
	struct OSDmemory *next;
};

unsigned long _g_idTracker_0 = 0;

struct OSDmemory *_g_head = NULL;

unsigned long tl880_allocate_osd_memory(struct tl880_dev *tl880dev, unsigned long size, unsigned long align)
{
	struct OSDmemory *listp1, *listp2;
	unsigned long ecx, edx, edi, retval;
	unsigned long alignsize = align - 1;
	unsigned long mask = ~alignsize;
	unsigned long finalsize = size + alignsize;
	
	listp1 = _g_head;

	if(!_g_head) {
		//initializeOSDMemory(sBoardInfo.osd_address, sBoardInfo.field_4);
	}

	//compactDescList();

	edi = size + align - 1;

	/* Search for an inactive block of the correct size */
	listp1 = _g_head;
	while(listp1 != NULL) {
		if(!listp1->active && listp1->field_c == finalsize) {
			listp1->active = 1;
			retval = (alignsize + listp1->field_4) & mask;
			listp1->field_0 = retval - listp1->field_4;
			
			return retval;
		}
		listp1 = listp1->next;
	}

	/*
loc_3a4a5:
	if(listp1 == NULL) {
		goto loc_3a4c4;
	}

	if(listp1->active) {
		goto loc_3a4b3;
loc_3a4b3:
		listp1 = listp1->next;
		goto loc_3a4a5;
	}

	if(listp1->field_c == size + align - 1) {
		goto loc_3a4b8;
loc_3a4b8:
		listp1->active = 1;
		goto loc_3a56b;
	}
	*/

loc_3a4c4:
	listp2 = _g_head;
	listp1 = _g_head;

	/*
	if(!_g_head) {
		goto loc_3a4e0;
	}

loc_3a4cf:
	if(listp1->active || listp1->field_c <= finalsize) {
		goto loc_3a4d9;
loc_3a4d9:
		listp1 = listp1->next;
		if(listp1) {
			goto loc_3a4cf;
		}
		goto loc_3a4e0;
	}

	if(listp1->field_c > (size + align - 1)) {
		goto loc_3a4fd;
	}
	*/


loc_3a4e0:
	edx = 0;
	ecx = 0;

	/* why does this loop return 0 after finding the end of the list? */
	/* 
	 * equivalent asm:
	 * loc_3A4E4:                              ; CODE XREF: allocateOSDMemory_Aligned(ulong,EALIGN)+115j
	 * 	cmp     eax, ebx        ; ebx is 0
	 * 	jz      loc_3A58E       ; loc_3a58e sets eax to 0 then returns
	 * 	
	 * 	cmp     [eax+10h], ebx  ; Compare Two Operands
	 * 	jz      loc_3A583       ; Jump if Zero (ZF=1)
	 * 	
	 * 	add     edx, [eax+0Ch]  ; Add
	 * 	jmp     loc_3A586       ; Jump
	 * loc_3A583:                              ; CODE XREF: allocateOSDMemory_Aligned(ulong,EALIGN)+7Bj
	 * 	add     ecx, [eax+0Ch]  ; Add
	 * 	
	 * loc_3A586:                              ; CODE XREF: allocateOSDMemory_Aligned(ulong,EALIGN)+84j
	 * 	mov     eax, [eax+18h]
	 * 	jmp     loc_3A4E4       ; Jump
	 */
loc_3a4e4:
	if(!listp2) {
		return 0;
	}

	if(listp2->active == 0) {
		goto loc_3a583;
loc_3a583:
		ecx += listp2->field_c;
		goto loc_3a586;
	}

	edx += listp2->field_c;
	goto loc_3a586;
loc_3a586:
	listp2 = listp2->next;
	goto loc_3a4e4;

loc_3a4fd:
	if(!(listp2 = kmalloc(1, 0x1c))) {
		return 0;
	}

	listp2->field_0 = 0;
	listp2->field_4 = 0;
	listp2->field_8 = 0;
	listp2->field_c = 0;
	listp2->active = 0;
	listp2->id = ++_g_idTracker_0;
	listp2->next = 0;
	listp2->next = listp1->next;
	listp1->next = listp2;
	listp2->active = 0;
	listp2->field_c = listp1->field_c - (size + align - 1);
	listp2->field_4 = listp1->field_4 + (size + align - 1);
	listp2->field_8 = listp1->field_8 + (size + align - 1);
	listp1->field_c = (size + align - 1);
	listp1->active = 1;
	
loc_3a56b:
	edx = listp1->field_4;
	ecx = ~(align - 1);
	retval = ((align - 1) + edx) & ~(align - 1);
	listp1->field_0 = retval - listp1->field_4;

	return retval;
}


