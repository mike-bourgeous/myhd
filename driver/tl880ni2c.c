/* 
 * I2C for TL880-based cards
 *
 * Based in part on the BT848 I2C interface in drivers/media/video/bttv-if.c
 *
 * (c) 2003 Mike Bourgeous <nitrogen@slimetech.com>
 */
#include "tl880.h"

static void tl880_i2c_set_scl(void *data, int state)
{
	struct tl880_i2c_bus *i2cbus;
	int i2c_port;

	if(!data) {
		printk(KERN_ERR "tl880: null parameter given to %s\n", __FUNCTION__);
		return;
	}

	i2cbus = (struct tl880_i2c_bus *)data;
	i2c_port = i2cbus->busid;
	state = state ? 1 : 0;	/* Make sure state is only 1 or 0 */

	switch(i2c_port) {
		/* MyHD */
		default:
		case 0:
			write_regbits(i2cbus->dev, 0x10198, 0x1a, 0x1a, 0);
			write_regbits(i2cbus->dev, 0x10194, 0x1a, 0x1a, ~state);
			break;
		case 1:
			write_regbits(i2cbus->dev, 0x10198, 0x12, 0x12, state);
			break;
		case 2:
			write_regbits(i2cbus->dev, 0x10198, 0x17, 0x17, 0);
			write_regbits(i2cbus->dev, 0x10194, 0x17, 0x17, ~state);
			break;
		/* WinTV-HD */
		case 3:	// NxtWave_Flag != 0
			write_regbits(i2cbus->dev, 0x10194, 3, 3, 1);
			write_regbits(i2cbus->dev, 0x10198, 3, 3, state);
			break;
		case 4: // NxtWave_Flag == 0
			write_regbits(i2cbus->dev, 0x10198, 0x1b, 0x1b, 0);
			write_regbits(i2cbus->dev, 0x10194, 0x1b, 0x1b, ~state);
			break;
		/* HiPix */
		case 5:
			write_regbits(i2cbus->dev, 0x10194, 0x1a, 0x1a, 1);
			write_regbits(i2cbus->dev, 0x10198, 0x1a, 0x1a, state);
			break;
	}

	if(state) {
		udelay(20);
	} else {
		udelay(10);
	}
}

static void tl880_i2c_set_sda(void *data, int state)
{
	struct tl880_i2c_bus *i2cbus;
	int i2c_port;

	if(!data) {
		printk(KERN_ERR "tl880: null parameter given to %s\n", __FUNCTION__);
		return;
	}

	i2cbus = (struct tl880_i2c_bus *)data;
	i2c_port = i2cbus->busid;
	state = state ? 1 : 0;	/* Make sure state is only 1 or 0 */

	switch(i2c_port) {
		/* MyHD */
		default:
		case 0:
			write_regbits(i2cbus->dev, 0x10198, 0x1b, 0x1b, 0);
			write_regbits(i2cbus->dev, 0x10194, 0x1b, 0x1b, ~state);
			break;
		case 1:
			write_regbits(i2cbus->dev, 0x10198, 0x11, 0x11, state);
			break;
		case 2:
			write_regbits(i2cbus->dev, 0x10198, 0x16, 0x16, 0);
			write_regbits(i2cbus->dev, 0x10194, 0x16, 0x16, ~state);
			break;
		/* WinTV-HD */
		case 3:	// NxtWave_Flag != 0
			write_regbits(i2cbus->dev, 0x10194, 1, 1, 1);
			write_regbits(i2cbus->dev, 0x10198, 1, 1, state);
			break;
		case 4: // NxtWave_Flag == 0
			write_regbits(i2cbus->dev, 0x10198, 0x1a, 0x1a, 0);
			write_regbits(i2cbus->dev, 0x10194, 0x1a, 0x1a, ~state);
			break;
		/* HiPix */
		case 5:
			write_regbits(i2cbus->dev, 0x10194, 0x1b, 0x1b, 1);
			write_regbits(i2cbus->dev, 0x10198, 0x1b, 0x1b, state);
			break;
	}

	if(state) {
		udelay(20);
	} else {
		udelay(10);
	}
}

