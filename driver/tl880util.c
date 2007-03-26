/* 
 * Some utility functions for the TL880 driver 
 *
 * (c) 2003-2007 Mike Bourgeous <nitrogen at users.sourceforge.net>
 *
 * $Log: tl880util.c,v $
 * Revision 1.7  2007/03/26 19:25:06  nitrogen
 * Added CVS log generation and updated copyrights and e-mail addresses.
 *
 */
#include "tl880.h"

void set_bits(unsigned long *value, long reg, long high_bit, long low_bit, unsigned long setvalue)
{
	register unsigned long mask = 0;

	/* set bits from high_bit to low_bit in mask to 1 */
	mask = ~(0xFFFFFFFF << (high_bit - low_bit + 1)) << low_bit;
	setvalue <<= low_bit;
	setvalue &= mask;
	mask = ~mask & *value;
	setvalue |= mask;
	*value = setvalue;
}


/* Find a tl880_dev by card number */
struct tl880_dev *find_tl880(unsigned long tl880_id)
{
	struct tl880_dev *list = tl880_list;

	while(list) {
		if(list->id == tl880_id) 
			break;
		list = list->next;
	}

	return list;
}

/* Find a tl880_dev by PCI device ID */
struct tl880_dev *find_tl880_pci(struct pci_dev *dev)
{
	struct tl880_dev *list = tl880_list;

	/* Try to find the device using the pci_dev pointer */
	while(list) {
		if(list->pcidev == dev) {
			return list;
		}
		list = list->next;
	}

	/* Perhaps the pointer doesn't match even when the card is the same - check for matching slot */
	list = tl880_list;
	while(list) {
		if(strcmp(list->pcidev->dev.bus_id, dev->dev.bus_id)) {
			printk(KERN_DEBUG "tl880: found a match on second pass in find_tl880_pci\n");
			break;
		}
	}

	return list;
}

EXPORT_SYMBOL(set_bits);
EXPORT_SYMBOL(find_tl880);

