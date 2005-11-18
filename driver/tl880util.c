/* 
 * Some utility functions for TL880-based cards driver 
 *
 * (c) 2003 Mike Bourgeous <nitrogen@slimetech.com>
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
	}

	return list;
}


