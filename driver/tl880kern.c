/* 
 * tl880.c - Teralogic 880 based HDTV cards
 *
 * Copyright (C) 2003 Mark Lehrer <mark@knm.org>
 * 		      Mike Bourgeous <nitrogen@slimetech.com>
 *
 * Reverse engineering, control functions:
 *   Mike Bourgeous <nitrogen@slimetech.com>
 *   
 * Driver framework (based on Stradis driver):
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

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/major.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/poll.h>
#include <linux/pci.h>
#include <linux/signal.h>
#include <asm/io.h>
#include <linux/ioport.h>
#include <asm/pgtable.h>
#include <asm/page.h>
#include <linux/sched.h>
#include <asm/types.h>
#include <linux/types.h>
#include <linux/wrapper.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/videodev.h>

#include "tl880.h"

#define DEBUG(x) 		/* debug driver */
#undef  IDEBUG	 		/* debug irq handler */
#undef  MDEBUG	 		/* debug memory management */

unsigned int debug;

MODULE_PARM(debug, "i");
MODULE_LICENSE("GPL");


#ifdef DEBUG
int tl880_driver(void)	/* for the benefit of ksymoops */
{
	return 1;
}
#endif


/* Taken from drivers/char/mem.c */
#ifndef pgprot_noncached
/*
 * This should probably be per-architecture in <asm/pgtable.h>
 */
static inline pgprot_t pgprot_noncached(pgprot_t _prot)
{
	unsigned long prot = pgprot_val(_prot);

#if defined(__i386__) || defined(__x86_64__)
	/* On PPro and successors, PCD alone doesn't always mean 
	    uncached because of interactions with the MTRRs. PCD | PWT
	    means definitely uncached. */ 
	if (boot_cpu_data.x86 > 3)
		prot |= _PAGE_PCD | _PAGE_PWT;
#elif defined(__powerpc__)
	prot |= _PAGE_NO_CACHE | _PAGE_GUARDED;
#elif defined(__mc68000__)
#ifdef SUN3_PAGE_NOCACHE
	if (MMU_IS_SUN3)
		prot |= SUN3_PAGE_NOCACHE;
	else
#endif
	if (MMU_IS_851 || MMU_IS_030)
		prot |= _PAGE_NOCACHE030;
	/* Use no-cache mode, serialized */
	else if (MMU_IS_040 || MMU_IS_060)
		prot = (prot & _CACHEMASK040) | _PAGE_NOCACHE_S;
#endif

	return __pgprot(prot);
}
#endif /* !pgprot_noncached */
/* End drivers/char/mem.c */

static int tl880_ioctl(struct inode *inode, struct file *file,
		       unsigned int cmd, unsigned long arg)
{
	struct tl880_dev *tl880dev = (struct tl880_dev *)file->private_data;
	unsigned long *argl;
	int err = 0;

	if(!file || !tl880dev) {
		return -EINVAL;
	}

	/* Verify the pointer is valid */
	if(_IOC_DIR(cmd) & _IOC_READ) {
		err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
	} else if(_IOC_DIR(cmd) & _IOC_WRITE) {
		err = !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));
	}
	if(err) {
		return -EFAULT;
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
			return -ENOTTY;
	}
	return 0;
}

enum tl880_maptype { TL880_MEM, TL880_REG, TL880_UNK };

static int tl880_mmap(struct file *file, struct vm_area_struct *vma, enum tl880_maptype maptype)
{
	unsigned long start = 0;
	unsigned long length = 0;
	struct tl880_dev *tl880dev = (struct tl880_dev *)file->private_data;

	/* Make sure all the expected data is present */
	if(!file || !vma || !tl880dev) {
		return -EINVAL;
	}

	/* Only allow mapping at the beginning of the file */
	if(vma->vm_pgoff) {
		printk(KERN_DEBUG "tl880: only map at start of file (not 0x%08lx)\n", vma->vm_pgoff << PAGE_SHIFT);
		return -EINVAL;
	}

	switch(maptype) {
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
		default:
			printk(KERN_WARNING "tl880: unknown mmap type requested for card id %i\n", tl880dev->id);
			return -EINVAL;
	}

	if(debug) {
		printk(KERN_DEBUG "tl880: mmap: start: 0x%08lx, length: 0x%08lx\n", start, length);
	}

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	vma->vm_flags |= VM_RESERVED;
	vma->vm_flags |= VM_IO;

	if(io_remap_page_range(vma->vm_start, start, vma->vm_end - vma->vm_start, vma->vm_page_prot)) {
		return -EAGAIN;
	}

	return 0;
}

