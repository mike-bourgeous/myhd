/* 
 * tl880kern.c - Teralogic 880 based HDTV cards - core kernel routines
 *
 * Copyright (C) 2003-2007 Mike Bourgeous <nitrogen at users.sourceforge.net>
 *
 * Reverse engineering, development:
 *   Mike Bourgeous <nitrogen at users.sourceforge.net>
 *   
 * Original driver framework (based on Stradis driver, mostly gone):
 *   Mark Lehrer <mark at knm.org>
 *
 * See the AUTHORS and THANKS files for other credits.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Log: tl880kern.c,v $
 * Revision 1.31  2008/06/28 02:12:40  nitrogen
 * Importing old changes.  See ChangeLog.
 *
 * Revision 1.30  2007/09/09 06:16:48  nitrogen
 * Started an ALSA driver.  New iocread4reg tool.  Driver enhancements.
 *
 * Revision 1.29  2007/09/08 09:20:34  nitrogen
 * Fixed memory pool allocation.
 *
 * Revision 1.28  2007/09/06 05:22:05  nitrogen
 * Improvements to audio support, documentation, and card memory management.
 *
 * Revision 1.27  2007/04/24 06:32:13  nitrogen
 * Changed most int/long types to explicit 32-bit sizes.  Fixed compilation and execution on 64-bit CPUs.
 *
 * Revision 1.26  2007/03/29 09:27:40  nitrogen
 * Tweaked mkdev scripts, improved MSP init, new tool, improved tools makefile, more docs.
 *
 * Revision 1.25  2007/03/29 09:01:20  nitrogen
 * Partial MSP init now working, with correct sequence (after MSP3400 I2C attach)
 *
 * Revision 1.24  2007/03/28 08:01:30  nitrogen
 * Initialization improvements, VPX improvements, minor comment and error message tweaks, better docs
 *
 * Revision 1.23  2007/03/26 20:00:26  nitrogen
 * Fixed interrupt info printk.
 *
 * Revision 1.22  2007/03/26 19:25:06  nitrogen
 * Added CVS log generation and updated copyrights and e-mail addresses.
 *
 */

#include "tl880.h"

#define DEBUG(x) 		/* debug driver */
#undef  IDEBUG	 		/* debug irq handler */
#undef  MDEBUG	 		/* debug memory management */

/* Driver global variables */
int debug;
int tl_major = 0;

/*****/
/* Local variables */
int n_tl880s = 0;
struct tl880_dev *tl880_list = NULL;
dev_t device_number;			/* Major/minor number */
/*****/


/* Supported PCI Devices */
static struct pci_device_id tl880_ids[] = {
	{ PCI_DEVICE(PCI_VENDOR_ID_TERALOGIC, PCI_DEVICE_ID_TERALOGIC_TL880) },
	{ 0, },
};
MODULE_DEVICE_TABLE(pci, tl880_ids);

/* Kernel PCI driver info */
int tl880_probe(struct pci_dev *dev, const struct pci_device_id *id);
void tl880_remove(struct pci_dev *dev);
static struct pci_driver tl880_pcidriver = {
	.name = "tl880",
	.id_table = tl880_ids,
	.probe = tl880_probe,
	.remove = tl880_remove
};

/* Module parameters */
module_param(debug, int, 0);
MODULE_PARM_DESC(debug, "Set TL880 driver debug level (1 enables interrupt debugging)");

module_param(tl_major, int, 0);
MODULE_PARM_DESC(tl_major, "TL880 device major number - 0 or unset for automatic");

/* Other local static functions */
static struct tl880_dev *tl880_unconfigure(struct tl880_dev *tl880dev);

/* Module information for modinfo */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mike Bourgeous http://myhd.sf.net/");
MODULE_DESCRIPTION("Kernel driver for TL880-based HDTV cards");


#ifdef DEBUG
int tl880_driver(void)	/* Something for ksymoops? */
{
	return 1;
}
#endif

