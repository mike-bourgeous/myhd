/*
 * tl880fb.c -- Framebuffer driver for the TL880 chip
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/tty.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>

#include "tl880.h"
#include "tl880fb.h"




static struct fb_ops tl880fb_ops;
/*
 *  If your driver supports multiple boards, you should make the  
 *  below data types arrays, or allocate them dynamically (using kmalloc()). 
 */ 

/* 
 * This structure defines the hardware state of the graphics card. Normally
 * you place this in a header file in linux/include/video. This file usually
 * also includes register information. That allows other driver subsystems
 * and userland applications the ability to use the same header file to 
 * avoid duplicate work and easy porting of software. 
 */
//struct tl880fb_par;

/*
 * Here we define the default structs fb_fix_screeninfo and fb_var_screeninfo
 * if we don't use modedb. If we do use modedb see tl880fb_init how to use it
 * to get a fb_var_screeninfo. Otherwise define a default var as well. 
 */
static struct fb_fix_screeninfo tl880fb_fix = {
	.id =		"tl880fb",
	.type =		FB_TYPE_PACKED_PIXELS,
	.visual =	FB_VISUAL_TRUECOLOR,
	.xpanstep =	0,
	.ypanstep =	0,
	.ywrapstep =	0, 
	.accel =	FB_ACCEL_NONE,
};

static struct fb_var_screeninfo tl880fb_var = {
	.xres =		1024,
	.yres =		768,
	.xres_virtual =	1024,
	.yres_virtual =	768,
	.bits_per_pixel = 32,
	.transp = 	{ 24, 7, 0 },
	.red =		{ 16, 8, 0 },
      	.green =	{ 8, 8, 0 },
      	.blue =		{ 0, 8, 0 },
      	.activate =	FB_ACTIVATE_NOW,
      	.height =	-1,
      	.width =	-1,
      	.vmode =	FB_VMODE_NONINTERLACED,
};



	/*
	 * 	Modern graphical hardware not only supports pipelines but some 
	 *  also support multiple monitors where each display can have its  
	 *  its own unique data. In this case each display could be  
	 *  represented by a separate framebuffer device thus a separate 
	 *  struct fb_info. Now the struct tl880fb_par represents the graphics
	 *  hardware state thus only one exist per card. In this case the 
	 *  struct tl880fb_par for each graphics card would be shared between 
	 *  every struct fb_info that represents a framebuffer on that card. 
	 *  This allows when one display changes it video resolution (info->var) 
	 *  the other displays know instantly. Each display can always be
	 *  aware of the entire hardware state that affects it because they share
	 *  the same tl880fb_par struct. The other side of the coin is multiple
	 *  graphics cards that pass data around until it is finally displayed
	 *  on one monitor. Such examples are the voodoo 1 cards and high end
	 *  NUMA graphics servers. For this case we have a bunch of pars, each
	 *  one that represents a graphics state, that belong to one struct 
	 *  fb_info. Their you would want to have *par point to a array of device
	 *  states and have each struct fb_ops function deal with all those 
	 *  states. I hope this covers every possible hardware design. If not
	 *  feel free to send your ideas at jsimmons@users.sf.net 
	 */

	/*
	 *  If your driver supports multiple boards or it supports multiple 
	 *  framebuffers, you should make these arrays, or allocate them 
	 *  dynamically (using kmalloc()). 
	 */ 
static struct fb_info info;

	/* 
	 * Each one represents the state of the hardware. Most hardware have
	 * just one hardware state. These here represent the default state(s). 
	 */
static struct tl880fb_par current_par;

int tl880fb_init(void);
int tl880fb_setup(char*);

/**
 *	tl880fb_open - Optional function. Called when the framebuffer is
 *			 first accessed.
 *	@info: frame buffer structure that represents a single frame buffer
 *	@user: tell us if the userland (value=1) or the console is accessing
 *		   the framebuffer. 
 *
 *	This function is the first function called in the framebuffer api.
 *	Usually you don't need to provide this function. The case where it 
 *	is used is to change from a text mode hardware state to a graphics
 * 	mode state. 
 *
 *	Returns negative errno on error, or zero on success.
 */
static int tl880fb_open(struct fb_info *info, int user)
{
	printk("tl880fb: tl880fb_open - access by %s\n", user ? "userland" : "kernel");
	return 0;
}

/**
 *	tl880fb_release - Optional function. Called when the framebuffer 
 *			device is closed. 
 *	@info: frame buffer structure that represents a single frame buffer
 *	@user: tell us if the userland (value=1) or the console is accessing
 *		   the framebuffer. 
 *	
 *	Thus function is called when we close /dev/fb or the framebuffer 
 *	console system is released. Usually you don't need this function.
 *	The case where it is usually used is to go from a graphics state
 *	to a text mode state.
 *
 *	Returns negative errno on error, or zero on success.
 */
static int tl880fb_release(struct fb_info *info, int user)
{
	printk(KERN_INFO "tl880fb: tl880fb_release - access by %s\n", user ? "userland" : "kernel");
	return 0;
}