static int tl880_mem_mmap(struct file *file, struct vm_area_struct *vma)
{
	return tl880_mmap(file, vma, TL880_MEM);
}

static int tl880_reg_mmap(struct file *file, struct vm_area_struct *vma)
{
	return tl880_mmap(file, vma, TL880_REG);
}

static int tl880_unk_mmap(struct file *file, struct vm_area_struct *vma)
{
	return tl880_mmap(file, vma, TL880_UNK);
}

static ssize_t tl880_read(struct file *file, char *buf,
			  size_t count, loff_t *ppos)
{
	return -EINVAL;
}

static ssize_t tl880_write(struct file *file, const char *buf,
			   size_t count, loff_t *ppos)
{
	return 0;
}

static int tl880_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int tl880_release(struct inode *inode, struct file *file)
{
	return 0;
}

static struct file_operations tl880_mem_fops =
{
	.owner		= THIS_MODULE,
	.open		= tl880_open,
	.release	= tl880_release,
	.ioctl		= tl880_ioctl,
	.read		= tl880_read,
	.llseek		= no_llseek,
	.write		= tl880_write,
	.mmap		= tl880_mem_mmap
};

static struct file_operations tl880_reg_fops =
{
	.owner		= THIS_MODULE,
	.open		= tl880_open,
	.release	= tl880_release,
	.ioctl		= tl880_ioctl,
	.read		= tl880_read,
	.llseek		= no_llseek,
	.write		= tl880_write,
	.mmap		= tl880_reg_mmap
};

static struct file_operations tl880_unk_fops =
{
	.owner		= THIS_MODULE,
	.open		= tl880_open,
	.release	= tl880_release,
	.ioctl		= tl880_ioctl,
	.read		= tl880_read,
	.llseek		= no_llseek,
	.write		= tl880_write,
	.mmap		= tl880_unk_mmap
};


/*****/
static int n_tl880s = 0;
static struct tl880_dev *tl880_list = NULL;
static devfs_handle_t devfs_dir = NULL;
/*****/

void tl880_detect_card(struct tl880_dev *tl880dev)
{
	unsigned long value;
	if(!tl880dev) {
		return;
	}


	switch(tl880dev->subsys_vendor_id) {
		case PCI_SUBSYSTEM_VENDOR_ID_MIT:
			printk(KERN_INFO "tl880: Found MIT Inc. ");
			switch(tl880dev->subsys_device_id) {
				case PCI_SUBSYSTEM_DEVICE_ID_MYHD_MDP120:
					printk("MyHD MDP-120\n");
					tl880dev->board_type = TL880_CARD_MYHD_MDP120;
					break;
				case PCI_SUBSYSTEM_DEVICE_ID_MYHD:
					printk("MyHD ");
					write_regbits(tl880dev, 0x10198, 0xf, 8, 0x10);
					write_regbits(tl880dev, 0x10190, 0x17, 8, 0xffff);
					write_regbits(tl880dev, 0x10194, 0x17, 8, 0);
					value = read_regbits(tl880dev, 0x1019c, 0x17, 8);
					if((value & 0xc982) == 0xc182) {
						printk("MDP-110B ");
						tl880dev->board_type = TL880_CARD_MYHD_MDP110;
					} else {
						if ((value & 0x82) == 0x82) {
							printk("MDP-100B ");
							tl880dev->board_type = TL880_CARD_MYHD_MDP100;
						} else {
							printk("Unknown revision (mask 0x%04lx) ", value);
							tl880dev->board_type = TL880_CARD_ZERO;
						}
					}
					printk("\n");
					break;
				default:
					printk("Unknown card %04x:%04x\n",
						tl880dev->subsys_vendor_id, 
						tl880dev->subsys_device_id);
					write_register(tl880dev, 0x10190, 0x0cfffbff);
					write_register(tl880dev, 0x10194, 0xefb00);
					write_register(tl880dev, 0x10198, 0xe5900);
					break;
			}
			break;
		case PCI_SUBSYSTEM_VENDOR_ID_HAUPPAUGE:
			printk(KERN_INFO "tl880: Found Hauppauge Computer Works Inc. ");
			switch(tl880dev->subsys_device_id) {
				case PCI_SUBSYSTEM_DEVICE_ID_WINTV_HD:
					printk("WinTV-HD\n");
					tl880dev->board_type = TL880_CARD_WINTV_HD;
					break;
				default:
					printk("Unknown card %04x:%04x\n",
						tl880dev->subsys_vendor_id, 
						tl880dev->subsys_device_id);
					tl880dev->board_type = TL880_CARD_WINTV_HD;
					break;
			}
			write_register(tl880dev, 0x10190, 0x0cfffbff);
			write_register(tl880dev, 0x10194, 0xefb00);
			write_register(tl880dev, 0x10198, 0xe5900);
			break;
		case PCI_SUBSYSTEM_VENDOR_ID_TELEMANN:
			printk(KERN_INFO "tl880: Found Telemann Systems ");
			switch(tl880dev->subsys_device_id) {
				case PCI_SUBSYSTEM_DEVICE_ID_HIPIX:
					printk("HiPix\n");
					tl880dev->board_type = TL880_CARD_HIPIX;
					break;
				default:
					printk("Unknown card %04x:%04x\n",
						tl880dev->subsys_vendor_id, 
						tl880dev->subsys_device_id);
					tl880dev->board_type = TL880_CARD_HIPIX;
					break;
			}
			write_register(tl880dev, 0x10190, 0x0cfffbff);
			write_register(tl880dev, 0x10194, 0xefb00);
			write_register(tl880dev, 0x10198, 0xe5900);
			break;
		default:
			printk(KERN_WARNING "tl880: Found unknown TL880 card %04x:%04x\n", 
				tl880dev->subsys_vendor_id, tl880dev->subsys_device_id);
			write_register(tl880dev, 0x10190, 0x0cfffbff);
			write_register(tl880dev, 0x10194, 0xefb00);
			write_register(tl880dev, 0x10198, 0xe5900);
			break;
	}
}