static void tl880_log_status(void)
{
	struct tl880_dev *list = tl880_list;
	printk(KERN_INFO "tl880: VIDIOC_LOG_STATUS:\n");
	printk(KERN_INFO "tl880: \tNumber of cards: %d\n", n_tl880s);
	while(list) {
		printk(KERN_INFO "tl880: \tCard %d:\n", list->id);
	       	printk(KERN_INFO "tl880: \t  Model: %s\n", list->name);
		printk(KERN_INFO "tl880: \t  Type ID: %x\n", list->card_type);
		printk(KERN_INFO "tl880: \t  PCI Subsystem: %04x:%04x\n", 
				list->subsys_vendor_id, list->subsys_device_id);
		printk(KERN_INFO "tl880: \t  Bus slot: %s\n", list->pcidev->dev.bus_id);
		printk(KERN_INFO "tl880: \t  IRQ: %d\n", list->irq);
		printk(KERN_INFO "tl880: \t  Interrupt line appears to be %sshared\n", 
				list->elseint ? "" : "un");
		printk(KERN_INFO "tl880: \t  Interrupts processed:\n");
		printk(KERN_INFO "tl880: \t    VSC:   %3u\n", list->vsc_count);
		printk(KERN_INFO "tl880: \t    APU:   %3u\n", list->apu_count);
		printk(KERN_INFO "tl880: \t    BLT:   %3u\n", list->blt_count);
		printk(KERN_INFO "tl880: \t    MCE:   %3u\n", list->mce_count);
		printk(KERN_INFO "tl880: \t    MCU:   %3u\n", list->mcu_count);
		printk(KERN_INFO "tl880: \t    VPIP:  %3u\n", list->vpip_count);
		printk(KERN_INFO "tl880: \t    HPIP:  %3u\n", list->hpip_count);
		printk(KERN_INFO "tl880: \t    DPC:   %3u\n", list->dpc_count);
		printk(KERN_INFO "tl880: \t    TSD:   %3u\n", list->tsd_count);
		printk(KERN_INFO "tl880: \t    Total: %3u\n", list->int_count);
		printk(KERN_INFO "tl880: \t  VPX info:\n");
		printk(KERN_INFO "tl880: \t    I2C Addr:      %2x\n", list->vpx_addr);
		printk(KERN_INFO "tl880: \t    I2C Bus:       %2d\n", list->vpx_i2cbus);
		printk(KERN_INFO "tl880: \t    I2C Client ID: %2d\n", list->vpx_i2cclient);
		printk(KERN_INFO "tl880: \t  MSP info:\n");
		printk(KERN_INFO "tl880: \t    I2C Addr:      %2x\n", list->msp_addr);
		printk(KERN_INFO "tl880: \t    I2C Bus:       %2d\n", list->msp_i2cbus);
		printk(KERN_INFO "tl880: \t    I2C Client ID: %2d\n", list->msp_i2cclient);
		list = list->next;
	}
}

static int tl880_ioctl(struct inode *inode, struct file *file,
		       unsigned int cmd, unsigned long arg)
{
	struct tl880_dev *tl880dev;
	u32 *argl;
	int err = 0;

	if(CHECK_NULL(file) || CHECK_NULL(file)) {
		return -EINVAL;
	}

	/* The & DEV_MASK part is not necessary because of the division by 4 */
	if((tl880dev = find_tl880((iminor(inode) & DEV_MASK) >> 2)) == NULL) {
		printk(KERN_ERR "tl880: unable to find card for minor %u\n", iminor(inode));
		return -ENODEV;
	}

	/* Verify the pointer is valid */
	if(_IOC_DIR(cmd) & _IOC_READ) {
		err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
	}
	if(_IOC_DIR(cmd) & _IOC_WRITE) {
		err = err || !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));
	}
	if(err) {
		printk(KERN_ERR "tl880: invalid arg parameter to tl880_ioctl\n");
		return -EINVAL;
	}

	switch (cmd) {
		/* The do {} while(0); allows the use of very local variables */
		case TL880IOCREADREG:
			/* Read from TL880 register */
			do {
				u32 value;
				argl = (u32 *)arg;
				value = tl880_read_register(tl880dev, *argl);
				__put_user(value, argl);
			} while(0);
			break;
		case TL880IOCWRITEREG:
			/* Write to TL880 register */
			do {
				u32 wrprm[2];
				argl = (u32 *)arg;
				if(copy_from_user(wrprm, argl, sizeof(wrprm))) {
					printk(KERN_ERR "tl880: copy from user returned nonzero for writereg\n");
				}
				tl880_write_register(tl880dev, wrprm[0], wrprm[1]);
			} while(0);
			break;
		case TL880IOCSETVIP:
			/* Set TL880 VIP (VESA Interface Port??) state */
			do {
				u32 vipstate;
				argl = (u32 *)arg;
				if(copy_from_user(&vipstate, argl, sizeof(u32))) {
					printk(KERN_ERR "tl880: copy from user returned nonzero for setvip\n");
				}
				tl880_set_vip(tl880dev, vipstate);
			} while(0);
			break;
		case TL880IOCSETCURSORPOS:
			/* Set TL880 mouse cursor position */
			do {
				u32 cursorpos;
				argl = (u32 *)arg;
				if(copy_from_user(&cursorpos, argl, sizeof(u32))) {
					printk(KERN_ERR "tl880: copy from user returned nonzero for setcursorpos\n");
				}
				tl880_write_register(tl880dev, 0x10104, cursorpos);
			} while(0);
			break;
		case TL880IOCSETGPIO:
			/* Set GPIO state (taking card revision into consideration) */
			do {
				u32 wrprm[2]; //wrprm -- write parameter
				argl = (u32 *)arg;
				if(copy_from_user(wrprm, argl, sizeof(u32) * 2)) {
					printk(KERN_ERR "tl880: copy from user returned nonzero for writereg\n");
				}
				tl880_set_gpio(tl880dev, wrprm[0], wrprm[1]);
			} while(0);
			break;
		case VIDIOC_LOG_STATUS:
			/* Log driver status of tl880 and all clients */
			tl880_log_status();
			tl880_call_i2c_clients(tl880dev, cmd, (void *)arg);
			break;
		default:
			/* Unsupported function - try to pass to I2C clients */
			printk(KERN_ERR "tl880: unsupported ioctl 0x%08x - passing the buck\n", cmd);
			return tl880_call_i2c_clients(tl880dev, cmd, (void *)arg);
	}
	return 0;
}