/**
 *	  tl880fb_check_var - Optional function. Validates a var passed in. 
 *	  @var: frame buffer variable screen structure
 *	  @info: frame buffer structure that represents a single frame buffer 
 *
 *	Checks to see if the hardware supports the state requested by
 *	var passed in. This function does not alter the hardware state!!! 
 *	This means the data stored in struct fb_info and struct tl880fb_par do 
 *	  not change. This includes the var inside of struct fb_info. 
 *	Do NOT change these. This function can be called on its own if we
 *	intent to only test a mode and not actually set it. The stuff in 
 *	modedb.c is a example of this. If the var passed in is slightly 
 *	off by what the hardware can support then we alter the var PASSED in
 *	to what we can do. If the hardware doesn't support mode change 
 * 	a -EINVAL will be returned by the upper layers. You don't need to 
 *	implement this function then. If you hardware doesn't support 
 *	changing the resolution then this function is not needed. In this
 *	case the driver woudl just provide a var that represents the static
 *	state the screen is in.
 *
 *	Returns negative errno on error, or zero on success.
 */
static int tl880fb_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{
	struct tl880fb_par *par;
	struct tl880_dev *tl880dev;

	printk(KERN_DEBUG "tl880fb: check_var\n");

	return 0;

	if(var != &tl880fb_var) {
		printk(KERN_WARNING "tl880fb: not my var (heheh)\n");
		//return -EINVAL;
	}

	printk(KERN_DEBUG "tl880fb: checking for null var\n");
	
	if(!var) {
		printk(KERN_ERR "tl880fb: null var to tl880fb_check_var\n");
		return -EINVAL;
	}

	printk(KERN_DEBUG "tl880fb: checking for null info\n");
	if(!info) {
		printk(KERN_ERR "tl880fb: null info to tl880fb_check_var\n");
		return -EINVAL;
	}
	
	/*
	if(CHECK_NULL(var) || CHECK_NULL(info) || CHECK_NULL(info->par) || CHECK_NULL(((struct tl880fb_par *)(info->par))->tl880dev)) {
		printk(KERN_ERR "tl880fb: null parameters to tl880fb_check_var\n");
		return -EINVAL;
	}
	*/

	/*
	par = (struct tl880fb_par *)info->par;
	tl880dev = par->tl880dev;
	*/

	printk(KERN_INFO "tl880fb: Checking mode %dx%dx%d for card %i\n", var->xres, var->yres, var->bits_per_pixel, 0/*tl880dev->id*/);

	if(var->xres != var->xres_virtual) {
		//var->xres_virtual = var->xres;
		return -EINVAL;
	}

	if(var->yres != var->yres_virtual) {
		//var->yres_virtual = var->yres;
		return -EINVAL;
	}

	if(var->xoffset) {
		printk(KERN_WARNING "tl880fb: xoffset not supported\n");
		return -EINVAL;
	}

	/* TODO: Implement 24bpp and 8bpp */
	if(var->bits_per_pixel != 32) {
		//var->bits_per_pixel = 32;
		return -EINVAL;
	}
	
	var->xres = (var->xres + 0xf) & ~0xf;

	switch(var->xres)
	{
		case 720:
		case 1024:
		case 1280:
		case 1440:
		case 1920:
			break;
		default:
			printk(KERN_WARNING "tl880fb: xres not supported: %i\n", var->xres);
			//var->xres = 1024;
			return -EINVAL;
			break;
	}

	switch(var->yres)
	{
		case 480:
		case 720:
		case 768:
		case 1080:
			break;
		default:
			printk(KERN_WARNING "tl880fb: yres not supported: %i\n", var->yres);
			//var->yres = 768;
			return -EINVAL;
			break;
	}

	/* 32 bpp */
	var->transp.offset = 24;
	var->transp.length = 7;
	var->red.offset = 16;
	var->red.length = 8;
	var->green.offset = 8;
	var->green.length = 8;
	var->blue.offset = 0;
	var->blue.length = 8;

	var->width = -1;
	var->height = -1;

	var->accel_flags = FB_ACCEL_NONE;
	
	printk(KERN_INFO "tl880fb: Checked mode %dx%dx%d\n", var->xres, var->yres, var->bits_per_pixel);
	
	return 0;	   	
}

/**
 *	  tl880fb_set_par - Optional function. Alters the hardware state.
 *	  @info: frame buffer structure that represents a single frame buffer
 *
 *	Using the fb_var_screeninfo in fb_info we set the resolution of the
 *	this particular framebuffer. This function alters the par AND the
 *	fb_fix_screeninfo stored in fb_info. It doesn't not alter var in 
 *	fb_info since we are using that data. This means we depend on the
 *	data in var inside fb_info to be supported by the hardware. 
 *	tl880fb_check_var is always called before tl880fb_set_par to ensure this.
 *	Again if you can't change the resolution you don't need this function.
 *
 *	Returns negative errno on error, or zero on success.
 */