static int tl880_i2c_get_scl(void *data)
{
	struct tl880_i2c_bus *i2cbus;
	unsigned long value;
	int i2c_port;
	
	if(!data) {
		printk(KERN_ERR "tl880: null parameter given to %s\n", __FUNCTION__);
		return -EINVAL;
	}

	i2cbus = (struct tl880_i2c_bus *)data;
	i2c_port = i2cbus->busid;

	switch(i2c_port) {
		/* MyHD */
		default:
		case 0:
			write_regbits(i2cbus->dev, 0x10194, 0x1a, 0x1a, 0);
			value = read_regbits(i2cbus->dev, 0x1019c, 0x1a, 0x1a);
			break;
		case 1:
			value = read_regbits(i2cbus->dev, 0x1019c, 0x10, 0x10);
			break;
		case 2:
			write_regbits(i2cbus->dev, 0x10194, 0x17, 0x17, 0);
			value = read_regbits(i2cbus->dev, 0x1019c, 0x17, 0x17);
			break;
		/* WinTV-HD */
		case 3: // NxtWave_Flag != 0
			write_regbits(i2cbus->dev, 0x10194, 3, 3, 0);
			value = read_regbits(i2cbus->dev, 0x1019c, 3, 3);
			break;
		case 4: // NxtWave_Flag == 0
			write_regbits(i2cbus->dev, 0x10194, 0x1b, 0x1b, 0);
			value = read_regbits(i2cbus->dev, 0x1019c, 0x1b, 0x1b);
			break;
		/* HiPix */
		case 5:
			write_regbits(i2cbus->dev, 0x10194, 0x1a, 0x1a, 0);
			value = read_regbits(i2cbus->dev, 0x1019c, 0x1a, 0x1a);
			break;
	}

	return value ? 1 : 0;
}

static int tl880_i2c_get_sda(void *data)
{
	struct tl880_i2c_bus *i2cbus;
	unsigned long value;
	int i2c_port;

	if(!data) {
		printk(KERN_ERR "tl880: null parameter given to %s\n", __FUNCTION__);
		return -EINVAL;
	}

	i2cbus = (struct tl880_i2c_bus *)data;
	i2c_port = i2cbus->busid;

	switch(i2c_port) {
		/* MyHD */
		default:
		case 0:
			write_regbits(i2cbus->dev, 0x10194, 0x1b, 0x1b, 0);
			value = read_regbits(i2cbus->dev, 0x1019c, 0x1b, 0x1b);
			break;
		case 1:
			value = read_regbits(i2cbus->dev, 0x1019c, 0x10, 0x10);
			break;
		case 2:
			write_regbits(i2cbus->dev, 0x10194, 0x16, 0x16, 0);
			value = read_regbits(i2cbus->dev, 0x1019c, 0x16, 0x16);
			break;
		/* WinTV-HD */
		case 3: // NxtWave_Flag != 0
			value = read_regbits(i2cbus->dev, 0x1019c, 0, 0);
			break;
		case 4: // NxtWave_Flag == 0
			write_regbits(i2cbus->dev, 0x10194, 0x1a, 0x1a, 0);
			value = read_regbits(i2cbus->dev, 0x1019c, 0x1a, 0x1a);
			break;
		/* HiPix */
		case 5:
			write_regbits(i2cbus->dev, 0x10194, 0x1b, 0x1b, 0);
			value = read_regbits(i2cbus->dev, 0x1019c, 0x1b, 0x1b);
			break;
	}

	return value ? 1 : 0;
}

static void tl880_i2c_inc_use(struct i2c_adapter *adap)
{
	if(!adap) {
		printk(KERN_ERR "tl880: null parameter given to %s\n", __FUNCTION__);
	}

	MOD_INC_USE_COUNT;
}

static void tl880_i2c_dec_use(struct i2c_adapter *adap)
{
	if(!adap) {
		printk(KERN_ERR "tl880: null parameter given to %s\n", __FUNCTION__);
	}

	MOD_DEC_USE_COUNT;
}

static int tl880_i2c_attach_inform(struct i2c_client *client)
{
	if(!client) {
		printk(KERN_ERR "tl880: null parameter given to %s\n", __FUNCTION__);
	}

	printk(KERN_DEBUG "tl880: i2c client attach\n");
}

static int tl880_i2c_detach_inform(struct i2c_client *client)
{
	if(!client) {
		printk(KERN_ERR "tl880: null parameter given to %s\n", __FUNCTION__);
	}

	printk(KERN_DEBUG "tl880: i2c client detach\n");
}