/* Memory map function - called by the kernel when a userspace app mmaps our device(s) in /dev */
static int tl880_mmap(struct file *file, struct vm_area_struct *vma)
{
	unsigned long start = 0;
	unsigned long length = 0;
	struct tl880_dev *tl880dev;
	struct inode *inode;
	enum { TL880_MEM, TL880_REG, TL880_UNK } maptype;
	unsigned long offset;


	/* Make sure all the expected data is present */
	if(CHECK_NULL(file) || CHECK_NULL(vma)) {
		return -EINVAL;
	}

	inode = file->f_dentry->d_inode;
	offset = vma->vm_pgoff << PAGE_SHIFT;

	/* Determine which card function is requested by minor number */
	switch(iminor(inode) & FUNC_MASK) {
		case 0:
			maptype = TL880_MEM;
			break;
		case 1:
			maptype = TL880_REG;
			break;
		case 2:
			maptype = TL880_UNK;
			break;
		default:
			printk(KERN_WARNING "tl880: mmap: invalid minor number - no function exists for %u\n",
				iminor(inode) & FUNC_MASK);
			return -ENODEV;
	}

	/* Only allow mapping at the beginning of the file */
	if(vma->vm_pgoff) {
	/*
		printk(KERN_WARNING "tl880: only map at start of file (not 0x%08lx)\n", vma->vm_pgoff << PAGE_SHIFT);
		return -EINVAL;
	*/
		printk(KERN_DEBUG "tl880: mapping at offset 0x%08lx\n", offset);
	}

	if((tl880dev = find_tl880(iminor(inode) / 4)) == NULL) {
		printk(KERN_ERR "tl880: unable to find card for minor %u\n", iminor(inode));
		return -EINVAL;
	}

	switch(maptype) {
		default:
			/* Won't happen */
			printk(KERN_WARNING "tl880: maptype invalid in tl880_mmap - defaulting to mem\n");
		case TL880_MEM:
			start = (tl880dev->memphys & PAGE_MASK) + offset;
			length = PAGE_ALIGN((tl880dev->memphys & ~PAGE_MASK) + tl880dev->memlen - offset);
			break;
		case TL880_REG:
			start = (tl880dev->regphys & PAGE_MASK) + offset;
			length = PAGE_ALIGN((tl880dev->regphys & ~PAGE_MASK) + tl880dev->reglen - offset);
			break;
		case TL880_UNK:
			start = (tl880dev->unkphys & PAGE_MASK) + offset;
			length = PAGE_ALIGN((tl880dev->unkphys & ~PAGE_MASK) + tl880dev->unklen - offset);
			break;
	}

	if(debug) {
		printk(KERN_DEBUG "tl880: mmap: start: 0x%08lx, length: 0x%08lx\n", start, length);
	}

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	vma->vm_flags |= VM_RESERVED;
	vma->vm_flags |= VM_IO;

	if(remap_pfn_range(vma, vma->vm_start, start >> PAGE_SHIFT, vma->vm_end - vma->vm_start, vma->vm_page_prot)) {
		printk(KERN_ERR "tl880: remap_pfn_range call failed\n");
		return -EAGAIN;
	}

	return 0;
}


static ssize_t tl880_read(struct file *file, char *buf,
			  size_t count, loff_t *ppos)
{
	struct inode *char_inode;
	struct tl880_dev *tl880dev;
	enum { TL880_MEM, TL880_REG, TL880_UNK } readtype;

	if(CHECK_NULL(file) || CHECK_NULL(buf)) {
		return -EINVAL;
	}
	
	char_inode = file->f_dentry->d_inode;
	
	printk(KERN_DEBUG "tl880: tl880_read called for device %u, %u, card number %u\n",
		imajor(char_inode), iminor(char_inode), (iminor(char_inode) & DEV_MASK) / 4);

	/* Find card by minor number */
	if((tl880dev = find_tl880(iminor(char_inode) / 4)) == NULL) {
		printk(KERN_ERR "tl880: couldin't find card %u (minor %u)\n", iminor(char_inode) / 4, iminor(char_inode));
		return -ENODEV;
	}

	/* Determine which card function is requested by minor number */
	switch(iminor(char_inode) & FUNC_MASK) {
		case 0:
			readtype = TL880_MEM;
			break;
		case 1:
			readtype = TL880_REG;
			break;
		case 2:
			readtype = TL880_UNK;
			break;
		default:
			printk(KERN_WARNING "tl880: read: invalid minor number - no function exists for %u\n",
				iminor(char_inode) & FUNC_MASK);
			return -ENODEV;
	}

	return -EINVAL;
}