struct tl880_dev *create_tl880()
{
	struct tl880_dev *tl880dev;

	tl880dev = kmalloc(sizeof(struct tl880_dev), GFP_KERNEL);

	if(!tl880dev) {
		return tl880dev;
	}

	/** Housekeeping **/
	tl880dev->next = NULL;
	tl880dev->id = 0;
	tl880dev->subsys_vendor_id = 0;
	tl880dev->subsys_device_id = 0;

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

	/** I2C **/
	tl880dev->minbus = 0;
	tl880dev->maxbus = 0;
	tl880dev->i2cbuses = NULL;
	
	/** Interrupt **/
	tl880dev->irq = 0;
	tl880dev->int_type = 0;
	INIT_TQUEUE(&tl880dev->bh, tl880_bh, tl880dev);
	/*
	tl880dev->bh.list = NULL;
	tl880dev->bh.sync = 0;
	tl880dev->bh.routine = tl880_bh;
	tl880dev->bh.data = tl880dev;
	*/

	/** devfs **/
	tl880dev->devfs_device = NULL;

	return tl880dev;
}

void delete_tl880(struct tl880_dev *tl880dev)
{
	if(!tl880dev) {
		return;
	}

	kfree(tl880dev);
}

/* Set up data specific to each TL880 card in the system */
static int configure_tl880(struct pci_dev *dev)
{
	int result;
	struct tl880_dev *tl880dev;
	struct tl880_dev *list = tl880_list;
	char buf[16];

	if(!dev) {
		return -EINVAL;
	}
	
	printk(KERN_INFO "tl880: initializing card number %i\n", n_tl880s);

	if(!(tl880dev = create_tl880())) {
		printk(KERN_ERR "tl880: could not create tl880 device struct\n");
		return -ENOMEM;
	}
	
	if((result = request_irq(dev->irq, tl880_irq, SA_SHIRQ, "tl880", tl880dev)) < 0) {
		printk(KERN_ERR "tl880: could not set irq handler for irq %i\n", dev->irq);
		delete_tl880(tl880dev);
		return result;
	}

	tl880dev->id = n_tl880s;
	tl880dev->subsys_vendor_id = dev->subsystem_vendor;
	tl880dev->subsys_device_id = dev->subsystem_device;
	
	tl880dev->irq = dev->irq;

	/* Map and store memory region (bar0) */
	tl880dev->memspace = ioremap(pci_resource_start(dev, 0), pci_resource_len(dev, 0));
	tl880dev->memphys = pci_resource_start(dev, 0);
	tl880dev->memlen = pci_resource_len(dev, 0);
	if(!tl880dev->memlen) {
		printk(KERN_ERR "tl880: card %i mem has zero length!\n", tl880dev->id);
		delete_tl880(tl880dev);
		return -ENODEV;
	}

	/* Map and store register region (bar3) */
	tl880dev->regspace = ioremap(pci_resource_start(dev, 3), pci_resource_len(dev, 3));
	tl880dev->regphys = pci_resource_start(dev, 3);
	tl880dev->reglen = pci_resource_len(dev, 3);
	if(!tl880dev->reglen) {
		printk(KERN_ERR "tl880: card %i register space has zero length!\n", tl880dev->id);
		delete_tl880(tl880dev);
		return -ENODEV;
	}

	/* Map and store unknown region (bar4) */
	tl880dev->unkspace = ioremap(pci_resource_start(dev, 4), pci_resource_len(dev, 4));
	tl880dev->unkphys = pci_resource_start(dev, 4);
	tl880dev->unklen = pci_resource_len(dev, 4);
	if(!tl880dev->unklen) {
		printk(KERN_ERR "tl880: card %i unknown space has zero length!\n", tl880dev->id);
		delete_tl880(tl880dev);
		return -ENODEV;
	}

	printk(KERN_DEBUG "tl880: mem: 0x%08lx(0x%08lx), reg: 0x%08lx(0x%08lx), unk: 0x%08lx(0x%08lx)\n", 
		tl880dev->memphys, (unsigned long)tl880dev->memspace,
		tl880dev->regphys, (unsigned long)tl880dev->regspace,
		tl880dev->unkphys, (unsigned long)tl880dev->unkspace);

	/* Create /dev/tl880/... character devices with world read/write */
	sprintf(buf, "mem%i", tl880dev->id);
	tl880dev->devfs_device = devfs_register(devfs_dir, buf, 
			DEVFS_FL_DEFAULT | DEVFS_FL_AUTO_DEVNUM | DEVFS_FL_AUTO_OWNER, 
			0, 0, S_IFCHR | S_IRUGO | S_IWUGO, &tl880_mem_fops, tl880dev);
	sprintf(buf, "reg%i", tl880dev->id);
	tl880dev->devfs_device = devfs_register(devfs_dir, buf, 
			DEVFS_FL_DEFAULT | DEVFS_FL_AUTO_DEVNUM | DEVFS_FL_AUTO_OWNER, 
			0, 0, S_IFCHR | S_IRUGO | S_IWUGO, &tl880_reg_fops, tl880dev);
	sprintf(buf, "unk%i", tl880dev->id);
	tl880dev->devfs_device = devfs_register(devfs_dir, buf, 
			DEVFS_FL_DEFAULT | DEVFS_FL_AUTO_DEVNUM | DEVFS_FL_AUTO_OWNER, 
			0, 0, S_IFCHR | S_IRUGO | S_IWUGO, &tl880_unk_fops, tl880dev);

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
	
	if(!tl880dev) {
		printk(KERN_DEBUG "tl880: can't deinitialize NULL card\n");
		return NULL;
	}
	
	printk(KERN_INFO "tl880: deinitializing card number %i\n", tl880dev->id);

	tl880_deinit_i2c(tl880dev);

	free_irq(tl880dev->irq, tl880dev);
	if(tl880dev->memspace) {
		iounmap(tl880dev->memspace);
	}
	if(tl880dev->regspace) {
		iounmap(tl880dev->regspace);
	}
	if(tl880dev->unkspace) {
		iounmap(tl880dev->unkspace);
	}

	if(tl880dev->devfs_device) {
		devfs_unregister(tl880dev->devfs_device);
	}

	next = tl880dev->next;
	delete_tl880(tl880dev);
	
	return next;
}


