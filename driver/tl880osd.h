/* 
 * Overlay related declarations for TL880-based cards
 *
 * (c) 2003 Mike Bourgeous <nitrogen@slimetech.com>
 */

#ifndef _TL880OSD_H_
#define _TL880OSD_H_

#ifdef __KERNEL__

/*** Driver definitions ***/

/*** Driver types ***/
struct _SURFACE_DESC {
	unsigned long field_0;		// Used with findCursor
	unsigned long field_4;		// Used with WriteOSDBlock
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
	unsigned long field_8;	// OSD memory
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

struct OSDmemory {
	unsigned long field_0;
	unsigned long virt_addr;
	unsigned long phys_addr;
	unsigned long size;
	unsigned long active;
	unsigned long id;
	struct OSDmemory *next;
};


#endif /* __KERNEL__ */


/*** Userspace ioctls ***/


#ifdef __KERNEL__

/*** Driver variables ***/
extern unsigned long _g_idTracker_0;
extern struct OSDmemory *_g_head;


/*** Driver functions ***/

/* tl880osd.c */
int tl880_compact_osdmem(struct tl880_dev *tl880dev);
unsigned long tl880_allocate_osdmem(struct tl880_dev *tl880dev, unsigned long size, unsigned long align);
unsigned long tl880_get_osdmem_offset(struct tl880_dev *tl880dev, unsigned long addr);

/* tl880cursor.c */

#endif /* __KERNEL__ */

#endif /* _TL880OSD_H_ */