static ssize_t tl880_write(struct file *file, const char *buf,
			   size_t count, loff_t *ppos)
{
	struct inode *char_inode = file->f_dentry->d_inode;
	printk(KERN_DEBUG "tl880: tl880_write called for device %u, %u, card number %u\n",
		imajor(char_inode), iminor(char_inode), (iminor(char_inode) & DEV_MASK) / 4);

	return -EINVAL;
}


static int tl880_open(struct inode *inode, struct file *file)
{
	if(CHECK_NULL(inode) || CHECK_NULL(file)) {
		printk(KERN_ERR "tl880: tl880_open given null parameters\n");
		return 0;
	}

	/*
	struct inode *char_inode = file->f_dentry->d_inode;

	printk(KERN_DEBUG "tl880: tl880_open called for device %u, %u, card number %u\n",
		imajor(char_inode), iminor(char_inode), (iminor(char_inode) & DEV_MASK) / 4);
	printk(KERN_DEBUG "tl880: tl880_open (checking data) called for device %u, %u, card number %u\n",
		imajor(inode), iminor(inode), (iminor(inode) & DEV_MASK) / 4);
	*/

	return 0;
}


static int tl880_release(struct inode *inode, struct file *file)
{
	if(CHECK_NULL(inode) || CHECK_NULL(file)) {
		printk(KERN_ERR "tl880: tl880_release given null parameters\n");
		return 0;
	}
	
	/*
	struct inode *char_inode = file->f_dentry->d_inode;
	printk(KERN_DEBUG "tl880: tl880_release called for device %u, %u, card number %u\n",
		imajor(char_inode), iminor(char_inode), (iminor(char_inode) & DEV_MASK) / 4);
	printk(KERN_DEBUG "tl880: tl880_release (checking data) called for device %u, %u, card number %u\n",
		imajor(inode), iminor(inode), (iminor(inode) & DEV_MASK) / 4);
	*/

	return 0;
}


/* File operations structure for character device */
static struct file_operations tl880_fops =
{
	.owner		= THIS_MODULE,
	.open		= tl880_open,
	.release	= tl880_release,
	.ioctl		= tl880_ioctl,
	.read		= tl880_read,
	.llseek		= no_llseek,
	.write		= tl880_write,
	.mmap		= tl880_mmap
};


