/* 
 * tl880.c - Teralogic 880 based HDTV cards
 *
 * Copyright (C) 2003-2005 Mike Bourgeous <i_am_nitrogen@hotmail.com>
 *
 * Reverse engineering, all development:
 *   Mike Bourgeous <nitrogen@slimetech.com>
 *   
 * Original driver framework (based on Stradis driver, mostly gone):
 *   Mark Lehrer <mark@knm.org>
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
 */

#include "tl880.h"

#define DEBUG(x) 		/* debug driver */
#undef  IDEBUG	 		/* debug irq handler */
#undef  MDEBUG	 		/* debug memory management */

/* Driver global variables */
unsigned int debug;
unsigned int tl_major;

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
MODULE_PARM_DESC(debug, "Set TL880 driver debug level (currently ignored)");

module_param(tl_major, int, 0);
MODULE_PARM_DESC(tl_major, "TL880 device major number - 0 or unset for automatic");

/* Other local static functions */
static struct tl880_dev *unconfigure_tl880(struct tl880_dev *tl880dev);

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



static int tl880_ioctl(struct inode *inode, struct file *file,
		       unsigned int cmd, unsigned long arg)
{
	struct tl880_dev *tl880dev;
	unsigned long *argl;
	int err = 0;

	if(CHECK_NULL(file) || CHECK_NULL(file)) {
		return -EINVAL;
	}

	/* The & DEV_MASK part is not necessary because of the / 4 */
	tl880dev = find_tl880((iminor(inode) & DEV_MASK) / 4);

	if(tl880dev == NULL) {
		printk(KERN_ERR "tl880: unable to find card for minor %u\n", iminor(inode));
		return -ENODEV;
	}

	printk(KERN_DEBUG "tl880: IOCTL is for ");
	switch(iminor(inode) & FUNC_MASK) {
		case 0:
			printk("mem ");
			break;
		case 1:
			printk("reg ");
			break;
		case 2:
			printk("unk ");
			break;
		default:
			printk("invalid device ");
			break;
	}
	printk("on card %u\n", tl880dev->id);
		

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
		case TL880IOCREADREG:
			argl = (unsigned long *)arg;
			__put_user(read_register(tl880dev, *argl), argl);
			break;
		case TL880IOCWRITEREG:
			do {
				unsigned long wrprm[2];
				argl = (unsigned long *)arg;
				copy_from_user(wrprm, argl, 8);
				write_register(tl880dev, wrprm[0], wrprm[1]);
			} while(0);
			break;
		case TL880IOCSETVIP:
			do {
				unsigned long vipstate;
				argl = (unsigned long *)arg;
				copy_from_user(&vipstate, argl, 4);
				tl880_set_vip(tl880dev, vipstate);
			} while(0);
			break;
		default:
			/* return -ENOIOCTLCMD; */
			printk(KERN_ERR "tl880: unsupported ioctl\n");
			return -ENOTTY;
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


	/* Make sure all the expected data is present */
	if(CHECK_NULL(file) || CHECK_NULL(vma)) {
		return -EINVAL;
	}

	inode = file->f_dentry->d_inode;

	printk(KERN_DEBUG "tl880: mmap called for device (%u, %u), card number %u\n",
		imajor(inode), iminor(inode), (iminor(inode) & DEV_MASK) / 4);

	
	/* Determine which card function is requested by minor number */
	switch(iminor(inode) & FUNC_MASK) {
		case 0:
			printk(KERN_DEBUG "tl880: mmap: minor device specifies mem\n");
			maptype = TL880_MEM;
		case 1:
			printk(KERN_DEBUG "tl880: mmap: minor device specifies reg\n");
			maptype = TL880_REG;
		case 2:
			printk(KERN_DEBUG "tl880: mmap: minor device specifies unk\n");
			maptype = TL880_UNK;
		default:
			printk(KERN_WARNING "tl880: invalid minor number - no function exists for %u\n",
				iminor(inode) & FUNC_MASK);
			return -ENODEV;
	}

	/* Only allow mapping at the beginning of the file */
	if(vma->vm_pgoff) {
		printk(KERN_WARNING "tl880: only map at start of file (not 0x%08lx)\n", vma->vm_pgoff << PAGE_SHIFT);
		return -EINVAL;
	}

	tl880dev = find_tl880(iminor(inode) / 4);

	if(tl880dev == NULL) {
		printk(KERN_ERR "tl880: unable to find card for minor %u\n", iminor(inode));
		return -EINVAL;
	}

	switch(maptype) {
		default:
			/* Won't happen */
			printk(KERN_WARNING "tl880: maptype invalid in tl880_mmap - defaulting to mem\n");
		case TL880_MEM:
			start = tl880dev->memphys & PAGE_MASK;
			length = PAGE_ALIGN((tl880dev->memphys & ~PAGE_MASK) + tl880dev->memlen);
			break;
		case TL880_REG:
			start = tl880dev->regphys & PAGE_MASK;
			length = PAGE_ALIGN((tl880dev->regphys & ~PAGE_MASK) + tl880dev->reglen);
			break;
		case TL880_UNK:
			start = tl880dev->unkphys & PAGE_MASK;
			length = PAGE_ALIGN((tl880dev->unkphys & ~PAGE_MASK) + tl880dev->unklen);
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
	struct inode *char_inode = file->f_dentry->d_inode;
	printk(KERN_DEBUG "tl880: tl880_read called for device %u, %u, card number %u\n",
		imajor(char_inode), iminor(char_inode), (iminor(char_inode) & DEV_MASK) / 4);

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
	/* dev_t i_rdev, struct cdev *i_cdev */
	struct inode *char_inode = file->f_dentry->d_inode;
	printk(KERN_DEBUG "tl880: tl880_open called for device %u, %u, card number %u\n",
		imajor(char_inode), iminor(char_inode), (iminor(char_inode) & DEV_MASK) / 4);
	printk(KERN_DEBUG "tl880: tl880_open (checking data) called for device %u, %u, card number %u\n",
		imajor(inode), iminor(inode), (iminor(inode) & DEV_MASK) / 4);

	return 0;
}


static int tl880_release(struct inode *inode, struct file *file)
{
	struct inode *char_inode = file->f_dentry->d_inode;
	printk(KERN_DEBUG "tl880: tl880_release called for device %u, %u, card number %u\n",
		imajor(char_inode), iminor(char_inode), (iminor(char_inode) & DEV_MASK) / 4);
	printk(KERN_DEBUG "tl880: tl880_release (checking data) called for device %u, %u, card number %u\n",
		imajor(inode), iminor(inode), (iminor(inode) & DEV_MASK) / 4);

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
	unsigned long value;

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
					write_regbits(tl880dev, 0x10198, 0xf, 8, 0x10);
					write_regbits(tl880dev, 0x10190, 0x17, 8, 0xffff);
					write_regbits(tl880dev, 0x10194, 0x17, 8, 0);
					value = read_regbits(tl880dev, 0x1019c, 0x17, 8);
					if((value & 0xc982) == 0xc182) {
						snprintf(tl880dev->name, 64, "%s MDP-110B", tl880dev->name);
						tl880dev->card_type = TL880_CARD_MYHD_MDP110;
					} else {
						if ((value & 0x82) == 0x82) {
							snprintf(tl880dev->name, 64, "%s MDP-100B", tl880dev->name);
							tl880dev->card_type = TL880_CARD_MYHD_MDP100;
						} else {
							snprintf(tl880dev->name, 64, "%s Unknown (0x%04lx)",
								tl880dev->name, value);
							/* Default to MDP 110 or 120 here? */
							tl880dev->card_type = TL880_CARD_ZERO;
						}
					}
					printk("\n");
					break;
				default:
					snprintf(tl880dev->name, 64, "%s Unknown card %04x:%04x", tl880dev->name,
						tl880dev->subsys_vendor_id, 
						tl880dev->subsys_device_id);
					write_register(tl880dev, 0x10190, 0x0cfffbff);
					write_register(tl880dev, 0x10194, 0xefb00);
					write_register(tl880dev, 0x10198, 0xe5900);
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
			write_register(tl880dev, 0x10190, 0x0cfffbff);
			write_register(tl880dev, 0x10194, 0xefb00);
			write_register(tl880dev, 0x10198, 0xe5900);
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
			write_register(tl880dev, 0x10190, 0x0cfffbff);
			write_register(tl880dev, 0x10194, 0xefb00);
			write_register(tl880dev, 0x10198, 0xe5900);
			break;
		default:
			snprintf(tl880dev->name, 64, "Unknown TL880 card %04x:%04x",
				tl880dev->subsys_vendor_id, tl880dev->subsys_device_id);
			write_register(tl880dev, 0x10190, 0x0cfffbff);
			write_register(tl880dev, 0x10194, 0xefb00);
			write_register(tl880dev, 0x10198, 0xe5900);
			break;
	}

	printk(KERN_INFO "tl880: Found %s\n", tl880dev->name);
}