/* Init driver global data */
static int init_tl880()
{
	return 0;
}


static void release_tl880(void)
{
	struct tl880_dev *tl880dev = tl880_list;
	while(tl880dev) {
		tl880dev = unconfigure_tl880(tl880dev);
	}
	tl880_list = NULL;
	n_tl880s = 0;

}

static int __init tl880_init(void)
{
	struct pci_dev *dev = NULL;
	int result = 0;

	if(!(devfs_dir = devfs_mk_dir(NULL, "tl880", NULL))) {
		return -EBUSY;
	}

	while ((dev = pci_find_device(PCI_VENDOR_ID_TERALOGIC,
				      PCI_DEVICE_ID_TERALOGIC_880, dev))) {
		printk(KERN_INFO "tl880: device found, irq %d\n", dev->irq); 
		if (0 != pci_enable_device(dev)) {
			printk(KERN_INFO "tl880: failed to enable device\n");
			break;
		}

		result = configure_tl880(dev);
		if(result) {
			devfs_unregister(devfs_dir);
			return result;
		}
	}
	/* If no TL880-based cards were found, return with some error */
	if (init_tl880() < 0 || !n_tl880s) {
		release_tl880();
		devfs_unregister(devfs_dir);
		return -EIO;
	}
	return 0;
}


static void __exit tl880_exit (void)
{
	release_tl880();
	if(devfs_dir) {
		devfs_unregister(devfs_dir);
		devfs_dir = NULL;
	}
	printk(KERN_INFO "tl880: module cleanup complete\n");
}


module_init(tl880_init);
module_exit(tl880_exit);