void tl880_detect_card(struct tl880_dev *tl880dev)
{
	u32 value;

	if(!tl880dev) {
		return;
	}

	switch(tl880dev->subsys_vendor_id) {
		case PCI_SUBSYSTEM_VENDOR_ID_MIT:
			snprintf(tl880dev->name, 64, "MIT Inc.");
			switch(tl880dev->subsys_device_id) {
				case PCI_SUBSYSTEM_DEVICE_ID_MYHD_MDP130:
					snprintf(tl880dev->name, 64, "%s MyHD MDP-130", tl880dev->name);
					tl880dev->card_type = TL880_CARD_MYHD_MDP130;
					break;
				case PCI_SUBSYSTEM_DEVICE_ID_MYHD_MDP120:
					snprintf(tl880dev->name, 64, "%s MyHD MDP-120", tl880dev->name);
					tl880dev->card_type = TL880_CARD_MYHD_MDP120;
					break;
				case PCI_SUBSYSTEM_DEVICE_ID_MYHD:
					snprintf(tl880dev->name, 64, "%s MyHD", tl880dev->name);
					tl880_write_regbits(tl880dev, 0x10198, 0xf, 8, 0x10);
					tl880_write_regbits(tl880dev, 0x10190, 0x17, 8, 0xffff);
					tl880_write_regbits(tl880dev, 0x10194, 0x17, 8, 0);
					value = tl880_read_regbits(tl880dev, 0x1019c, 0x17, 8);
					if((value & 0xc982) == 0xc182) {
						snprintf(tl880dev->name, 64, "%s MDP-110B", tl880dev->name);
						tl880dev->card_type = TL880_CARD_MYHD_MDP110;
					} else {
						if ((value & 0x82) == 0x82) {
							snprintf(tl880dev->name, 64, "%s MDP-100B", tl880dev->name);
							tl880dev->card_type = TL880_CARD_MYHD_MDP100;
						} else {
							snprintf(tl880dev->name, 64, "%s Unknown (0x%04x)",
								tl880dev->name, value);
							tl880dev->card_type = TL880_CARD_ZERO;
						}
					}
					break;
				default:
					snprintf(tl880dev->name, 64, "%s Unknown card %04x:%04x", tl880dev->name,
						tl880dev->subsys_vendor_id, 
						tl880dev->subsys_device_id);
					tl880_write_register(tl880dev, 0x10190, 0x0cfffbff);
					tl880_write_register(tl880dev, 0x10194, 0xefb00);
					tl880_write_register(tl880dev, 0x10198, 0xe5900);
					tl880dev->card_type = TL880_CARD_MYHD_MDP120;
					break;
			}
			break;
		case PCI_SUBSYSTEM_VENDOR_ID_HAUPPAUGE:
			snprintf(tl880dev->name, 64, "Hauppauge Computer Works Inc.");
			switch(tl880dev->subsys_device_id) {
				case PCI_SUBSYSTEM_DEVICE_ID_WINTV_HD:
					snprintf(tl880dev->name, 64, "%s WinTV-HD", tl880dev->name);
					tl880dev->card_type = TL880_CARD_WINTV_HD;
					break;
				default:
					snprintf(tl880dev->name, 64, "%s Unknown card %04x:%04x", tl880dev->name,
						tl880dev->subsys_vendor_id, 
						tl880dev->subsys_device_id);
					tl880dev->card_type = TL880_CARD_WINTV_HD;
					break;
			}
			tl880_write_register(tl880dev, 0x10190, 0x0cfffbff);
			tl880_write_register(tl880dev, 0x10194, 0xefb00);
			tl880_write_register(tl880dev, 0x10198, 0xe5900);
			break;
		case PCI_SUBSYSTEM_VENDOR_ID_TELEMANN:
		case PCI_SUBSYSTEM_VENDOR_ID_ZERO:
			/* The HiPix uses subsytem ID 0000:0000 */
			snprintf(tl880dev->name, 64, "Telemann Systems");
			switch(tl880dev->subsys_device_id) {
				case PCI_SUBSYSTEM_DEVICE_ID_HIPIX:
					snprintf(tl880dev->name, 64, "%s HiPix", tl880dev->name);
					tl880dev->card_type = TL880_CARD_HIPIX;
					break;
				default:
					snprintf(tl880dev->name, 64, "%s Unknown card %04x:%04x", tl880dev->name,
						tl880dev->subsys_vendor_id, 
						tl880dev->subsys_device_id);
					tl880dev->card_type = TL880_CARD_HIPIX;
					break;
			}
			tl880_write_register(tl880dev, 0x10190, 0x0cfffbff);
			tl880_write_register(tl880dev, 0x10194, 0xefb00);
			tl880_write_register(tl880dev, 0x10198, 0xe5900);
			break;
		default:
			snprintf(tl880dev->name, 64, "Unknown TL880 card %04x:%04x",
				tl880dev->subsys_vendor_id, tl880dev->subsys_device_id);
			tl880_write_register(tl880dev, 0x10190, 0x0cfffbff);
			tl880_write_register(tl880dev, 0x10194, 0xefb00);
			tl880_write_register(tl880dev, 0x10198, 0xe5900);
			break;
	}

	printk(KERN_INFO "tl880: Found %s\n", tl880dev->name);
}


static struct tl880_dev *tl880_create_dev(void)
{
	struct tl880_dev *tl880dev;

	tl880dev = kmalloc(sizeof(struct tl880_dev), GFP_KERNEL);

	if(CHECK_NULL(tl880dev)) {
		return NULL;
	}

	/** Housekeeping **/
	tl880dev->next = NULL;
	tl880dev->pcidev = NULL;
	tl880dev->id = 0;
	tl880dev->subsys_vendor_id = 0;
	tl880dev->subsys_device_id = 0;
	tl880dev->name[0] = 0;

	/** I/O **/
	tl880dev->memspace = NULL;
	tl880dev->memphys = 0;
	tl880dev->memlen = 0;
	tl880dev->regspace = NULL;
	tl880dev->regphys = 0;
	tl880dev->reglen = 0;
	tl880dev->unkspace = NULL;
	tl880dev->unkphys = 0;
	tl880dev->unklen = 0;

	tl880dev->pool = NULL;

	/** DMA **/
	tl880dev->dmavirt = NULL;
	tl880dev->dmaphys = 0;

	/** I2C **/
	tl880dev->minbus = 0;
	tl880dev->maxbus = 0;
	tl880dev->i2cbuses = NULL;
	
	/** Interrupt **/
	tl880dev->irq = 0;
	tl880dev->elseint = 0;

	tl880dev->int_mask = 0;
	tl880dev->int_type = 0;
	tl880dev->int_count = 0;

	tl880dev->vsc_mask = 0;
	tl880dev->vsc_type = 0;
	tl880dev->vsc_count = 0;
	
	tl880dev->apu_mask = 0;
	tl880dev->apu_type = 0;
	tl880dev->apu_count = 0;
	tl880dev->apu_th_funcs = NULL;
	tl880dev->apu_bh_funcs = NULL;
	
	tl880dev->blt_mask = 0;
	tl880dev->blt_type = 0;
	tl880dev->blt_count = 0;
	
	tl880dev->mce_mask = 0;
	tl880dev->mce_type = 0;
	tl880dev->mce_count = 0;
	
	tl880dev->mcu_mask = 0;
	tl880dev->mcu_type = 0;
	tl880dev->mcu_count = 0;
	