static int tl880fb_set_par(struct fb_info *info)
{
	struct tl880fb_par *par;
	struct tl880_dev *tl880dev;

	if(!info) {
		printk(KERN_ERR "tl880fb: null info to tl880fb_set_par\n");
	} else if(!info->par) {
		printk(KERN_ERR "tl880fb: null info->par in tl880fb_set_par\n");
	}

	/*
	if(CHECK_NULL(info) || CHECK_NULL(info->par) || CHECK_NULL(((struct tl880fb_par *)(info->par))->tl880dev)) {
		printk(KERN_ERR "tl880fb: null parameters to tl880fb_set_par\n");
		return -EINVAL;
	}
	

	par = (struct tl880fb_par *)info->par;
	tl880dev = par->tl880dev;
	*/

	printk(KERN_DEBUG "tl880fb: tl880fb_set_par on card %i\n", 0/*tl880dev->id*/);
	/* ... */
	/* return -EINVAL; */
	return 0;
}

/**
 *  	tl880fb_setcolreg - Optional function. Sets a color register.
 *	  @regno: Which register in the CLUT we are programming 
 *	  @red: The red value which can be up to 16 bits wide 
 *	@green: The green value which can be up to 16 bits wide 
 *	@blue:  The blue value which can be up to 16 bits wide.
 *	@transp: If supported, the alpha value which can be up to 16 bits wide.
 *	  @info: frame buffer info structure
 * 
 *  	Set a single color register. The values supplied have a 16 bit
 *  	magnitude which needs to be scaled in this function for the hardware. 
 *	Things to take into consideration are how many color registers, if
 *	any, are supported with the current color visual. With truecolor mode
 *	no color palettes are supported. Here a pseudo palette is created
 *	which we store the value in pseudo_palette in struct fb_info. For
 *	pseudocolor mode we have a limited color palette. To deal with this
 *	we can program what color is displayed for a particular pixel value.
 *	DirectColor is similar in that we can program each color field. If
 *	we have a static colormap we don't need to implement this function. 
 * 
 *	Returns negative errno on error, or zero on success.
 */
static int tl880fb_setcolreg(unsigned regno, unsigned red, unsigned green,
			   unsigned blue, unsigned transp,
			   struct fb_info *info)
{
	printk(KERN_INFO "tl880fb: tl880fb_setcolreg\n");

	return 0;
	if (regno >= 256)  /* no. of hw registers */
	   return -EINVAL;
	/*
	 * Program hardware... do anything you want with transp
	 */

	/* grayscale works only partially under directcolor */
	if (info->var.grayscale) {
	   /* grayscale = 0.30*R + 0.59*G + 0.11*B */
	   red = green = blue = (red * 77 + green * 151 + blue * 28) >> 8;
	}

	/* Directcolor:
	 *   var->{color}.offset contains start of bitfield
	 *   var->{color}.length contains length of bitfield
	 *   {hardwarespecific} contains width of DAC
	 *   cmap[X] is programmed to (X << red.offset) | (X << green.offset) | (X << blue.offset)
	 *   RAMDAC[X] is programmed to (red, green, blue)
	 *
	 * Pseudocolor:
	 *	uses offset = 0 && length = DAC register width.
	 *	var->{color}.offset is 0
	 *	var->{color}.length contains widht of DAC
	 *	cmap is not used
	 *	DAC[X] is programmed to (red, green, blue)
	 * Truecolor:
	 *	does not use RAMDAC (usually has 3 of them).
	 *	var->{color}.offset contains start of bitfield
	 *	var->{color}.length contains length of bitfield
	 *	cmap is programmed to (red << red.offset) | (green << green.offset) |
	 *					  (blue << blue.offset) | (transp << transp.offset)
	 *	RAMDAC does not exist
	 */
#define CNVT_TOHW(val,width) ((((val)<<(width))+0x7FFF-(val))>>16)
	switch (info->fix.visual) {
	   case FB_VISUAL_TRUECOLOR:
	   case FB_VISUAL_PSEUDOCOLOR:
			   red = CNVT_TOHW(red, info->var.red.length);
			   green = CNVT_TOHW(green, info->var.green.length);
			   blue = CNVT_TOHW(blue, info->var.blue.length);
			   transp = CNVT_TOHW(transp, info->var.transp.length);
			   break;
	   case FB_VISUAL_DIRECTCOLOR:
		   /* example here assumes 8 bit DAC. Might be different 
		* for your hardware */	
			   red = CNVT_TOHW(red, 8);	   
			   green = CNVT_TOHW(green, 8);
			   blue = CNVT_TOHW(blue, 8);
			   /* hey, there is bug in transp handling... */
			   transp = CNVT_TOHW(transp, 8);
			   break;
	}
#undef CNVT_TOHW
	/* Truecolor has hardware independent palette */
	if (info->fix.visual == FB_VISUAL_TRUECOLOR) {
	   u32 v;

	   if (regno >= 16)
		   return -EINVAL;

	   v = (red << info->var.red.offset) |
		   (green << info->var.green.offset) |
		   (blue << info->var.blue.offset) |
		   (transp << info->var.transp.offset);

	   switch (info->var.bits_per_pixel) {
		case 8:
			/* Yes some hand held devices have this. */ 
		   		((u8*)(info->pseudo_palette))[regno] = v;
			break;	
   		case 16:
		   		((u16*)(info->pseudo_palette))[regno] = v;
			break;
		case 24:
		case 32:	
		   		((u32*)(info->pseudo_palette))[regno] = v;
			break;
	   }
	   return 0;
	}
	/* ... */
	return 0;
}

