/* 
 * I2C for TL880-based cards
 *
 * Based in part on the BT848 I2C interface in drivers/media/video/bttv-if.c
 *
 * (c) 2003-2007 Mike Bourgeous <nitrogen at users.sourceforge.net>
 *
 * $Log: tl880i2c.c,v $
 * Revision 1.17  2007/03/29 09:01:20  nitrogen
 * Partial MSP init now working, with correct sequence (after MSP3400 I2C attach)
 *
 * Revision 1.16  2007/03/29 08:38:54  nitrogen
 * Initial MSP configuration support.
 *
 * Revision 1.15  2007/03/26 19:55:25  nitrogen
 * Made MDP-130 I2C selection more sensible.
 *
 * Revision 1.14  2007/03/26 19:25:06  nitrogen
 * Added CVS log generation and updated copyrights and e-mail addresses.
 *
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

	/* TODO: replace this with an array of function pointers initialized according to card type? */
	switch(i2c_port) {
		/* MyHD */
		default:
		case 0:
			tl880_write_regbits(i2cbus->dev, 0x10198, 0x1a, 0x1a, 0);
			tl880_write_regbits(i2cbus->dev, 0x10194, 0x1a, 0x1a, ~state);
			break;
		case 1:
			tl880_write_regbits(i2cbus->dev, 0x10194, 0x12, 0x12, 1);
			tl880_write_regbits(i2cbus->dev, 0x10198, 0x12, 0x12, state);
			break;
		case 2:
			tl880_write_regbits(i2cbus->dev, 0x10198, 0x17, 0x17, 0);
			tl880_write_regbits(i2cbus->dev, 0x10194, 0x17, 0x17, ~state);
			break;
		/* WinTV-HD */
		case 3:	// NxtWave_Flag != 0
			tl880_write_regbits(i2cbus->dev, 0x10194, 3, 3, 1);
			tl880_write_regbits(i2cbus->dev, 0x10198, 3, 3, state);
			break;
		case 4: // NxtWave_Flag == 0
			tl880_write_regbits(i2cbus->dev, 0x10194, 0x1b, 0x1b, 1);
			tl880_write_regbits(i2cbus->dev, 0x10198, 0x1b, 0x1b, state);
			break;
		/* HiPix */
		case 5:
			tl880_write_regbits(i2cbus->dev, 0x10194, 0x1a, 0x1a, 1);
			tl880_write_regbits(i2cbus->dev, 0x10198, 0x1a, 0x1a, state);
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
			tl880_write_regbits(i2cbus->dev, 0x10198, 0x1b, 0x1b, 0);
			tl880_write_regbits(i2cbus->dev, 0x10194, 0x1b, 0x1b, ~state);
			break;
		case 1:
			tl880_write_regbits(i2cbus->dev, 0x10198, 0x11, 0x11, state);
			break;
		case 2:
			tl880_write_regbits(i2cbus->dev, 0x10198, 0x16, 0x16, 0);
			tl880_write_regbits(i2cbus->dev, 0x10194, 0x16, 0x16, ~state);
			break;
		/* WinTV-HD */
		case 3:	// NxtWave_Flag != 0
			tl880_write_regbits(i2cbus->dev, 0x10194, 1, 1, 1);
			tl880_write_regbits(i2cbus->dev, 0x10198, 1, 1, state);
			break;
		case 4: // NxtWave_Flag == 0
			tl880_write_regbits(i2cbus->dev, 0x10194, 0x1a, 0x1a, 1);
			tl880_write_regbits(i2cbus->dev, 0x10198, 0x1a, 0x1a, state);
			break;
		/* HiPix */
		case 5:
			tl880_write_regbits(i2cbus->dev, 0x10194, 0x1b, 0x1b, 1);
			tl880_write_regbits(i2cbus->dev, 0x10198, 0x1b, 0x1b, state);
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
			tl880_write_regbits(i2cbus->dev, 0x10194, 0x1a, 0x1a, 0);
			value = tl880_read_regbits(i2cbus->dev, 0x1019c, 0x1a, 0x1a);
			break;
		case 1:
			value = tl880_read_regbits(i2cbus->dev, 0x1019c, 0x12, 0x12);
			break;
		case 2:
			tl880_write_regbits(i2cbus->dev, 0x10194, 0x17, 0x17, 0);
			value = tl880_read_regbits(i2cbus->dev, 0x1019c, 0x17, 0x17);
			break;
		/* WinTV-HD */
		case 3: // NxtWave_Flag != 0
			tl880_write_regbits(i2cbus->dev, 0x10194, 3, 3, 0);
			value = tl880_read_regbits(i2cbus->dev, 0x1019c, 3, 3);
			break;
		case 4: // NxtWave_Flag == 0
			tl880_write_regbits(i2cbus->dev, 0x10194, 0x1b, 0x1b, 0);
			value = tl880_read_regbits(i2cbus->dev, 0x1019c, 0x1b, 0x1b);
			break;
		/* HiPix */
		case 5:
			tl880_write_regbits(i2cbus->dev, 0x10194, 0x1a, 0x1a, 0);
			value = tl880_read_regbits(i2cbus->dev, 0x1019c, 0x1a, 0x1a);
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
			tl880_write_regbits(i2cbus->dev, 0x10194, 0x1b, 0x1b, 0);
			value = tl880_read_regbits(i2cbus->dev, 0x1019c, 0x1b, 0x1b);
			break;
		case 1:
			value = tl880_read_regbits(i2cbus->dev, 0x1019c, 0x10, 0x10);
			break;
		case 2:
			tl880_write_regbits(i2cbus->dev, 0x10194, 0x16, 0x16, 0);
			value = tl880_read_regbits(i2cbus->dev, 0x1019c, 0x16, 0x16);
			break;
		/* WinTV-HD */
		case 3: // NxtWave_Flag != 0
			value = tl880_read_regbits(i2cbus->dev, 0x1019c, 0, 0);
			break;
		case 4: // NxtWave_Flag == 0
			tl880_write_regbits(i2cbus->dev, 0x10194, 0x1a, 0x1a, 0);
			value = tl880_read_regbits(i2cbus->dev, 0x1019c, 0x1a, 0x1a);
			break;
		/* HiPix */
		case 5:
			tl880_write_regbits(i2cbus->dev, 0x10194, 0x1b, 0x1b, 0);
			value = tl880_read_regbits(i2cbus->dev, 0x1019c, 0x1b, 0x1b);
			break;
	}

	return value ? 1 : 0;
}