	tl880dev->vpip_mask = 0;
	tl880dev->vpip_type = 0;
	tl880dev->vpip_count = 0;
	
	tl880dev->hpip_mask = 0;
	tl880dev->hpip_type = 0;
	tl880dev->hpip_count = 0;
	
	tl880dev->dpc_mask = 0;
	tl880dev->dpc_type = 0;
	tl880dev->dpc_count = 0;
	
	tl880dev->tsd_mask = 0;
	tl880dev->tsd_type = 0;
	tl880dev->tsd_count = 0;
	
	/** Character device **/
	tl880dev->char_device = NULL;
	tl880dev->major = tl_major;
	tl880dev->minor = 0;

	/** Audio state **/
	tl880dev->audio_mode = 0;
	tl880dev->iau_base = 0;
	tl880dev->iau_iba_reg = 0;
	tl880dev->iau_iea_reg = 0;
	tl880dev->iau_ira_reg = 0;

	/** Audio state **/
	tl880dev->audio_mode = ZERO;
	tl880dev->iec_buf = NULL;
	tl880dev->iau_base = 0;
	tl880dev->iau_iba_reg = 0;
	tl880dev->iau_iea_reg = 0;
	tl880dev->iau_ira_reg = 0;

	tl880dev->msp_addr = 0;
	tl880dev->msp_i2cbus = 0;
	tl880dev->msp_i2cclient = -1;

	/** Video state **/

	/** Video chip state */
	tl880dev->vpx_addr = 0;
	tl880dev->vpx_i2cbus = 0;
	tl880dev->vpx_i2cclient = -1;
	tl880dev->vpx_video_standard = 0;

	return tl880dev;
}

/* Free the memory used by a tl880_dev struct */
static void tl880_delete_dev(struct tl880_dev *tl880dev)
{
	/* Make sure the driver is really loaded */
	if(CHECK_NULL(tl880dev)) {
		return;
	}

	/* Free interrupt handler callback lists */
	while(tl880dev->apu_th_funcs != NULL) {
		tl880_remove_int_callback(tl880dev, &tl880dev->apu_th_funcs, tl880dev->apu_th_funcs->func);
	}
	while(tl880dev->apu_bh_funcs != NULL) {
		tl880_remove_int_callback(tl880dev, &tl880dev->apu_th_funcs, tl880dev->apu_th_funcs->func);
	}

	/* Free the device information structure */
	kfree(tl880dev);
}

#if 0
/* Dummy interrupt handler */
static irqreturn_t tl880_irq_noop(int irq, void *dev_id)
{
	return IRQ_HANDLED;
}
#endif

