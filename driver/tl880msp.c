/* 
 * MSP34x0G control for TL880-based cards (until msp3220 driver has needed features)
 *
 * Functions primarily based on msp3220 driver and VPX3226F datasheet.
 *
 * (c) 2007 Mike Bourgeous <nitrogen at users.sourceforge.net>
 *
 * $Log: tl880msp.c,v $
 * Revision 1.3  2007/03/29 09:27:40  nitrogen
 * Tweaked mkdev scripts, improved MSP init, new tool, improved tools makefile, more docs.
 *
 * Revision 1.2  2007/03/29 09:01:20  nitrogen
 * Partial MSP init now working, with correct sequence (after MSP3400 I2C attach)
 *
 * Revision 1.1  2007/03/29 08:38:54  nitrogen
 * Initial MSP configuration support.
 *
 */
#include "tl880.h"

static unsigned int gMspI2sMaster = 0;
static unsigned short gModus = 0;
static unsigned short gI2sMode = 0;
static unsigned short gScart1Volume = 0;
static unsigned short gInput = 0;

static unsigned int g_ntscOsFrames = 0;
static unsigned int g_ntscCapMode = 0;
static unsigned int g_starPtr = 0;

int tl880_msp_read(struct tl880_dev *tl880dev, unsigned char reg)
{
	if(CHECK_NULL(tl880dev)) {
		return -1;
	}

//	return tl880_i2c_read_byte_data(&tl880dev->i2cbuses[tl880dev->msp_i2cbus], tl880dev->msp_addr, reg);
	return -1;
}

// Modified from drivers/media/video/msp3400-driver.c
int tl880_msp_write(struct tl880_dev *tl880dev, int dev, int addr, int val)
{
	int err;
	u8 buffer[5];
	struct i2c_client *client;

	if(CHECK_NULL(tl880dev) || CHECK_NULL(tl880dev->i2cbuses)) {
		return -1;
	}

	if(TL_ASSERT(tl880dev->msp_i2cclient >= 0)) {
		return -1;
	}

	client = tl880dev->i2cbuses[tl880dev->msp_i2cbus].i2c_clients[tl880dev->msp_i2cclient];

	if(CHECK_NULL(client)) {
		return -1;
	}

	buffer[0] = dev;
	buffer[1] = addr >> 8;
	buffer[2] = addr &  0xff;
	buffer[3] = val  >> 8;
	buffer[4] = val  &  0xff;

	printk(KERN_DEBUG "tl880: msp: write\n");
	for (err = 0; err < 3; err++) {
		if (i2c_master_send(client, buffer, 5) == 5)
			break;
		printk(KERN_WARNING "tl880: msp: I/O error #%d (write 0x%02x/0x%02x)\n", err,
		       dev, addr);
		current->state = TASK_INTERRUPTIBLE;
		schedule_timeout(msecs_to_jiffies(10));
	}
	if (err == 3) {
		return -1;
	}
	printk(KERN_DEBUG "tl880: msp: finished write\n");
	return 0;
}


// msp_type should be array of size 2
unsigned short tl880_msp_init(struct tl880_dev *tl880dev, unsigned short *msp_type)
{
	unsigned short retval = 0;

	gModus = 0x83 | (gMspI2sMaster ? 0x20 : 0);
	gI2sMode = 0x320;
	gScart1Volume = 0xf300;
	gInput = 6;

	// Not in Windows
	tl880_msp_write(tl880dev, 0x10, 0x30, gModus);

#ifdef WILLNOTCOMPILE
	if(tl880_msp_reset(tl880dev)) {
		return -1;
	}

	retval = (((msp_type[0] = tl880_msp_read(tl880dev, 0x12, 0x1e)) == 0xffff) ? 1 : 0);
	retval |= (((msp_type[1] = tl880_msp_read(tl880dev, 0x12, 0x1f)) == 0xffff) ? 1 : 0);;
#endif

	return retval;
}

void tl880_msp_config(struct tl880_dev *tl880dev)
{
	unsigned short msp_type[2];
	struct v4l2_routing msp_routing;
	int standard;
	
	printk(KERN_DEBUG "tl880: Configuring the MSP chip\n");

#ifdef WILLNOTCOMPILE
	tl880_msp_set_std(tl880dev, /* gpJanus+16714 */ 2);
#endif

	memset(&msp_routing, 0, sizeof(msp_routing));

	// TODO: call only the MSP
	standard = V4L2_STD_NTSC_M;
	tl880_call_i2c_clients(tl880dev, VIDIOC_S_STD, (void *)&standard);
	tl880_call_i2c_clients(tl880dev, VIDIOC_INT_G_AUDIO_ROUTING, (void *)&msp_routing);

	printk(KERN_DEBUG "tl880: msp routing is %04x by %04x\n", 
			msp_routing.input, msp_routing.output);

	msp_routing.input = MSP_INPUT(MSP_IN_MUTE, MSP_IN_TUNER1, MSP_DSP_IN_TUNER, MSP_DSP_IN_TUNER);
	msp_routing.output = MSP_OUTPUT(MSP_SC_IN_MUTE);

	printk(KERN_DEBUG "tl880: setting msp routing to %04x x %04x\n", 
			msp_routing.input, msp_routing.output);

	tl880_call_i2c_clients(tl880dev, VIDIOC_INT_S_AUDIO_ROUTING, (void *)&msp_routing);

	tl880_msp_init(tl880dev, msp_type);
}

