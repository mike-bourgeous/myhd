/* 
 * Functions for handling /proc user interface
 *
 * (c) 2005 Mike Bourgeous <nitrogen@slimetech.com>
 */
#include "tl880.h"

/* Function called when user reads from /proc/tl880/devices */
static int tl880_proc_read(char *page, char **start, off_t offset, int count, int *eof, void *data)
{
	int len = 0;
	struct tl880_dev *tl880dev = tl880_list;
	
	printk(KERN_DEBUG "tl880: tl880_proc_read called with page=0x%08lx, offset=0x%08lx, count=%i\n",
		(unsigned long)page, (unsigned long)offset, count);

	sprintf(page, "");

	while(tl880dev != NULL) {
		printk(KERN_DEBUG "tl880: adding card %u to /proc/tl880\n", tl880dev->id);
		snprintf(page, count - 72, "%s%u: %s\t%04x:%04x\n", page,
			tl880dev->id, tl880dev->name,
			tl880dev->subsys_vendor_id, tl880dev->subsys_device_id);
		tl880dev = tl880dev->next;
		/* printk(KERN_DEBUG "tl880: so far, page is %s\n", page); */
	}

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
		printk(KERN_DEBUG "tl880: created /proc entry %s\n", entry->name);
		return 0;
	}
}

void tl880_remove_proc_entry()
{
	/* remove_proc_entry("tl880/devices", NULL); */
	remove_proc_entry("tl880", NULL);
}
