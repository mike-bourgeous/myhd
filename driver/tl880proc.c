/* 
 * Functions for handling /proc user interface
 *
 * (c) 2005-2007 Mike Bourgeous <nitrogen at users.sourceforge.net>
 *
 * $Log: tl880proc.c,v $
 * Revision 1.7  2007/04/24 06:32:13  nitrogen
 * Changed most int/long types to explicit 32-bit sizes.  Fixed compilation and execution on 64-bit CPUs.
 *
 * Revision 1.6  2007/03/26 19:25:06  nitrogen
 * Added CVS log generation and updated copyrights and e-mail addresses.
 *
 */
#include "tl880.h"

/* Function called when user reads from /proc/tl880/devices */
static int tl880_proc_read(char *page, char **start, off_t offset, int count, int *eof, void *data)
{
	struct tl880_dev *tl880dev = tl880_list;
	
	printk(KERN_DEBUG "tl880: tl880_proc_read called with page=0x%08lx, offset=0x%08lx, count=%i\n",
		(unsigned long)page, (unsigned long)offset, count);

	page[0] = '\0';

	while(tl880dev != NULL && count - strlen(page) > 0) {
		snprintf(page, count - strlen(page), "%s%u: %s\t%04x:%04x\tDMA@%08lx\n", page,
			tl880dev->id, tl880dev->name,
			tl880dev->subsys_vendor_id, tl880dev->subsys_device_id,
			(unsigned long)tl880dev->dmaphys);
		tl880dev = tl880dev->next;
		/* printk(KERN_DEBUG "tl880: so far, page is %s\n", page); */
	}

	/* XXX: at this point tl880dev will always be null? */
	if(!tl880dev) {
		*eof = 1;
	}
	
	return strlen(page);
}

/* Create tl880 entries in /proc */
int tl880_create_proc_entry()
{
	struct proc_dir_entry *entry;

	entry = create_proc_read_entry("tl880", 0, NULL, tl880_proc_read, NULL);
	
	if(!entry) {
		printk(KERN_ERR "tl880: failed to create /proc entry\n");
		return -EIO;
	} else {
		entry->owner = THIS_MODULE;
		return 0;
	}
}

void tl880_remove_proc_entry()
{
	remove_proc_entry("tl880", NULL);
}

