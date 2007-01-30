/* 
 * VPX322x control for TL880-based cards (until vpx3220 driver has needed features)
 *
 * Functions based on vpx3220 driver and VPX3226F datasheet.
 *
 * (c) 2007 Mike Bourgeous <nitrogen at users.sourceforge.net>
 */
#include "tl880.h"


/* TODO: store VPX address in tl880 struct instead of hard-coding in other functions */
int tl880_vpx_read(struct tl880_i2c_bus *i2cbus, unsigned short addr, unsigned char reg)
{
	return tl880_i2c_read_byte_data(i2cbus, addr, reg);
}

int tl880_vpx_write(struct tl880_i2c_bus *i2cbus, unsigned short addr, unsigned char reg, unsigned char value)
{
	/*
	 * Note - the vpx driver stores written data in an array, which will now be out of sync.
	 * So, the sooner all vpx code is moved over there, the better
	 */

	return tl880_i2c_write_byte_data(i2cbus, addr, reg, value);
}

unsigned short tl880_vpx_read_fp(struct tl880_i2c_bus *i2cbus, unsigned short addr, unsigned short fpaddr)
{
	int result;
	unsigned short data;

	/* Write the 16-bit address to the FPRD register */
	if((result = tl880_i2c_write_word_data(i2cbus, addr, 0x26, __cpu_to_be16(fpaddr))) < 0) {
		printk(KERN_WARNING "tl880: Failed to write vpx FP read register: %d\n", result);
		return -1;
	}

	/* TODO: check for FP busy status */
	/*
	if (vpx3220_fp_status(client) < 0) {
		return -1;
	}
	*/

	/* Read the 16-bit data from the FPDAT register */
	data = tl880_i2c_read_word_data(i2cbus, addr, 0x28);
	if(data == (unsigned short)-1) {
		printk(KERN_WARNING "tl880: Failed to read vpx FP data\n");
		return -1;
	}

	return __be16_to_cpu(data);
}

int tl880_vpx_write_fp(struct tl880_i2c_bus *i2cbus, unsigned short addr, unsigned short fpaddr, unsigned short data)
{
	int result;

	/* Write the 16-bit address to the FPWR register */
	if((result = tl880_i2c_write_word_data(i2cbus, addr, 0x27, __cpu_to_be16(fpaddr))) < 0) {
		printk(KERN_WARNING "tl880: Failed to write vpx FP write register: %d\n", result);
		return -1;
	}

	/* TODO: check for FP busy status */
	/*
	if (vpx3220_fp_status(client) < 0) {
		return -1;
	}
	*/

	/* Write the 16-bit data to the FPDAT register */
	if((result = tl880_i2c_write_word_data(i2cbus, addr, 0x28, __cpu_to_be16(data))) < 0) {
		printk(KERN_WARNING "tl880: Failed to write vpx FP data: %d\n", result);
		return -1;
	}

	return 0;
}