/* Set up data specific to each TL880 card in the system */
static int tl880_configure(struct pci_dev *dev)
{
	int result;
	struct tl880_dev *tl880dev;
	struct tl880_dev *list = tl880_list;
	/* char buf[16]; */
	unsigned char pin = 0;

	if(CHECK_NULL(dev)) {
		return -EINVAL;
	}
	
	printk(KERN_INFO "tl880: Initializing card %d\n", n_tl880s);

	/* Allocate device info struct */
	if(!(tl880dev = tl880_create_dev())) {
		printk(KERN_ERR "tl880: could not create tl880 device struct; out of memory\n");
		return -ENOMEM;
	}

	/* Initialize housekeeping values */
	tl880dev->pcidev = dev;
	tl880dev->id = n_tl880s;
	tl880dev->subsys_vendor_id = dev->subsystem_vendor;
	tl880dev->subsys_device_id = dev->subsystem_device;

	/* Enable bus master setting (just in case?) */
	pci_set_master(dev);

	/* Verify 32-bit DMA */
	if((result = pci_set_dma_mask(dev, 0x00000000ffffffffULL))) {
		printk(KERN_ERR "tl880: card does not support 32-bit DMA\n");
		tl880_delete_dev(tl880dev);
		return result;
	}

	/* Map and store memory region (bar0) */
	tl880dev->memspace = ioremap(pci_resource_start(dev, 0), pci_resource_len(dev, 0));
	tl880dev->memphys = pci_resource_start(dev, 0);
	tl880dev->memlen = pci_resource_len(dev, 0);
	if(!tl880dev->memlen) {
		printk(KERN_ERR "tl880: card %d mem has zero length!\n", tl880dev->id);
		tl880_unconfigure(tl880dev);
		tl880_delete_dev(tl880dev);
		return -ENODEV;
	}

	/* Map and store register region (bar3) */
	tl880dev->regspace = ioremap(pci_resource_start(dev, 3), pci_resource_len(dev, 3));
	tl880dev->regphys = pci_resource_start(dev, 3);
	tl880dev->reglen = pci_resource_len(dev, 3);
	if(!tl880dev->reglen) {
		printk(KERN_ERR "tl880: card %d register space has zero length!\n", tl880dev->id);
		tl880_unconfigure(tl880dev);
		tl880_delete_dev(tl880dev);
		return -ENODEV;
	}

	/* Map and store unknown region (bar4) */
	tl880dev->unkspace = ioremap(pci_resource_start(dev, 4), pci_resource_len(dev, 4));
	tl880dev->unkphys = pci_resource_start(dev, 4);
	tl880dev->unklen = pci_resource_len(dev, 4);
	if(!tl880dev->unklen) {
		printk(KERN_ERR "tl880: Card %d unknown space has zero length!\n", tl880dev->id);
		tl880_unconfigure(tl880dev);
		tl880_delete_dev(tl880dev);
		return -ENODEV;
	}

	printk(KERN_DEBUG "tl880: Card %d resources: mem: 0x%08lx(0x%08lx), reg: 0x%08lx(0x%08lx), unk: 0x%08lx(0x%08lx)\n", 
		tl880dev->id,
		tl880dev->memphys, (unsigned long)tl880dev->memspace,
		tl880dev->regphys, (unsigned long)tl880dev->regspace,
		tl880dev->unkphys, (unsigned long)tl880dev->unkspace);

	/* Allocate DMA buffer */
	if(!(tl880dev->dmavirt = dma_alloc_coherent(&(dev->dev), 0x100000, &tl880dev->dmaphys, GFP_ATOMIC))) {
		printk(KERN_ERR "tl880: Failed to allocate dma space\n");
		tl880dev->dmaphys = 0;
	}

	/* Tell the card about the DMA buffer (this should normally be done elsewhere) */
	tl880_write_register(tl880dev, 0x25718, tl880dev->dmaphys >> 16);
	tl880_write_register(tl880dev, 0x2571c, tl880dev->dmaphys & 0xffff);
		
	/* Initialize tasklet for handling slow interrupt-driven tasks */
	tasklet_init(&tl880dev->tasklet, tl880_bh, tl880dev->id);

	/* Inform the kernel about the availability of our device nodes */
	tl880dev->minor = tl880dev->id * 4;
	tl880dev->char_device = cdev_alloc();
	tl880dev->char_device->ops = &tl880_fops;
	tl880dev->char_device->owner = THIS_MODULE;
	if(cdev_add(tl880dev->char_device, MKDEV(tl880dev->major, tl880dev->minor), 3)) {
		printk(KERN_ERR "tl880: could not register character device %u, %u with the kernel\n",
			tl_major, tl880dev->minor);
	}

	/* Disable interrupts and detect card type */
	tl880_disable_interrupts(tl880dev);
	tl880_detect_card(tl880dev);

	/* Store driver handle in pci_dev struct */
	pci_set_drvdata(tl880dev->pcidev, tl880dev);
	
	/* Get IRQ number and set IRQ handler */
	if((result = pci_read_config_byte(dev, PCI_INTERRUPT_PIN, &pin)) < 0) {
		printk(KERN_WARNING "tl880: couldn't determine interrupt pin\n");
	}
	
	tl880dev->irq = dev->irq;
	printk(KERN_INFO "tl880: Card %d uses interrupt pin %u on IRQ line %u\n", n_tl880s, pin, tl880dev->irq);
	if(debug) {
		printk(KERN_DEBUG "tl880: calling request_irq with: %d, 0x%08lx, 0x%08x, %s, 0x%08lx\n",
				tl880dev->pcidev->irq, (unsigned long)tl880_irq, IRQF_SHARED, "tl880", (unsigned long)tl880dev);
	}

	if((result = request_irq(tl880dev->pcidev->irq, tl880_irq, IRQF_SHARED, "tl880", tl880dev)) != 0) {
		printk(KERN_ERR "tl880: could not set irq handler for irq %d: %d\n", tl880dev->pcidev->irq, result);
		tl880_unconfigure(tl880dev);
		tl880_delete_dev(tl880dev);
		return result;
	}

	/* Initialize card memory management */
	if(tl880_init_memory(tl880dev)) {
		printk(KERN_ERR "tl880: unable to initialize memory management\n");
		//tl880_unconfigure(tl880dev);
		//tl880_delete_dev(tl880dev);
		//return -EFAULT;
	}

	/* Initialize card- and chip-specific functions */
	tl880_init_dev(tl880dev);

	if(list) {
		/* Look for the end of the linked list */
		while(list && list->next) {
			list = list->next;
		}
		/* Add this device to the list */
		list->next = tl880dev;
	} else {
		/* Create the list */
		tl880_list = tl880dev;
	}

	n_tl880s++;
	return 0;
}


/* Clean up card-specific data */
static struct tl880_dev *tl880_unconfigure(struct tl880_dev *tl880dev)
{
	struct tl880_dev *next;
	//struct workqueue_struct *wqueue;
	
	if(!tl880dev) {
		printk(KERN_ERR "tl880: can't deinitialize NULL card\n");
		return NULL;
	}
	
	printk(KERN_INFO "tl880: deinitializing card number %d\n", tl880dev->id);
	
	/* Stop card functions */
	tl880_deinit_dev(tl880dev);

	/* Unregister character devices */
	cdev_del(tl880dev->char_device);
	tl880dev->char_device = NULL;

