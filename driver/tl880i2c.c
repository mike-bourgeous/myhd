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
			write_regbits(i2cbus->dev, 0x10194, 0x12, 0x12, 1);
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
			write_regbits(i2cbus->dev, 0x10194, 0x1b, 0x1b, 1);
			write_regbits(i2cbus->dev, 0x10198, 0x1b, 0x1b, state);
			break;
		/* HiPix */
		case 5:
			write_regbits(i2cbus->dev, 0x10194, 0x1a, 0x1a, 1);
			write_regbits(i2cbus->dev, 0x10198, 0x1a, 0x1a, state);
			break;
	}

	udelay(state ? 20 : 10);
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
			write_regbits(i2cbus->dev, 0x10194, 0x1a, 0x1a, 1);
			write_regbits(i2cbus->dev, 0x10198, 0x1a, 0x1a, state);
			break;
		/* HiPix */
		case 5:
			write_regbits(i2cbus->dev, 0x10194, 0x1b, 0x1b, 1);
			write_regbits(i2cbus->dev, 0x10198, 0x1b, 0x1b, state);
			break;
	}

	udelay(state ? 20 : 10);
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
			value = read_regbits(i2cbus->dev, 0x1019c, 0x12, 0x12);
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

/* Based on bttv_call_i2c_clients from bttv driver */
void tl880_call_i2c_clients(struct tl880_dev *tl880dev, unsigned int cmd, void *arg)
{
	int i;
	int j;
	
	for(i = 0; i < I2C_CLIENTS_MAX; i++) {
		for(j = tl880dev->minbus; j <= tl880dev->maxbus; j++) {
			if (!tl880dev->i2cbuses[j].i2c_clients[i]) {
				continue;
			}
			if (!tl880dev->i2cbuses[j].i2c_clients[i]->driver->command) {
				continue;
			}
			tl880dev->i2cbuses[j].i2c_clients[i]->driver->command(tl880dev->i2cbuses[j].i2c_clients[i], cmd, arg);
		}
	}
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
	struct tl880_i2c_bus *i2cbus = (struct tl880_i2c_bus *)client->adapter->data;
	struct tl880_dev *tl880dev = (struct tl880_dev *)i2cbus->dev;
	int i;

	printk(KERN_DEBUG "tl880: i2c client %s attach to bus %i-%i\n", client->name, tl880dev->id, i2cbus->busno);
	printk(KERN_DEBUG "tl880: driver %s (id %i)\n", client->driver->name, client->driver->id);

	for(i = 0; i < I2C_CLIENTS_MAX; i++) {
		if(!i2cbus->i2c_clients[i]) {
			i2cbus->i2c_clients[i] = client;
			break;
		}
	}
	
	if(client->driver->id == I2C_DRIVERID_TUNER) {
		int tuner_type;

		switch(tl880dev->board_type) {
			case TL880_CARD_MYHD_MDP100A:
			case TL880_CARD_MYHD_MDP100:
			case TL880_CARD_MYHD_MDP110:
			case TL880_CARD_MYHD_MDP120:
			default:
				tuner_type = 40;
		}
				
		client->driver->command(client, TUNER_SET_TYPE, &tuner_type);
	}

	return 0;
}

static int tl880_i2c_detach_inform(struct i2c_client *client)
{
	struct tl880_i2c_bus *i2cbus = (struct tl880_i2c_bus *)client->adapter->data;
	struct tl880_dev *tl880dev = (struct tl880_dev *)i2cbus->dev;

	if(!client) {
		printk(KERN_ERR "tl880: null parameter given to %s\n", __FUNCTION__);
	}

	printk(KERN_DEBUG "tl880: i2c client %s detach from bus %i-%i\n", client->name, tl880dev->id, i2cbus->busno);

	return 0;
}

static struct i2c_algo_bit_data tl880_i2c_algo_template = {
	setsda:  tl880_i2c_set_sda,
	setscl:  tl880_i2c_set_scl,
	getsda:  tl880_i2c_get_sda,
	getscl:  tl880_i2c_get_scl,
	udelay:  10,
	mdelay:  10, 
	timeout: 100
};

#define I2C_HW_B_TL880	0xdd

static struct i2c_adapter tl880_i2c_adap_template = {
	name:			"tl880",
	id:			I2C_HW_B_TL880,
	inc_use:		tl880_i2c_inc_use,
	dec_use:		tl880_i2c_dec_use,
	client_register:	tl880_i2c_attach_inform,
	client_unregister:	tl880_i2c_detach_inform
};

int tl880_init_i2c(struct tl880_dev *tl880dev)
{
	int i, ret = 0;
	
	if(!tl880dev) {
		printk(KERN_ERR "tl880: null parameter given to %s\n", __FUNCTION__);
		return -EINVAL;
	}

	printk(KERN_DEBUG "tl880: initializing i2c\n");

	switch(tl880dev->board_type) {
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
			break;
	}

	tl880dev->i2cbuses = kmalloc(sizeof(struct tl880_i2c_bus) * (tl880dev->maxbus - tl880dev->minbus + 1), GFP_KERNEL);
	
	for(i = tl880dev->minbus; i <= tl880dev->maxbus; i++) {
		struct tl880_i2c_bus *bus = tl880dev->i2cbuses;
		int j = i - tl880dev->minbus;
		bus[j].busid = i;
		bus[j].busno = i - tl880dev->minbus;
		memcpy(&bus[j].i2c_adap, &tl880_i2c_adap_template, sizeof(struct i2c_adapter));
		memcpy(&bus[j].i2c_algo, &tl880_i2c_algo_template, sizeof(struct i2c_algo_bit_data));
		bus[j].dev = tl880dev;
		
		/* 32 - length("tl880") = 27 */
		snprintf(bus[j].i2c_adap.name+strlen(bus[j].i2c_adap.name), 27, " %i-%i", tl880dev->id, bus[j].busno);
		
		bus[j].i2c_algo.data = &bus[j];
		bus[j].i2c_adap.data = &bus[j];
		bus[j].i2c_adap.algo_data = &bus[j].i2c_algo;
		
		tl880_i2c_set_scl(&bus[j], 0);
		tl880_i2c_set_sda(&bus[j], 0);

		ret = i2c_bit_add_bus(&bus[j].i2c_adap);
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
		i2c_bit_del_bus(&tl880dev->i2cbuses[i - tl880dev->minbus].i2c_adap);
	}

	kfree(tl880dev->i2cbuses);
	tl880dev->i2cbuses = NULL;
	tl880dev->minbus = 0;
	tl880dev->maxbus = 0;
}