/* Based on bttv_call_i2c_clients from bttv driver */
int tl880_call_i2c_clients(struct tl880_dev *tl880dev, unsigned int cmd, void *arg)
{
	int i;
	int j;
	int result;
	unsigned long oldarg = 0;
	int done = 0;

	if(!tl880dev) {
		printk(KERN_ERR "tl880: null tl880dev in tl880_call_i2c_clients\n");
		return -EINVAL;
	}

	if(!tl880dev->i2cbuses) {
		printk(KERN_ERR "tl880: no i2cbuses found in tl880_call_i2c_clients on card %d\n", tl880dev->id);
		return -ENOENT;
	}

	if(arg != NULL) {
		oldarg = *(unsigned long *)arg;
	}

	printk(KERN_DEBUG "tl880: sending command to i2c clients: ");
	v4l_printk_ioctl(cmd);

	for(j = tl880dev->minbus; j <= tl880dev->maxbus; j++) {
		for(i = 0; i < I2C_CLIENTS_MAX; i++) {
			if (!tl880dev->i2cbuses[j].i2c_clients[i]) {
				continue;
			}
			if (!tl880dev->i2cbuses[j].i2c_clients[i]->driver->command) {
				continue;
			}
			printk(KERN_DEBUG "tl880: sending command 0x%08x to i2c client %s (id %d, bus %d)\n", 
					cmd, tl880dev->i2cbuses[j].i2c_clients[i]->name, i, j);
			result = tl880dev->i2cbuses[j].i2c_clients[i]->driver->command(tl880dev->i2cbuses[j].i2c_clients[i], cmd, arg);
			if(result) {
				printk(KERN_INFO "tl880: \tnonzero return from i2c client %s (id %d, bus %d): %d\n", 
					tl880dev->i2cbuses[j].i2c_clients[i]->name, i, j, result);
				/* done = 1; */
			}
			if(arg != NULL && *(unsigned long *)arg != oldarg) {
				printk(KERN_INFO "tl880: \targument changed by i2c client %s (id %d, bus %d): from 0x%lx to 0x%lx\n", 
					tl880dev->i2cbuses[j].i2c_clients[i]->name, i, j, oldarg, *(unsigned long *)arg);
				/* done = 1; */
				*(unsigned long *)arg = oldarg;
			}
			if(done) {
				return result;
			}
		}
	}

	return 0;
}