	/* Remove the interrupt handler */
	tl880_disable_interrupts(tl880dev);
	if(tl880dev->irq) {
		free_irq(tl880dev->pcidev->irq, tl880dev);
		tl880dev->irq = 0;
	}

	/* Kill the tasklet */
	tasklet_kill(&tl880dev->tasklet);

	/* Disable I2C */
	tl880_deinit_i2c(tl880dev);

	/* Unmap memory regions */
	if(tl880dev->memspace) {
		iounmap(tl880dev->memspace);
		tl880dev->memspace = NULL;
	}
	if(tl880dev->regspace) {
		iounmap(tl880dev->regspace);
		tl880dev->regspace = NULL;
	}
	if(tl880dev->unkspace) {
		iounmap(tl880dev->unkspace);
		tl880dev->unkspace = NULL;
	}

	/* Free DMA regions */
	if(tl880dev->dmavirt) {
		dma_free_coherent(&(tl880dev->pcidev->dev), 0x100000, tl880dev->dmavirt, tl880dev->dmaphys);
		tl880dev->dmavirt = NULL;
		tl880dev->dmaphys = 0;
	}

	/* Free generic allocation stuff */
	tl880_deinit_memory(tl880dev);

	/* Inform the kernel that this device is no longer in use */
	/* pci_dev_put(tl880dev->pcidev);
	tl880dev->pcidev = NULL;
	*/

	/* Return the next card in the linked list and free this one */
	next = tl880dev->next;
	tl880_delete_dev(tl880dev);
	
	return next;
}


/* Process kernel request to enable a tl880 card */
int tl880_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
	int result;

	printk(KERN_INFO "tl880: Probing card in slot %s (card id is %04x:%04x)\n",
		dev->dev.bus_id, id->vendor, id->device);

	if ((result = pci_enable_device(dev)) != 0) {
		printk(KERN_INFO "tl880: failed to enable device\n");
		return result;
	}

	if((result = tl880_configure(dev)) != 0) {
		printk(KERN_ERR "tl880: failed to configure card in slot %s - not tl880?\n", dev->dev.bus_id);
		return result;
	}

	return 0;
}

/* tl880_remove: deinitialize a tl880 card at the request of the kernel */
void tl880_remove(struct pci_dev *dev)
{
	struct tl880_dev *tl880dev, *tl880prev;

	/* Check for NULL parameter */
	if(CHECK_NULL(dev)) {
		return;
	}
	
	/* We can't remove a card when no cards exist */
	if(n_tl880s <= 0 || CHECK_NULL(tl880_list)) {
		printk(KERN_ERR "tl880: attempt by kernel to tl880_remove when no cards are active\n");
		return;
	}

	/* Find the matching tl880_dev for this pci_dev */
	tl880dev = find_tl880_pci(dev);
	if(CHECK_NULL(tl880dev)) {
		return;
	}

	printk(KERN_INFO "tl880: kernel request to remove card %d\n", tl880dev->id);

	tl880prev = tl880_list;
	while(tl880prev != NULL && tl880prev->next != NULL && tl880prev->next != tl880dev) {
		tl880prev = tl880prev->next;
	}

	if(tl880dev == tl880_list) {
		/* Only one card is loaded - delete the linked list */
		tl880_list = NULL;
	} else if(tl880prev->next == tl880dev) {
		/* More than one card is loaded - remove this device from the linked list */
		tl880prev->next = tl880dev->next;
	} else {
		/* Impossible: no match was found - previous NULL checking shouldn't let us get this far */
		printk(KERN_ERR "tl880: impossible situation: tl880_remove found no match\n");
		return;
	}

	tl880_unconfigure(tl880dev);
	n_tl880s -= 1;
}


static int __init tl880_init(void)
{
	int result = 0;

	printk(KERN_INFO "tl880: Loading tl880 driver\n");

	/* Get static or dynamically allocated major device number */
	if(tl_major) {
		printk(KERN_INFO "tl880: Using specified device number of %u\n", tl_major);
		device_number = MKDEV(tl_major, 0);
		result = register_chrdev_region(device_number, DEV_COUNT, "tl880");
	} else {
		result = alloc_chrdev_region(&device_number, 0, DEV_COUNT, "tl880");
		tl_major = MAJOR(device_number);
	}
	
	if(result) {
		printk(KERN_ERR "tl880: Failed to allocate major number %u with error code: %d\n", tl_major, result);
		return result;
	}

	tl880_create_proc_entry();
	
	return pci_register_driver(&tl880_pcidriver);
}


static void __exit tl880_exit(void)
{
	printk(KERN_INFO "tl880: Unloading tl880 driver\n");
	pci_unregister_driver(&tl880_pcidriver);
	unregister_chrdev_region(device_number, DEV_COUNT);
	tl880_remove_proc_entry();
	printk(KERN_INFO "tl880: module cleanup complete\n");
}

module_init(tl880_init);
module_exit(tl880_exit);