struct tl880_dev *tl880_create_dev(void)
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
	
	tl880dev->blt_mask = 0;
	tl880dev->blt_type = 0;
	tl880dev->blt_count = 0;
	
	tl880dev->mce_mask = 0;
	tl880dev->mce_type = 0;
	tl880dev->mce_count = 0;
	
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

	return tl880dev;
}

void delete_tl880(struct tl880_dev *tl880dev)
{
	/* Make sure the driver is really loaded */
	if(!tl880dev) {
		printk(KERN_WARNING "tl880: Attempt to unload tl880 before loading?\n");
		return;
	}

	/* Free the device information structure */
	kfree(tl880dev);
}

/* Set up data specific to each TL880 card in the system */
static int configure_tl880(struct pci_dev *dev)
{
	int result;
	struct tl880_dev *tl880dev;
	struct tl880_dev *list = tl880_list;
	/* char buf[16]; */
	unsigned char pin = 0;

	if(CHECK_NULL(dev)) {
		return -EINVAL;
	}
	
	printk(KERN_INFO "tl880: initializing card number %i\n", n_tl880s);

	/* Allocate device info struct */
	if(!(tl880dev = tl880_create_dev())) {
		printk(KERN_ERR "tl880: could not create tl880 device struct; out of memory\n");
		return -ENOMEM;
	}
	
	/* Verify 32-bit DMA */
	if((result = pci_set_dma_mask(dev, 0xffffffff))) {
		printk(KERN_ERR "tl880: card does not support 32-bit DMA\n");
		delete_tl880(tl880dev);
		return result;
	}

	/* Set IRQ handler */
	/*
	if((result = pci_read_config_byte(dev, PCI_INTERRUPT_LINE, &tl880dev->irq)) < 0) {
		printk(KERN_ERR "tl880: Could not read IRQ line from PCI config\n");
		delete_tl880(tl880dev);
		return result;
	}
	if(tl880dev->irq != dev->irq) {
		printk(KERN_WARNING "tl880: IRQ line in PCI config (%u) doesn't match IRQ line from dev->irq (%u)\n",
			tl880dev->irq, dev->irq);
		// pci_write_config_byte(dev, PCI_INTERRUPT_LINE, dev->irq);
		tl880dev->irq = dev->irq;
	}
	*/
	tl880dev->irq = dev->irq;

	if((result = pci_read_config_byte(dev, PCI_INTERRUPT_PIN, &pin)) < 0) {
		printk(KERN_WARNING "tl880: couldn't determine interrupt pin\n");
	}
	printk(KERN_INFO "tl880: card %i uses interrupt pin %u on IRQ line %u\n", n_tl880s, pin, tl880dev->irq);
	if((result = request_irq(dev->irq, tl880_irq, SA_SHIRQ, "tl880", tl880dev)) < 0) {
		printk(KERN_ERR "tl880: could not set irq handler for irq %i\n", dev->irq);
		delete_tl880(tl880dev);
		return result;
	}

	/* Initialize housekeeping values */
	tl880dev->pcidev = dev;
	tl880dev->id = n_tl880s;
	tl880dev->subsys_vendor_id = dev->subsystem_vendor;
	tl880dev->subsys_device_id = dev->subsystem_device;

	/* Map and store memory region (bar0) */
	tl880dev->memspace = ioremap(pci_resource_start(dev, 0), pci_resource_len(dev, 0));
	tl880dev->memphys = pci_resource_start(dev, 0);
	tl880dev->memlen = pci_resource_len(dev, 0);
	if(!tl880dev->memlen) {
		printk(KERN_ERR "tl880: card %i mem has zero length!\n", tl880dev->id);
		unconfigure_tl880(tl880dev);
		return -ENODEV;
	}

	/* Map and store register region (bar3) */
	tl880dev->regspace = ioremap(pci_resource_start(dev, 3), pci_resource_len(dev, 3));
	tl880dev->regphys = pci_resource_start(dev, 3);
	tl880dev->reglen = pci_resource_len(dev, 3);
	if(!tl880dev->reglen) {
		printk(KERN_ERR "tl880: card %i register space has zero length!\n", tl880dev->id);
		unconfigure_tl880(tl880dev);
		return -ENODEV;
	}

	/* Map and store unknown region (bar4) */
	tl880dev->unkspace = ioremap(pci_resource_start(dev, 4), pci_resource_len(dev, 4));
	tl880dev->unkphys = pci_resource_start(dev, 4);
	tl880dev->unklen = pci_resource_len(dev, 4);
	if(!tl880dev->unklen) {
		printk(KERN_ERR "tl880: card %i unknown space has zero length!\n", tl880dev->id);
		unconfigure_tl880(tl880dev);
		return -ENODEV;
	}

	printk(KERN_DEBUG "tl880: card %i: mem: 0x%08lx(0x%08lx), reg: 0x%08lx(0x%08lx), unk: 0x%08lx(0x%08lx)\n", 
		tl880dev->id,
		tl880dev->memphys, (unsigned long)tl880dev->memspace,
		tl880dev->regphys, (unsigned long)tl880dev->regspace,
		tl880dev->unkphys, (unsigned long)tl880dev->unkspace);

	/* Allocate DMA buffer */
	if(!(tl880dev->dmavirt = dma_alloc_coherent(&(dev->dev), 0x100000, &tl880dev->dmaphys, GFP_ATOMIC))) {
		printk(KERN_ERR "tl880: failed to allocate dma space\n");
		tl880dev->dmaphys = 0;
	}
		
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
	} else {
		printk(KERN_DEBUG "tl880: registered character device at 0x%08lx, magic 0x%08x (%u, %u) - or (%u, %u)?\n",
			(unsigned long)tl880dev->char_device, MKDEV(tl880dev->major, tl880dev->minor),
			tl880dev->major, tl880dev->minor,
			MAJOR(tl880dev->char_device->dev), MINOR(tl880dev->char_device->dev));
	}
				
	/* Disable interrupts, detect specific card revision, init card */
	tl880_disable_interrupts(tl880dev);
	tl880_detect_card(tl880dev);
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
static struct tl880_dev *unconfigure_tl880(struct tl880_dev *tl880dev)
{
	struct tl880_dev *next;
	//struct workqueue_struct *wqueue;
	