static int tl880_i2c_attach_inform(struct i2c_client *client)
{
	struct tl880_i2c_bus *i2cbus;
	struct tl880_dev *tl880dev;
	int client_no;
	int ret = 0; /* Ret is to be used only for this function's return value */

	if(CHECK_NULL(client)) {
		printk(KERN_ERR "tl880: null parameter given to %s\n", __FUNCTION__);
		return -EINVAL;
	}

	/* Get card-specific data */
	i2cbus = i2c_get_adapdata(client->adapter);
	if(CHECK_NULL(i2cbus)) {
		printk(KERN_ERR "tl880: Couldn't get tl880_i2c_bus pointer from i2c adapter\n");
		return -EINVAL;
	}
	tl880dev = i2cbus->dev;
	if(CHECK_NULL(tl880dev)) {
		printk(KERN_ERR "tl880: Couldn't get tl880_dev pointer from i2c adapter\n");
		return -EINVAL;
	}

	printk(KERN_DEBUG "tl880: I2C client %s has attached to address %x of bus %d on card %d\n", client->name, client->addr, i2cbus->busno, tl880dev->id);
	if(client->driver != NULL && client->driver->driver.name != NULL) {
		printk(KERN_DEBUG "tl880: I2C client driver is %s (id %d)\n", client->driver->driver.name, client->driver->id);
	}

	for(client_no = 0; client_no < I2C_CLIENTS_MAX; client_no++) {
		if(!i2cbus->i2c_clients[client_no]) {
			i2cbus->i2c_clients[client_no] = client;
			break;
		}
	}

	if(client_no == I2C_CLIENTS_MAX) {
		printk(KERN_ERR "tl880: out of I2C client spaces\n");
	} else {
		printk(KERN_DEBUG "tl880: I2C client index is %d\n", client_no);
	}
	
	if(client->driver->id == I2C_DRIVERID_TUNER) {
		struct tuner *tuner;
		struct tuner_setup tun_setup;
		int tuner_type, cmdval;

		/* See if this is the VPX's address */
		if(client->addr == 0x43) {
			printk(KERN_INFO "tl880: rejecting tuner on probable VPX address\n");
			ret = -ENODEV;
		}

		tun_setup.mode_mask = T_ANALOG_TV | T_DIGITAL_TV;
		tun_setup.addr = client->addr;

		switch(tl880dev->card_type) {
			case TL880_CARD_MYHD_MDP100A:
			case TL880_CARD_MYHD_MDP100:
			case TL880_CARD_MYHD_MDP110:
			case TL880_CARD_MYHD_MDP120:
			case TL880_CARD_MYHD_MDP130:
				tun_setup.type = tuner_type = TUNER_PHILIPS_ATSC;
				client->driver->command(client, TUNER_SET_TYPE_ADDR, &tun_setup);
				break;
			default:
				break;
		}

		cmdval = 61250 * 16 / 1000; /* US channel 3 */
		client->driver->command(client, VIDIOCSFREQ, &cmdval);
		tuner = i2c_get_clientdata(client);

		printk(KERN_INFO "tl880: Tuner info: type: %u, freq: %u (want %u), has_signal: %d\n",
				tuner->type, tuner->tv_freq, cmdval,
				tuner->has_signal ? tuner->has_signal(client) : -1);
	} else if(client->driver->id == I2C_DRIVERID_MSP3400 || client->driver->id == I2C_DRIVERID_TVAUDIO) {
		struct video_audio audio_state;

		client->driver->command(client, VIDIOCGAUDIO, &audio_state);

		tl880dev->msp_i2cclient = client_no;
		
		//tl880_msp_config(tl880dev);
		// Don't init msp here - msp driver isn't initialized apparently

		printk(KERN_INFO "tl880: I2C audio client: name: \"%s\", channel: %d, flags: 0x%08x\n",
			audio_state.name ? audio_state.name : "", audio_state.audio, audio_state.flags);
		printk(KERN_INFO "tl880: I2C audio client: vol: %u, bal: %u, bas: %u, trb: %u\n",
			audio_state.volume, audio_state.balance,
			audio_state.bass, audio_state.treble);
	}

	if(ret) {
		client->driver->detach_client(client);
	}
	return ret;
}