static struct i2c_algo_bit_data tl880_i2c_algo_template = {
	setsda:  tl880_i2c_set_sda,
	setscl:  tl880_i2c_set_scl,
	getsda:  tl880_i2c_get_sda,
	getscl:  tl880_i2c_get_scl,
	udelay:  10,
	mdelay:  10,
	timeout: 200
};

#define I2C_HW_B_TL880	0xdd
#define I2C_ALGO_TL880	0x0b0000

static int tl880_i2c_xfer(struct i2c_adapter *adap, struct i2c_msg msgs[], int num)
{
	struct tl880_i2c_bus *i2cbus = adap->algo_data;
	int i;
	
	// printk(KERN_DEBUG "tl880: %i i2c transfers for bus %i:\n", num, i2cbus->busid);

	for(i = 0; i < num; i++) {
		if(msgs[i].flags & I2C_M_RD) {
			// printk(KERN_DEBUG "\tmsg %i, read, addr %i, flags 0x%04x, len %i\n", i, msgs[i].addr, msgs[i].flags, msgs[i].len);
			if(i2c_read(i2cbus, msgs[i].addr << 1 | 1, 0, 0, msgs[i].buf, msgs[i].len)) {
				return -EREMOTEIO;
			}
		} else {
			// printk(KERN_DEBUG "\tmsg %i, write, addr %i, flags 0x%04x, len %i\n", i, msgs[i].addr, msgs[i].flags, msgs[i].len);
			if(i2c_write(i2cbus, msgs[i].addr << 1 & ~1, 0, 0, msgs[i].buf, msgs[i].len)) {
				return -EREMOTEIO;
			}
		}
	}	
	
	return i;
}

static int tl880_i2c_control(struct i2c_adapter *adap, unsigned int cmd, unsigned long arg)
{
	return 0;
}

static u32 tl880_i2c_func(struct i2c_adapter *adap)
{
	return I2C_FUNC_SMBUS_EMUL;
}

static struct i2c_algorithm tl880_i2c_algo = {
	"tl880 algorithm",
	I2C_ALGO_TL880,
	tl880_i2c_xfer,
	NULL,
	NULL,
	NULL,
	tl880_i2c_control,
	tl880_i2c_func
};

static struct i2c_adapter tl880_i2c_adap_template = {
	name:			"tl880",
	id:			I2C_HW_B_TL880,
	inc_use:		tl880_i2c_inc_use,
	dec_use:		tl880_i2c_dec_use,
	client_register:	tl880_i2c_attach_inform,
	client_unregister:	tl880_i2c_detach_inform
};

static int tl880_i2c_add_bus(struct i2c_adapter *adap)
{
	struct tl880_i2c_bus *i2cbus = adap->algo_data;

	adap->id |= tl880_i2c_algo.id;
	adap->algo = &tl880_i2c_algo;
	adap->timeout = 100;
	adap->retries = 3;

	printk(KERN_DEBUG "tl880: Registering tl880 i2c adapter with bus ID %i card %i\n", i2cbus->busid, i2cbus->dev->id);
	
	i2c_add_adapter(adap);

	return 0;
}

static int tl880_i2c_del_bus(struct i2c_adapter *adap)
{
	struct tl880_i2c_bus *i2cbus = adap->algo_data;
	int retval;

	printk(KERN_DEBUG "tl880: Unregistering tl880 i2c adapter with bus ID %i card %i\n", i2cbus->busid, i2cbus->dev->id);

	if((retval = i2c_del_adapter(adap)) < 0) {
		return retval;
	}

	return 0;
}

