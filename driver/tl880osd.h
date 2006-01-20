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

/*
 * Example values for SOverlaySurface
 *
 * field_0:	01F10000
 * field_4:	001D8000
 * field_8:	01F11BD0
 * field_c:	001D9BD0
 * field_10:	00000000
 * field_14:	00000050
 * field_18:	00000038
 * field_1c:	00000001
 * field_20:	00000001
 * field_24:	00000000
 * field_28:	00000001
 * field_2c:	00000004
 * width:	000000E7
 * height:	00000078
 * field_38:	00000080
 * field_3c:	00000001
 * field_40:	00000001
 * field_44:	00000000
 * field_48:	00000007
 * field_4c:	00000001
 * field_50:	0189CC18
 * field_54:	00000100
 * field_58:	00000008
 * field_5c:	000000E8
 * field_60:	00000000
 * field_64:	00000000
 * id:		52300040
 * next:	00000000
 */

struct SOverlaySurface {
	unsigned long field_0;
	unsigned long field_4;
	unsigned long field_8;	// OSD memory
	unsigned long field_c;	// return value of getSDRAMoffset - location of image in card memory?
	unsigned long field_10;
	unsigned long field_14; // _SURFACE_DESC->field_4
	unsigned long field_18; // _SURFACE_DESC->field_8
	unsigned long field_1c;
	unsigned long field_20;
	unsigned long field_24;
	unsigned long field_28;
	unsigned long field_2c;
	unsigned long width; // _SURFACE_DESC->field_10 // width
	unsigned long height; // _SURFACE_DESC->field_12 // height
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
	unsigned long id;
	struct SOverlaySurface *next;
};

/*
 * Example values for OSDmemory
 *
 * field_0:	00000000
 * virt_addr:	01F10000
 * phys_addr:	001D8000
 * size:	00000517
 * active:	00000001
 * id:		52300001
 * next:	E27A8388
 */


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
int tl880_deallocate_osdmem(struct tl880_dev *tl880dev, unsigned long addr);

/* tl880cursor.c */
struct SOverlaySurface *tl880_create_cursor(struct tl880_dev *tl880dev, struct _SURFACE_DESC *surface);
void tl880_set_cursor_position(struct tl880_dev *tl880dev, int x, int y);
void tl880_show_cursor(struct tl880_dev *tl880dev, struct SOverlaySurface *surface, unsigned char state);
void tl880_add_cursor(struct tl880_dev *tl880dev, struct SOverlaySurface *cursor);
void tl880_delete_cursor(struct tl880_dev *tl880dev, struct SOverlaySurface *cursor);
void tl880_delete_cursor_list(struct tl880_dev *tl880dev);
struct SOverlaySurface *tl880_find_cursor(struct tl880_dev *tl880dev, unsigned long addr);


#endif /* __KERNEL__ */

#endif /* _TL880OSD_H_ */