static int tl880_i2c_detach_inform(struct i2c_client *client)
{
	/*
	struct tl880_i2c_bus *i2cbus = (struct tl880_i2c_bus *)client->adapter->data;
	struct tl880_dev *tl880dev = (struct tl880_dev *)i2cbus->dev;
	*/
	struct tl880_i2c_bus *i2cbus;
	struct tl880_dev *tl880dev;
	int i;

	if(CHECK_NULL(client)) {
		printk(KERN_ERR "tl880: null parameter given to %s\n", __FUNCTION__);
		return -EINVAL;
	}

	/* Get card-specific data */
	i2cbus = i2c_get_adapdata(client->adapter);
	if(CHECK_NULL(i2cbus)) {
		printk(KERN_ERR "tl880: Couldn't get i2cbus pointer from i2c adapter\n");
		return -EINVAL;
	}
	tl880dev = i2cbus->dev;
	if(CHECK_NULL(tl880dev)) {
		printk(KERN_ERR "tl880: Couldn't get tl880dev pointer from i2c adapter\n");
		return -EINVAL;
	}

	printk(KERN_DEBUG "tl880: i2c client %s detach from bus %d-%d\n", client->name, tl880dev->id, i2cbus->busno);

	/* Remove this client's entry from the bus's list of clients */
	for(i = 0; i < I2C_CLIENTS_MAX; i++) {
		if(i2cbus->i2c_clients[i] == client) {
			i2cbus->i2c_clients[i] = NULL;
		}
	}

	return 0;
}

static struct i2c_algo_bit_data tl880_i2c_algo_template = {
	setsda:  tl880_i2c_set_sda,
	setscl:  tl880_i2c_set_scl,
	getsda:  tl880_i2c_get_sda,
	getscl:  tl880_i2c_get_scl,
	udelay:  25,
	timeout: (HZ / 5)
};

static struct i2c_adapter tl880_i2c_adap_template = {
	name:			"tl880",
	id:			I2C_HW_B_TL880,
	class:			I2C_CLASS_TV_ANALOG | I2C_CLASS_TV_DIGITAL,
	client_register:	tl880_i2c_attach_inform,
	client_unregister:	tl880_i2c_detach_inform
};