int tl880_init_i2c(struct tl880_dev *tl880dev)
{
	int i, ret = 0;
	
	if(!tl880dev) {
		printk(KERN_ERR "tl880: null parameter given to %s\n", __FUNCTION__);
		return -EINVAL;
	}

	printk(KERN_DEBUG "tl880: initializing i2c\n");

	switch(tl880dev->card_type) {
		case TL880_CARD_HIPIX:
			tl880dev->minbus = 5;
			tl880dev->maxbus = 5;
			break;
		case TL880_CARD_WINTV_HD:
			tl880dev->minbus = 3;
			tl880dev->maxbus = 4;
			break;
		case TL880_CARD_MYHD_MDP120:
		case TL880_CARD_MYHD_MDP110:
			tl880dev->minbus = 0;
			tl880dev->maxbus = 2;
			break;
		case TL880_CARD_MYHD_MDP100:
		case TL880_CARD_MYHD_MDP100A:
			tl880dev->minbus = 0;
			tl880dev->maxbus = 1;
			break;
		default:
			tl880dev->minbus = 5;
			tl880dev->maxbus = 5;
	}
	
	tl880dev->i2cbuses = kmalloc(sizeof(struct tl880_i2c_bus) * (tl880dev->maxbus - tl880dev->minbus + 1), GFP_KERNEL);

	for(i = tl880dev->minbus; i <= tl880dev->maxbus && !ret; i++) {
		struct tl880_i2c_bus *bus = tl880dev->i2cbuses;
		int j = i - tl880dev->minbus;
		bus[j].busid = i;
		memcpy(&bus[j].i2c_adap, &tl880_i2c_adap_template, sizeof(struct i2c_adapter));
		memcpy(&bus[j].i2c_algo, &tl880_i2c_algo_template, sizeof(struct i2c_algo_bit_data));
		bus[j].dev = tl880dev;
		
		/* 32 - length("tl880") = 27 */
		snprintf(bus[j].i2c_adap.name+strlen(bus[j].i2c_adap.name), 27, " %i-%i", tl880dev->id, bus[j].busid);
		
		bus[j].i2c_algo.data = &bus[j];
		bus[j].i2c_adap.data = &bus[j];
		bus[j].i2c_adap.algo_data = &bus[j];
		
		tl880_i2c_set_scl(&bus[j], 0);
		tl880_i2c_set_sda(&bus[j], 0);

		ret = tl880_i2c_add_bus(&bus[j].i2c_adap);
	}

	return ret;
}

void tl880_deinit_i2c(struct tl880_dev *tl880dev)
{
	int i;

	if(!tl880dev) {
		printk(KERN_ERR "tl880: null parameter given to %s\n", __FUNCTION__);
		return;
	}

	printk(KERN_DEBUG "tl880: deinitializing i2c\n");

	for(i = tl880dev->minbus; i <= tl880dev->maxbus; i++) {
		tl880_i2c_del_bus(&tl880dev->i2cbuses[i - tl880dev->minbus].i2c_adap);
	}

	kfree(tl880dev->i2cbuses);
	tl880dev->i2cbuses = NULL;
	tl880dev->minbus = 0;
	tl880dev->maxbus = 0;
}

/* 
 * Imitation of I2C methods used in Windows driver (using i2c-algo-bit.c and
 * other sources as references)
 */

int i2c_write(struct tl880_i2c_bus *i2cbus, unsigned char addr, int bregsel, unsigned char reg, unsigned char *data, unsigned int count)
{
	// lock
	return do_i2c_write(i2cbus, addr, bregsel, reg, data, count);
	// unlock
}

int i2c_read(struct tl880_i2c_bus *i2cbus, unsigned char addr, int bregsel, unsigned char reg, unsigned char *data, unsigned int count)
{
	// lock
	return do_i2c_read(i2cbus, addr, bregsel, reg, data, count);
	// unlock
}

int i2c_write8(struct tl880_i2c_bus *i2cbus, unsigned char addr, int reg, unsigned char data)
{
	if(reg < 0) {
		return i2c_write(i2cbus, addr, 0, 0, &data, 1);
	} else {
		return i2c_write(i2cbus, addr, 1, reg, &data, 1);
	}
}

int i2c_write16(struct tl880_i2c_bus *i2cbus, unsigned char addr, int reg, unsigned short data)
{
	unsigned char cdata[2] = {data >> 8, data & 0xff};

	if(reg < 0) {
		return i2c_write(i2cbus, addr, 0, 0, cdata, 2);
	} else {
		return i2c_write(i2cbus, addr, 1, reg, cdata, 2);
	}
}

int i2c_read8(struct tl880_i2c_bus *i2cbus, unsigned char addr, int reg, unsigned char *data)
{
	if(!data) {
		return -EFAULT;
	}
	
	if(reg < 0) {
		return i2c_read(i2cbus, addr, 0, 0, data, 1);
	} else {
		return i2c_read(i2cbus, addr, 1, reg, data, 1);
	}
}