/**
 *	  tl880fb_pan_display - NOT a required function. Pans the display.
 *	  @var: frame buffer variable screen structure
 *	  @info: frame buffer structure that represents a single frame buffer
 *
 *	Pan (or wrap, depending on the `vmode' field) the display using the
 *  	`xoffset' and `yoffset' fields of the `var' structure.
 *  	If the values don't fit, return -EINVAL.
 *
 *	  Returns negative errno on error, or zero on success.
 */
static int tl880fb_pan_display(struct fb_var_screeninfo *var,
				 struct fb_info *info)
{
	/* ... */
	return -EINVAL;
}

/**
 *	  tl880fb_blank - NOT a required function. Blanks the display.
 *	  @blank_mode: the blank mode we want. 
 *	  @info: frame buffer structure that represents a single frame buffer
 *
 *	  Blank the screen if blank_mode != 0, else unblank. Return 0 if
 *	  blanking succeeded, != 0 if un-/blanking failed due to e.g. a 
 *	  video mode which doesn't support it. Implements VESA suspend
 *	  and powerdown modes on hardware that supports disabling hsync/vsync:
 *	  blank_mode == 2: suspend vsync
 *	  blank_mode == 3: suspend hsync
 *	  blank_mode == 4: powerdown
 *
 *	  Returns negative errno on error, or zero on success.
 *
 */
static int tl880fb_blank(int blank_mode, struct fb_info *info)
{
	struct tl880fb_par *par = (struct tl880fb_par *) info->par;
	struct tl880_dev *tl880dev = par->tl880dev;

	printk(KERN_DEBUG "tl880fb: tl880fb_blank\n");

	tl880_write_register(tl880dev, 0x10000, 0x20);
	return 0;
}

/* ------------ Accelerated Functions --------------------- */

/*
 * We provide our own functions if we have hardware acceleration
 * or non packed pixel format layouts. If we have no hardware 
 * acceleration, we can use a generic unaccelerated function. If using
 * a pack pixel format just use the functions in cfb_*.c. Each file 
 * has one of the three different accel functions we support.
 */

/**
 *	  tl880fb_fillrect - REQUIRED function. Can use generic routines if 
 *		 	 non acclerated hardware and packed pixel based.
 *			 Draws a rectangle on the screen.		
 *
 *	  @info: frame buffer structure that represents a single frame buffer
 *	@region: The structure representing the rectangular region we 
 *		 wish to draw to.
 *
 *	This drawing operation places/removes a retangle on the screen 
 *	depending on the rastering operation with the value of color which
 *	is in the current color depth format.
 */
void tl880fb_fillrect(struct fb_info *p, const struct fb_fillrect *region)
{
/*	Meaning of struct fb_fillrect
 *
 *	@dx: The x and y corrdinates of the upper left hand corner of the 
 *	@dy: area we want to draw to. 
 *	@width: How wide the rectangle is we want to draw.
 *	@height: How tall the rectangle is we want to draw.
 *	@color:	The color to fill in the rectangle with. 
 *	@rop: The raster operation. We can draw the rectangle with a COPY
 *		  of XOR which provides erasing effect. 
 */

	printk(KERN_DEBUG "tl880fb: tl880fb_fillrect\n");
	cfb_fillrect(p, region);
}

/**
 *	  tl880fb_copyarea - REQUIRED function. Can use generic routines if
 *					   non acclerated hardware and packed pixel based.
 *					   Copies one area of the screen to another area.
 *
 *	  @info: frame buffer structure that represents a single frame buffer
 *	  @area: Structure providing the data to copy the framebuffer contents
 *		   from one region to another.
 *
 *	  This drawing operation copies a rectangular area from one area of the
 *	screen to another area.
 */
void tl880fb_copyarea(struct fb_info *p, const struct fb_copyarea *area) 
{
/*
 *	  @dx: The x and y coordinates of the upper left hand corner of the
 *	@dy: destination area on the screen.
 *	  @width: How wide the rectangle is we want to copy.
 *	  @height: How tall the rectangle is we want to copy.
 *	  @sx: The x and y coordinates of the upper left hand corner of the
 *	  @sy: source area on the screen.
 */

	printk(KERN_DEBUG "tl880fb: tl880fb_copyarea\n");
	cfb_copyarea(p, area);
}


/**
 *	  tl880fb_imageblit - REQUIRED function. Can use generic routines if
 *						non acclerated hardware and packed pixel based.
 *						Copies a image from system memory to the screen. 
 *
 *	  @info: frame buffer structure that represents a single frame buffer
 *	@image:	structure defining the image.
 *
 *	  This drawing operation draws a image on the screen. It can be a 
 *	mono image (needed for font handling) or a color image (needed for
 *	tux). 
 */