int tl880_init_i2c(struct tl880_dev *tl880dev)
{
	int i, ret = 0, result = 0;
	
	if(CHECK_NULL(tl880dev)) {
		printk(KERN_ERR "tl880: null parameter given to %s\n", __FUNCTION__);
		return -EINVAL;
	}

	printk(KERN_DEBUG "tl880: Initializing i2c on card %d\n", tl880dev->id);

	if((result = request_module("i2c_algo_bit"))) {
		printk(KERN_WARNING "tl880: unable to request i2c_algo_bit module: %d\n", result);
	}
	if((result = request_module("i2c_dev"))) {
		printk(KERN_WARNING "tl880: unable to request i2c_dev module: %d\n", result);
	}

	switch(tl880dev->card_type) {
		case TL880_CARD_HIPIX:
			tl880dev->minbus = 5;
			tl880dev->maxbus = 5;
			break;
		case TL880_CARD_WINTV_HD:
			tl880dev->minbus = 3;
			tl880dev->maxbus = 4;
			break;
		case TL880_CARD_MYHD_MDP130:
		case TL880_CARD_MYHD_MDP120:
			tl880dev->minbus = 0;
			tl880dev->maxbus = 0;
			break;
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
			tl880dev->minbus = 0;
			tl880dev->maxbus = 0;
			break;
	}

	tl880dev->i2cbuses = kmalloc(sizeof(struct tl880_i2c_bus) * (tl880dev->maxbus - tl880dev->minbus + 1), GFP_KERNEL);
	
	for(i = tl880dev->minbus; i <= tl880dev->maxbus; i++) {
		struct tl880_i2c_bus *bus = tl880dev->i2cbuses;
		int j = i - tl880dev->minbus;
		int k;
		bus[j].busid = i;
		bus[j].busno = i - tl880dev->minbus;
		memcpy(&bus[j].i2c_adap, &tl880_i2c_adap_template, sizeof(struct i2c_adapter));
		memcpy(&bus[j].i2c_algo, &tl880_i2c_algo_template, sizeof(struct i2c_algo_bit_data));
		bus[j].dev = tl880dev;

		for(k = 0; k < I2C_CLIENTS_MAX; k++) {
			tl880dev->i2cbuses[i].i2c_clients[k] = NULL;
		}
		
		/* 32 - length("tl880") = 27 */
		snprintf(bus[j].i2c_adap.name+strlen(bus[j].i2c_adap.name), 27, " %d-%d", tl880dev->id, bus[j].busno);
		
		bus[j].i2c_algo.data = &bus[j];
		/* bus[j].i2c_adap.data = &bus[j]; */
		i2c_set_adapdata(&bus[j].i2c_adap, &bus[j]);
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

	for(i = tl880dev->minbus; i <= tl880dev->maxbus; i++) {
		i2c_bit_del_bus(&tl880dev->i2cbuses[i - tl880dev->minbus].i2c_adap);
	}

	kfree(tl880dev->i2cbuses);
	tl880dev->i2cbuses = NULL;
	tl880dev->minbus = 0;
	tl880dev->maxbus = 0;
}


/* I2C access functions for features not handled by external chip drivers - 
 * based on i2c_smbus_* functions in drivers/i2c/i2c-core.c
 */
int tl880_i2c_read_byte(struct tl880_i2c_bus *i2cbus, unsigned short addr)
{
	union i2c_smbus_data data;

	if(CHECK_NULL(i2cbus)) {
		return -EINVAL;
	}

	if(i2c_smbus_xfer(&(i2cbus->i2c_adap), addr, 0,
				I2C_SMBUS_READ,0,I2C_SMBUS_BYTE, &data)) {
		return -EIO;
	} else {
		return data.byte;
	}
}

int tl880_i2c_write_byte(struct tl880_i2c_bus *i2cbus, unsigned short addr, unsigned char value)
{
	if(CHECK_NULL(i2cbus)) {
		return -EINVAL;
	}

	return i2c_smbus_xfer(&(i2cbus->i2c_adap), addr, 0,
	                      I2C_SMBUS_WRITE, value, I2C_SMBUS_BYTE, NULL);
}

int tl880_i2c_read_byte_data(struct tl880_i2c_bus *i2cbus, unsigned short addr, unsigned char command)
{
	union i2c_smbus_data data;

	if(CHECK_NULL(i2cbus)) {
		return -EINVAL;
	}

	if (i2c_smbus_xfer(&(i2cbus->i2c_adap), addr, 0,
	                   I2C_SMBUS_READ,command, I2C_SMBUS_BYTE_DATA,&data)) {
		return -EIO;
	} else {
		return data.byte;
	}
}

int tl880_i2c_write_byte_data(struct tl880_i2c_bus *i2cbus, unsigned short addr, unsigned char command, unsigned char value)
{
	union i2c_smbus_data data;

	if(CHECK_NULL(i2cbus)) {
		return -EINVAL;
	}

	data.byte = value;
	return i2c_smbus_xfer(&(i2cbus->i2c_adap), addr, 0,
	                      I2C_SMBUS_WRITE,command,
	                      I2C_SMBUS_BYTE_DATA,&data);
}

int tl880_i2c_read_word_data(struct tl880_i2c_bus *i2cbus, unsigned short addr, unsigned char command)
{
	union i2c_smbus_data data;

	if(CHECK_NULL(i2cbus)) {
		return -EINVAL;
	}

	if (i2c_smbus_xfer(&(i2cbus->i2c_adap), addr, 0,
	                   I2C_SMBUS_READ,command, I2C_SMBUS_WORD_DATA, &data)) {
		return -EIO;
	} else {
		return data.word;
	}
}

int tl880_i2c_write_word_data(struct tl880_i2c_bus *i2cbus, unsigned short addr, unsigned char command, unsigned short value)
{
	union i2c_smbus_data data;

	if(CHECK_NULL(i2cbus)) {
		return -EINVAL;
	}

	data.word = value;
	return i2c_smbus_xfer(&(i2cbus->i2c_adap), addr, 0,
	                      I2C_SMBUS_WRITE,command,
	                      I2C_SMBUS_WORD_DATA,&data);
}