	if(!tl880dev) {
		printk(KERN_ERR "tl880: can't deinitialize NULL card\n");
		return NULL;
	}
	
	printk(KERN_INFO "tl880: deinitializing card number %i\n", tl880dev->id);
	
	/* Unregister character devices */
	printk(KERN_DEBUG "tl880: deleting character device\n");
	cdev_del(tl880dev->char_device);
	tl880dev->char_device = NULL;

	tl880_disable_interrupts(tl880dev);

	printk(KERN_DEBUG "tl880: calling tl880_deinit_i2c\n");
	tl880_deinit_i2c(tl880dev);

	printk(KERN_DEBUG "tl880: calling free_irq\n");
	free_irq(tl880dev->pcidev->irq, tl880dev);

	/* Kill the tasklet */
	printk(KERN_DEBUG "tl880: removing tasklet\n");
	tasklet_kill(&tl880dev->tasklet);

	/* Unmap memory regions */
	printk(KERN_DEBUG "tl880: unmapping memory regions\n");
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
	printk(KERN_DEBUG "tl880: freeing DMA mapping\n");
	if(tl880dev->dmavirt) {
		dma_free_coherent(&(tl880dev->pcidev->dev), 0x100000, tl880dev->dmavirt, tl880dev->dmaphys);
		tl880dev->dmavirt = NULL;
		tl880dev->dmaphys = 0;
	}