void tl880fb_imageblit(struct fb_info *p, const struct fb_image *image) 
{
/*
 *	  @dx: The x and y coordinates of the upper left hand corner of the
 *	@dy: destination area to place the image on the screen.
 *	  @width: How wide the image is we want to copy.
 *	  @height: How tall the image is we want to copy.
 *	  @fg_color: For mono bitmap images this is color data for	 
 *	  @bg_color: the foreground and background of the image to
 *		   write directly to the frmaebuffer.
 *	@depth:	How many bits represent a single pixel for this image.
 *	@data: The actual data used to construct the image on the display.
 *	@cmap: The colormap used for color images.   
 */

	printk(KERN_DEBUG "tl880fb: tl880fb_imageblit\n");
	cfb_imageblit(p, image);
}

/**
 *	tl880fb_cursor - 	OPTIONAL. If your hardware lacks support
 *			for a cursor, leave this field NULL.
 *
 *	  @info: frame buffer structure that represents a single frame buffer
 *	@cursor: structure defining the cursor to draw.
 *
 *	  This operation is used to set or alter the properities of the
 *	cursor.
 *
 *	Returns negative errno on error, or zero on success.
 */
int tl880fb_cursor(struct fb_info *info, struct fb_cursor *cursor)
{
/*
 *	  @set: 	Which fields we are altering in struct fb_cursor 
 *	@enable: Disable or enable the cursor 
 *	  @rop: 	The bit operation we want to do. 
 *	  @mask:  This is the cursor mask bitmap. 
 *	  @dest:  A image of the area we are going to display the cursor.
 *		Used internally by the driver.	 
 *	  @hot:	The hot spot. 
 *	@image:	The actual data for the cursor image.
 *
 *	  NOTES ON FLAGS (cursor->set):
 *
 *	  FB_CUR_SETIMAGE - the cursor image has changed (cursor->image.data)
 *	  FB_CUR_SETPOS   - the cursor position has changed (cursor->image.dx|dy)
 *	  FB_CUR_SETHOT   - the cursor hot spot has changed (cursor->hot.dx|dy)
 *	  FB_CUR_SETCMAP  - the cursor colors has changed (cursor->fg_color|bg_color)
 *	  FB_CUR_SETSHAPE - the cursor bitmask has changed (cursor->mask)
 *	  FB_CUR_SETSIZE  - the cursor size has changed (cursor->width|height)
 *	  FB_CUR_SETALL   - everything has changed
 *
 *	  NOTES ON ROPs (cursor->rop, Raster Operation)
 *
 *	  ROP_XOR		 - cursor->image.data XOR cursor->mask
 *	  ROP_COPY		- curosr->image.data AND cursor->mask
 *
 *	  OTHER NOTES:
 *
 *	  - fbcon only supports a 2-color cursor (cursor->image.depth = 1)
 *	  - The fb_cursor structure, @cursor, _will_ always contain valid
 *		fields, whether any particular bitfields in cursor->set is set
 *		or not.
 */

	struct tl880fb_par *par = (struct tl880fb_par *) info->par;
	struct tl880_dev *tl880dev = par->tl880dev;

	printk(KERN_INFO "tl880fb: tl880fb_cursor on tl880dev %i\n", tl880dev->id);

	return 0;
}

/**
 *	tl880fb_rotate -  NOT a required function. If your hardware
 *			supports rotation the whole screen then 
 *			you would provide a hook for this. 
 *
 *	  @info: frame buffer structure that represents a single frame buffer
 *	@angle: The angle we rotate the screen.   
 *
 *	  This operation is used to set or alter the properities of the
 *	cursor.
 */
void tl880fb_rotate(struct fb_info *info, int angle)
{
	/* Not possible to implement */
}

/**
 *	tl880fb_poll - NOT a required function. The purpose of this
 *			 function is to provide a way for some process
 *			 to wait until a specific hardware event occurs
 *			 for the framebuffer device.
 * 				 
 *	  @info: frame buffer structure that represents a single frame buffer
 *	@wait: poll table where we store process that await a event.	 
 */
void tl880fb_poll(struct fb_info *info, poll_table *wait)
{
	/* Not yet implemented */
}

/**
 *	tl880fb_sync - NOT a required function. Normally the accel engine 
 *			 for a graphics card take a specific amount of time.
 *			 Often we have to wait for the accelerator to finish
 *			 its operation before we can write to the framebuffer
 *			 so we can have consistent display output. 
 *
 *	  @info: frame buffer structure that represents a single frame buffer
 */
void tl880fb_sync(struct fb_info *info)
{
}

	/*
	 *  Initialization
	 */

void set_aux_horizontal_scaler(int width, int a)
{
	u32 value;

	value  = (((((a << 8) / width) & 0xff) * width + (a * 0xf00)) << 0x14) & 0xfff00000;
	value |= ((((((a << 8) / width) & 0xff) + 1) * width - (a << 8)) << 8) & 0x000fff00;
	value |= 					    ((a << 8) / width) & 0x000000ff;
	tl880_write_register(current_par.tl880dev, 0x100c8, value);
	tl880_write_register(current_par.tl880dev, 0x100d0, (0x6b3 - a * 2) | 0x75a000);
}

