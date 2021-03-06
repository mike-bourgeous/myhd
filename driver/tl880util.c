/* 
 * Some utility functions for the TL880 driver 
 *
 * (c) 2003-2007 Mike Bourgeous <nitrogen at users.sourceforge.net>
 *
 * $Log: tl880util.c,v $
 * Revision 1.9  2007/09/09 06:16:48  nitrogen
 * Started an ALSA driver.  New iocread4reg tool.  Driver enhancements.
 *
 * Revision 1.8  2007/04/24 06:32:14  nitrogen
 * Changed most int/long types to explicit 32-bit sizes.  Fixed compilation and execution on 64-bit CPUs.
 *
 * Revision 1.7  2007/03/26 19:25:06  nitrogen
 * Added CVS log generation and updated copyrights and e-mail addresses.
 *
 */
#include "tl880.h"

void set_bits(u32 *value, u32 reg, int high_bit, int low_bit, u32 setvalue)
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

/* Return the number of tl880s */
int tl880_card_count()
{
	return n_tl880s;
}

EXPORT_SYMBOL(set_bits);
EXPORT_SYMBOL(find_tl880);
EXPORT_SYMBOL(find_tl880_pci);
EXPORT_SYMBOL(tl880_card_count);