	/* Inform the kernel that this device is no longer in use */
	printk(KERN_DEBUG "tl880: freeing PCI device\n");
	pci_dev_put(tl880dev->pcidev);
	tl880dev->pcidev = NULL;

	/* Return the next card in the linked list and free this one */
	next = tl880dev->next;
	delete_tl880(tl880dev);
	
	return next;
}


/* Init driver global data */
static int init_tl880(void)
{
	return 0;
}


/* Clean up all tl880 device info structures */
static void release_tl880(void)
{
	struct tl880_dev *tl880dev = tl880_list;
	while(tl880dev) {
		tl880dev = unconfigure_tl880(tl880dev);
	}
	tl880_list = NULL;
	n_tl880s = 0;

}


/* Process kernel request to enable a tl880 card */
int tl880_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
	int result;

	printk(KERN_INFO "tl880: Kernel is probing a device with id: %04x:%04x, %04x:%04x\n",
		id->vendor, id->device, id->subvendor, id->subdevice);

	printk(KERN_DEBUG "tl880: calling pci_enable_device\n");
	if ((result = pci_enable_device(dev)) != 0) {
		printk(KERN_INFO "tl880: failed to enable device\n");
		return result;
	}
	printk(KERN_INFO "tl880: irq after pci_enable_device: %u\n", dev->irq); 

	if((result = configure_tl880(dev)) != 0) {
		printk(KERN_ERR "tl880: failed to configure card in slot %s\n", dev->dev.bus_id);
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

	printk(KERN_DEBUG "tl880: processing kernel request to remove card %i\n", tl880dev->id);

	unconfigure_tl880(tl880dev);
	n_tl880s -= 1;
}