void set_aux_vertical_scaler(u32 height, u32 a, u32 interlace)
{
	u32 value;

	value  = (((((a << 8) / height) & 0xff) * height + (a * 0xf00)) << 0x14) & 0xfff00000;
	value |= ((((((a << 8) / height) & 0xff) + 1) * height - (a << 8)) << 8) & 0x000fff00;
	value |= 					     ((a << 8) / height) & 0x000000ff;

	tl880_write_register(current_par.tl880dev, 0x100cc, value);
	value = tl880_read_register(current_par.tl880dev, 0x100c0);

	while(value != (value = tl880_read_register(current_par.tl880dev, 0x100c0)));

	value = interlace ? 2 : 0;

	tl880_write_register(current_par.tl880dev, 0x100c0, (((a << 8) / height) << 8) | value | height);
	tl880_write_register(current_par.tl880dev, 0x100d4, 0x1050020d | (0x41000 - (a << 0xa)));
	tl880_write_register(current_par.tl880dev, 0x100d8, 0x20b00107 | (0x82c00 - (a << 0xa)));
}


void set_aux_picture_origin(u32 x, u32 y)
{
	tl880_write_register(current_par.tl880dev, 0x100c4, x | (y << 0xc));
}

u32 test_aux_dscr_mbox_empty(void)
{
	return (tl880_read_register(current_par.tl880dev, 0x25728) ? 0 : 1);
}

u32 test_aux_current_line_count(void)
{
	return tl880_read_register(current_par.tl880dev, 0x2573c);
}

void aux_reset(void)
{
	int dbg_me = 0, dbg_lc = 0;

	while(!test_aux_dscr_mbox_empty()) {
		tl880_write_register(current_par.tl880dev, 0x25728, 0);
		dbg_me++;
	}
	while(test_aux_current_line_count()) {
		tl880_write_register(current_par.tl880dev, 0x2573c, 0);
		dbg_lc++;
	}
	tl880_write_register(current_par.tl880dev, 0x25744, 1);
}



void set_aux_1024x768p(int srcw, int srch, int full, int component_video)
{
	u32 value = 0;
	long reg = 0x100c0;

	set_bits(&value, reg, 0, 0, 0);
	set_bits(&value, reg, 1, 1, 0);
	set_bits(&value, reg, 2, 2, component_video ? 0 : 1);
	set_bits(&value, reg, 3, 3, 0);
	set_bits(&value, reg, 4, 4, 1);
	set_bits(&value, reg, 0xf, 8, 0);
	set_bits(&value, reg, 0x1b, 0x10, 0x2d1);	/* 721 */

	tl880_write_register(current_par.tl880dev, reg, value);

	set_aux_picture_origin(0, 2);
	set_aux_horizontal_scaler(0x400, srcw);	/* 1024 */
	set_aux_vertical_scaler(0x300, srch, 0);	/* 768 */
}


void set_sync_1024x768p(int invert_hsync, int invert_vsync)
{
	u32 value = 0;
	long reg;
	
	reg = 0x10014;
	set_bits(&value, reg, 0, 0, 0);			/* A - 0 */
	set_bits(&value, reg, 1, 1, 1);			/* B - 1 */
	set_bits(&value, reg, 2, 2, 1);			/* C - 1 */
	set_bits(&value, reg, 3, 3, 1);			/* D - 1 */
	set_bits(&value, reg, 0xf, 6, 0x21c);		/* E - 540 */
	set_bits(&value, reg, 0x1a, 0x10, 0x400);	/* F - 1024 */
	set_bits(&value, reg, 0x1b, 0x1b, 1);		/* G - 1 */
	set_bits(&value, reg, 0x1c, 0x1c, 1);		/* H - 1 */
	set_bits(&value, reg, 0x1d, 0x1d, 0);		/* I - 0 */
	set_bits(&value, reg, 0x1e, 0x1e, 0);		/* J - 0 */
	tl880_write_register(current_par.tl880dev, reg, value);

	reg = 0x10018;
	value = 0;
	set_bits(&value, reg, 8, 0, 0x19);		/* K - 31 */
	set_bits(&value, reg, 9, 9, invert_hsync ? 1 : 0); /* L - 0 */
	set_bits(&value, reg, 0xa, 0xa, invert_vsync ? 1 : 0); /* M - 0 */
	set_bits(&value, reg, 0x12, 0xc, 0x7f);		/* N - 127 */
	set_bits(&value, reg, 0x1c, 0x14, 0xa0);	/* O - 160 */

	tl880_write_register(current_par.tl880dev, reg, value);

	reg = 0x1001c;
	value = 0;
	set_bits(&value, reg, 2, 0, 6);			/* P - 6 */
	set_bits(&value, reg, 0xb, 4, 0x1d);		/* Q - 29 */
	set_bits(&value, reg, 0x16, 0xc, 0x300);	/* R - 768 */
	set_bits(&value, reg, 0x1b, 0x18, 3);		/* S - 3 */
	set_bits(&value, reg, 0x1c, 0x1c, 0);		/* T - 0 */

	tl880_write_register(current_par.tl880dev, reg, value);

	reg = 0x10020;
	value = 0;
	set_bits(&value, reg, 2, 0, 0);			/* U - 0 */
	set_bits(&value, reg, 0xb, 4, 0);		/* V - 0 */
	set_bits(&value, reg, 0x16, 0xc, 0);		/* W - 0 */
	set_bits(&value, reg, 0x1b, 0x18, 0);		/* X - 0 */

	tl880_write_register(current_par.tl880dev, reg, value);
	
	reg = 0x10028;
	value = 0;
	set_bits(&value, reg, 0xb, 0, 0x28);		/* BB - 40 */
	set_bits(&value, reg, 0x18, 0x18, 1);		/* CC - 1 */

	tl880_write_register(current_par.tl880dev, reg, value);
	
	reg = 0x10024;
	value = 0;
	set_bits(&value, reg, 0x15, 0, 5);		/* Y - 5 */
	set_bits(&value, reg, 0x18, 0x18, 1);		/* Z - 1 */
	set_bits(&value, reg, 0x1f, 0x1c, 4);		/* AA - 4 */

	tl880_write_register(current_par.tl880dev, reg, value);
}