int i2c_read16(struct tl880_i2c_bus *i2cbus, unsigned char addr, int reg, unsigned short *data)
{
	int retval;
	unsigned char cdata[2] = {0, 0}; /* To prevent "uninitalized" warning */

	if(!data) {
		return -EFAULT;
	}
	
	if(reg < 0) {
		retval = i2c_read(i2cbus, addr, 0, 0, cdata, 2);
	} else {
		retval = i2c_read(i2cbus, addr, 1, reg, cdata, 2);
	}

	*data = (cdata[0] << 8) | cdata[1];

	return retval;
}


int do_i2c_write(struct tl880_i2c_bus *i2cbus, unsigned char addr, int bregsel, unsigned char reg, unsigned char *data, unsigned int count)
{
	int i;
	
	/*
	if(card_type == MDP110B) {
		if(addr & 0xf8 == 0xc0) {
			i2c_port = 2;
		} else {
			if((addr & 0xfe) == 0x14) {
				i2c_port = 1;
			} else {
				i2c_port = 0;
			}
		}
	} else {
		i2c_port = 0;
	}
	*/

	if(addr == 0xff) {
		if(count == 2) {
			udelay((-(*data) << 8) - *(data + 1));
			return 0;
		} else {
			return 1;
		}
	} else {
		/* Send start bit to I2C bus */
		do_i2c_start_bit(i2cbus);

		/* Write address to I2C bus */
		if(do_i2c_write_byte(i2cbus, addr & ~1)) {
			do_i2c_stop_bit(i2cbus);
			return -1;
		}
		
		/* Select register or write first byte */
		if(bregsel) {
			if(do_i2c_write_byte(i2cbus, reg)) {
				do_i2c_stop_bit(i2cbus);
				return -1;
			}
		}
		
		/* Write (remaining) data */
		if(count && data) {
			for(i = 0; i < count; i++) {
				if(do_i2c_write_byte(i2cbus, data[i])) {
					do_i2c_stop_bit(i2cbus);
					return -99;
				}
			}
		}

		/* Send stop bit to I2C bus */
		do_i2c_stop_bit(i2cbus);
	}

	return 0;
}

void do_i2c_start_bit(struct tl880_i2c_bus *i2cbus)
{
	/* Make sure clock is low */
	tl880_i2c_set_scl((void *)i2cbus, 0);

	/* Set data high then set clock high */
	tl880_i2c_set_sda((void *)i2cbus, 1);
	tl880_i2c_set_scl((void *)i2cbus, 1);

	/* Set data low */
	tl880_i2c_set_sda((void *)i2cbus, 0);

	/* Delay 10 microseconds */
	if(i2cbus->busid != 1) {
		udelay(10);
	}

	/* Set clock low */
	tl880_i2c_set_scl((void *)i2cbus, 0);
}

void do_i2c_stop_bit(struct tl880_i2c_bus *i2cbus)
{
	/* Make sure clock is low */
	tl880_i2c_set_scl((void *)i2cbus, 0);

	/* Set data low then set clock high */
	tl880_i2c_set_sda((void *)i2cbus, 0);
	tl880_i2c_set_scl((void *)i2cbus, 1);

	/* Set data high */
	tl880_i2c_set_sda((void *)i2cbus, 1);

	/* Delay 10 microseconds */
	if(i2cbus->busid != 1) {
		udelay(10);
	}

	/* Check for NACK */
	if(!tl880_i2c_get_sda((void *)i2cbus)) {
		// printk(KERN_DEBUG "tl880: NAK\n");
	}	

	/* Set clock low */
	tl880_i2c_set_scl((void *)i2cbus, 0);
}

int do_i2c_write_bit(struct tl880_i2c_bus *i2cbus, int bit)
{
	/* Set clock low and data to the bit being sent */
	tl880_i2c_set_scl((void *)i2cbus, 0);
	tl880_i2c_set_sda((void *)i2cbus, bit ? 1 : 0);

	tl880_i2c_set_scl((void *)i2cbus, 1);

	if(i2cbus->busid != 1) {
		udelay(10);
	}

	tl880_i2c_set_scl((void *)i2cbus, 0);

	return 0;
}