#if 0
static void __exit tl880_exit(void);
#endif
static int __init tl880_init(void)
{
	struct pci_dev *dev = NULL;
	int result = 0;

	/* Get static or dynamically allocated major device number */
	if(tl_major) {
		printk(KERN_INFO "tl880: Using specified device number of %u\n", tl_major);
		device_number = MKDEV(tl_major, 0);
		result = register_chrdev_region(device_number, DEV_COUNT, "tl880");
	} else {
		printk(KERN_INFO "tl880: Requesting dynamic device number\n");
		result = alloc_chrdev_region(&device_number, 0, DEV_COUNT, "tl880");
		tl_major = MAJOR(device_number);
	}
	
	if(result) {
		printk(KERN_ERR "tl880: Failed to allocate major number %u with error code: %i\n", tl_major, result);
		return result;
	} else {
		printk(KERN_INFO "tl880: base device major/minor: %u, %u\n", MAJOR(device_number), MINOR(device_number));
	}

	tl880_create_proc_entry();
	
	/* Request modules to drive card subfunctions */
	/* TODO: Move these to more appropriate locations (i.e. card-specific init functions) */
	printk(KERN_DEBUG "tl880: Requesting modules\n");
	request_module("i2c-dev");
	request_module("tuner");
	//request_module("nxt2002");

#if 0
	/* 
	 * Look for TL880 devices on the PCI bus
	 * 
	 * - dev is passed as an argument to pci_get_device to allow
	 * the kernel to continue scanning the rest of the PCI bus.
	 */
	while ((dev = pci_get_device(PCI_VENDOR_ID_TERALOGIC,
				      PCI_DEVICE_ID_TERALOGIC_TL880, dev))) {
		printk(KERN_INFO "tl880: device found, current irq %u, slot %s\n", dev->irq, dev->dev.bus_id); 
		printk(KERN_DEBUG "tl880: calling pci_enable_device\n");
		if (0 != pci_enable_device(dev)) {
			printk(KERN_INFO "tl880: failed to enable device\n");
			break;
		}
		printk(KERN_INFO "tl880: irq after pci_enable_device: %u\n", dev->irq); 

		result = configure_tl880(dev);
		if(result) {
			printk(KERN_ERR "tl880: failed to configure card in slot %s\n", dev->dev.bus_id);
			pci_dev_put(dev);
			tl880_exit();
			return result;
		}
	}
	
	/* If no TL880-based cards were found, return with some error */
	if (init_tl880() < 0 || !n_tl880s) {
		printk(KERN_ERR "tl880: No cards found\n");
		tl880_exit();
		return -ENODEV;
	}
	
	printk(KERN_INFO "tl880: found %u card(s).\n", n_tl880s);

#endif
	
	return pci_register_driver(&tl880_pcidriver);
}


static void __exit tl880_exit(void)
{
	pci_unregister_driver(&tl880_pcidriver);
	/* release_tl880(); */
	unregister_chrdev_region(device_number, DEV_COUNT);
	tl880_remove_proc_entry();
	printk(KERN_INFO "tl880: module cleanup complete\n");
}


module_init(tl880_init);
module_exit(tl880_exit);