int __init tl880fb_init(void)
{
	int cmap_len = 256, retval;	
   
	/*
	 *  For kernel boot options (in 'video=tl880fb:<options>' format)
	 */
#ifndef MODULE
	char *option = NULL;

	if (fb_get_options("tl880fb", &option))
		return -ENODEV;
	tl880fb_setup(option);
#endif

	printk(KERN_INFO "tl880fb: tl880fb_init\n");

	request_module("tl880");

	/*
	 * Set up flags to indicate what sort of acceleration your
	 * driver can provide (pan/wrap/copyarea/etc.) and whether it
	 * is a module -- see FBINFO_* in include/linux/fb.h
	 */
	info.flags = FBINFO_DEFAULT;

	current_par.tl880dev = find_tl880(0);
	info.par = &current_par;

	/* 
	 * Here we set the screen_base to the virtual memory address
	 * for the framebuffer. Usually we obtain the resource address
	 * from the bus layer and then translate it to virtual memory
	 * space via ioremap. Consult ioport.h. 
	 */
	info.screen_base = current_par.tl880dev->memspace + 0x300000;
	info.screen_size = 1024*768*4;
	info.fbops = &tl880fb_ops;
	info.fix = tl880fb_fix;
	info.var = tl880fb_var;

	printk(KERN_INFO "tl880fb: memphys is 0x%08lx, memspace is 0x%08lx, memlen is 0x%08lx\n", 
			current_par.tl880dev->memphys, (unsigned long)current_par.tl880dev->memspace, current_par.tl880dev->memlen);

	info.fix.smem_start = current_par.tl880dev->memphys + 0x300000;
	info.fix.smem_len = 1024*768*4;
	info.fix.mmio_start = current_par.tl880dev->regphys;
	info.fix.mmio_len = current_par.tl880dev->reglen;

	//info.pseudo_palette = pseudo_palette; /* we don't use a pseudo_palette (yet?) */

	/*
	 * This should give a reasonable default video mode. The following is
	 * done when we can set a video mode. 
	 */
	/*
	if (!mode_option) 
		mode_option = "1024x768-32@60";
	//retval = fb_find_mode(&info.var, &info, "1024x768-24@60", NULL, 0, NULL, 24);
	if (!retval || retval == 4)
		return -EINVAL;			
	*/

	/* This has to be done !!! */	
	fb_alloc_cmap(&info.cmap, cmap_len, 8);
	
	info.var.bits_per_pixel = 32;
  
	info.fix.line_length = info.var.xres * ((info.var.bits_per_pixel + 7) / 8);


	if (register_framebuffer(&info) < 0) {
		fb_dealloc_cmap(&info.cmap);
		return -EINVAL;
	}

	printk(KERN_INFO "fb%d: %s frame buffer device\n", info.node, info.fix.id);

	/* Set video mode */
	aux_reset();
	set_sync_1024x768p(1, 1);
	set_aux_1024x768p(720, 480, 1, 1);

	/* Turn off TL880 OSD */
	tl880_write_regbits(current_par.tl880dev, 0x10000, 2, 2, 0);

	/* Install OSD display list */
	tl880_write_memory(current_par.tl880dev, 0x2d8000, 0x60100040);
	tl880_write_memory(current_par.tl880dev, 0x2d8004, 0x70000000);
	tl880_write_memory(current_par.tl880dev, 0x2d8008, 0x80300000);
	tl880_write_memory(current_par.tl880dev, 0x2d800c, 0x14000000);
	
	/* Make sure colors are normal */
	tl880_write_register(current_par.tl880dev, 0x10080, 0x0);
	
	/* Set OSD display list position */
	tl880_write_register(current_par.tl880dev, 0x10084, 0x2d8000);

	/* Set OSD brightness to its default value */
	tl880_write_register(current_par.tl880dev, 0x10094, 0x8000);

	/* Clear the framebuffer */
	memset(info.screen_base, 0, info.screen_size);

	/* Turn on TL880 OSD */
	tl880_write_regbits(current_par.tl880dev, 0x10000, 2, 2, 1);

	/* Turn on video output (is this MyHD specific?) */
	tl880_write_regbits(current_par.tl880dev, 0x10000, 7, 7, 0);
	tl880_write_regbits(current_par.tl880dev, 0x10000, 5, 5, 1);
	tl880_write_regbits(current_par.tl880dev, 0x10190, 0xa, 0xa, 1);
	tl880_write_regbits(current_par.tl880dev, 0x10194, 0xa, 0xa, 1);
	tl880_write_regbits(current_par.tl880dev, 0x10198, 0xa, 0xa, 1);

	return 0;
}

	/*
	 *  Cleanup
	 */