int do_i2c_write_byte(struct tl880_i2c_bus *i2cbus, unsigned char data)
{
	unsigned char mask;
	int retval;

	for(mask = 0x80; mask; mask >>= 1) {
		if(do_i2c_write_bit(i2cbus, (data & mask) ? 1 : 0)) {
			return -1;
		}
	}

	/* Get ack */
	tl880_i2c_set_sda(i2cbus, 1);
	tl880_i2c_set_scl(i2cbus, 1);
	retval = tl880_i2c_get_sda(i2cbus);

	tl880_i2c_set_scl(i2cbus, 0);

	// printk("sda %i: %s\n", retval, retval ? "noack" : "ack");

	return retval ? -1 : 0;
}

int do_i2c_write_byte_and_wait(struct tl880_i2c_bus *i2cbus, unsigned char data)
{
	int retval, i, mask;

	for(mask = 0x80; mask; mask >>= 1) {
		if(do_i2c_write_bit(i2cbus, (data & mask) ? 1 : 0)) {
			return -1;
		}
	}

	tl880_i2c_set_sda(i2cbus, 1);
	tl880_i2c_set_scl(i2cbus, 1);

	/* Wait for a while in case a device holds SCL */
	for(i = 0; i < 1024; i++) {
		if(tl880_i2c_get_scl(i2cbus)) {
			break;
		}

		udelay(10);
	}

	/* If we gave up waiting for SCL */
	if(i == 1024) {
		printk(KERN_DEBUG "tl880: timeout on I2C bus %i\n", i2cbus->busid);
		tl880_i2c_set_scl(i2cbus, 0);
		return -2;
	}

	retval = tl880_i2c_get_sda(i2cbus);

	tl880_i2c_set_scl(i2cbus, 0);

	return retval ? -1 : 0;
}

int do_i2c_read(struct tl880_i2c_bus *i2cbus, unsigned char addr, int bregsel, unsigned char reg, unsigned char *data, unsigned int count)
{
	int i;
	
	/*
	if(card_type == MDP110B) {
		if(addr & 0xf8 == 0xc0) {
			i2c_port = 2;
		} else {
			if((addr & 0xfe) == 0x14) {
				i2c_port = 1;
			} else {
				i2c_port = 0;
			}
		}
	} else {
		i2c_port = 0;
	}
	*/

	if(addr == 0xff) {
		if(count == 2) {
			udelay((-(*data) << 8) - *(data + 1));
			return 0;
		} else {
			return 1;
		}
	} else {
		if(bregsel) {
			addr &= ~1;
		} else {
			addr |= 1;
		}
		/* Send start bit to I2C bus */
		do_i2c_start_bit(i2cbus);

		/* Write address to I2C bus */
		if(do_i2c_write_byte(i2cbus, addr)) {
			do_i2c_stop_bit(i2cbus);
			return -1;
		}
		
		/* Select register */
		if(bregsel) {
			if(do_i2c_write_byte(i2cbus, reg)) {
				do_i2c_stop_bit(i2cbus);
				return -1;
			}
			do_i2c_start_bit(i2cbus);
			do_i2c_write_byte(i2cbus, addr | 1);
		}
		
		/* Read data */
		if(count && data) {
			for(i = 0; i < count; i++) {
				if(do_i2c_read_byte(i2cbus, &data[i])) {
					do_i2c_stop_bit(i2cbus);
					return -1;
				}
				/* Send ACK (except after last byte) */
				do_i2c_write_bit(i2cbus, (i == count - 1) ? 0 : 1);
			}
		}

		/* Send stop bit to I2C bus */
		do_i2c_stop_bit(i2cbus);
	}

	return 0;
}

int do_i2c_read_byte(struct tl880_i2c_bus *i2cbus, unsigned char *data)
{
	unsigned char d = 0;
	int i;

	if(!data) {
		return -EFAULT;
	}

	tl880_i2c_set_scl(i2cbus, 0);
	tl880_i2c_set_sda(i2cbus, 1);

	for(i = 0; i < 8; i++) {
		tl880_i2c_set_scl(i2cbus, 1);
		if(i2cbus->busid != 1) {
			udelay(10);
		}
		
		d <<= 1;
		d |= tl880_i2c_get_sda(i2cbus);
		
		tl880_i2c_set_scl(i2cbus, 0);
		if(i2cbus->busid != 1) {
			udelay(10);
		}
	}

	*data = d;

	return 0;
}