static void __exit tl880fb_cleanup(void)
{
	/*
	 *  If your driver supports multiple boards, you should unregister and
	 *  clean up all instances.
	 */

	struct tl880_dev *tl880dev = find_tl880(0);

	printk(KERN_INFO "tl880fb: tl880fb_cleanup\n");

	/*
	if(tl880dev) {
		tl880_write_regbits(tl880dev, 0x10000, 2, 2, 0);
		tl880_write_regbits(tl880dev, 0x10190, 0xa, 0xa, 1);
		tl880_write_regbits(tl880dev, 0x10194, 0xa, 0xa, 1);
		tl880_write_regbits(tl880dev, 0x10198, 0xa, 0xa, 1);
	}
	*/

	unregister_framebuffer(&info);
	fb_dealloc_cmap(&info.cmap);
	/* ... */
}

	/*
	 *  Setup
	 */

/* 
 * Only necessary if your driver takes special options,
 * otherwise we fall back on the generic fb_setup().
 */
int tl880fb_setup(char *options)
{
	/* Parse user speficied options (`video=tl880fb:') */
	return 0;
}


/* Memory map function - called by the kernel when a userspace app mmaps our device(s) in /dev */
static int tl880fb_mmap(struct fb_info *info, struct file *file, struct vm_area_struct *vma)
{
	u32 start = 0;
	u32 length = 0;
	struct tl880_dev *tl880dev;
	u32 offset;
	struct tl880fb_par *par;


	/* Make sure all the expected data is present */
	/*
	if(CHECK_NULL(info) || CHECK_NULL(info->par) || CHECK_NULL(file) || CHECK_NULL(vma)) {
		return -EINVAL;
	}
	*/
	
	offset = vma->vm_pgoff << PAGE_SHIFT;
	start = (info->fix.smem_start & PAGE_MASK) + offset;
	length = PAGE_ALIGN((info->fix.smem_len & ~PAGE_MASK) + info->fix.smem_len - offset);

	printk(KERN_DEBUG "tl880fb: tl880fb_mmap: start: 0x%08x, length: 0x%08x, offset: 0x%08x\n", start, length, offset);

	par = (struct tl880fb_par *)info->par;
	tl880dev = par->tl880dev;

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	vma->vm_flags |= VM_RESERVED;
	vma->vm_flags |= VM_IO;

	if(remap_pfn_range(vma, vma->vm_start, start >> PAGE_SHIFT, vma->vm_end - vma->vm_start, vma->vm_page_prot)) {
		printk(KERN_ERR "tl880: remap_pfn_range call failed\n");
		return -EAGAIN;
	}

	return 0;
}



/* ------------------------------------------------------------------------- */

	/*
	 *  Frame buffer operations
	 */

static struct fb_ops tl880fb_ops = {
	.owner		= THIS_MODULE,
	.fb_open	= tl880fb_open,
	.fb_release	= tl880fb_release,
	//.fb_read	= tl880fb_read,
	//.fb_write	= tl880fb_write,
	.fb_check_var	= tl880fb_check_var,
	.fb_set_par	= tl880fb_set_par,	
	.fb_setcolreg	= tl880fb_setcolreg,
	.fb_blank	= tl880fb_blank,
	//.fb_pan_display	= tl880fb_pan_display,	
	.fb_fillrect	= cfb_fillrect, 	/* Needed !!! */ 
	.fb_copyarea	= cfb_copyarea,		/* Needed !!! */ 
	.fb_imageblit	= cfb_imageblit,	/* Needed !!! */
	//.fb_cursor	= tl880fb_cursor,	/* Optional !!! */
	//.fb_ioctl	= tl880fb_ioctl,
	//.fb_mmap	= tl880fb_mmap,	
};


/* ------------------------------------------------------------------------- */


	/*
	 *  Modularization
	 */

module_init(tl880fb_init);
module_exit(tl880fb_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mike Bourgeous <i_am_nitrogen at hotmail.com>");
MODULE_DESCRIPTION("TL880 framebuffer device driver");

