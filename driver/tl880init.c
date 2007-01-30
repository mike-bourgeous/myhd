/* 
 * Init stuff for TL880-based cards
 *
 * (c) 2003 Mike Bourgeous <nitrogen@slimetech.com>
 */
#include "tl880.h"

#if 0
/* From dvbdev.h */
#define dvb_attach(FUNCTION, ARGS...) ({ \
        void *__r = NULL; \
        typeof(&FUNCTION) __a = symbol_request(FUNCTION); \
        if (__a) { \
                __r = (void *) __a(ARGS); \
                if (__r == NULL) \
                        symbol_put(FUNCTION); \
        } else { \
                printk(KERN_ERR "DVB: Unable to find symbol "#FUNCTION"()\n"); \
        } \
        __r; \
})
/* Taken from drivers/media/dvb/b2c2/flexcop-fe-tuner.c */
static struct nxt200x_config nxt200x_demod_config = {
	        .demod_address    = 0x0a,
};
#endif

static void tl880_init_sdram(struct tl880_dev *tl880dev)
{
	const unsigned long wintvhd = 0x34302130;	/* WinTV-HD 2x2Mx32? */
	const unsigned long mdp100 = 0x34302130;	/* MyHD 2x2Mx32 */
	const unsigned long janus = 0x34302024;		/* Janus 8x1Mx16 */
	const unsigned long hipix = 0x34303140;		/* HiPix 8x2MB */
	const unsigned long other = 0x34303140;

	switch(tl880dev->card_type) {
		/* Telemann Systems Inc. */
		case TL880_CARD_HIPIX:
			tl880_write_register(tl880dev, 0x28000, hipix);
			break;
		/* Hauppauge Computer Works Inc. */
		case TL880_CARD_WINTV_HD:
			tl880_write_register(tl880dev, 0x28000, wintvhd);
			break;
		/* MIT Inc. */
		case TL880_CARD_MYHD_MDP130:
		case TL880_CARD_MYHD_MDP120:
		case TL880_CARD_MYHD_MDP110:
		case TL880_CARD_MYHD_MDP100:
		case TL880_CARD_MYHD_MDP100A:
		default:
			tl880_write_register(tl880dev, 0x28000, mdp100);
			break;
		/* TeraLogic Group */
		case TL880_CARD_JANUS:
			tl880_write_register(tl880dev, 0x28000, janus);
			break;
		case TL880_CARD_ZERO:
			tl880_write_register(tl880dev, 0x28000, other);
			break;
	}
}

static void tl880_init_sys_pll_eq(struct tl880_dev *tl880dev, unsigned short reference)
{
	tl880_write_register(tl880dev, 0x5000, ((reference & 0x3ff) << 0x11) - 0x1e6e0);
}

static void tl880_init_mif_pll_eq(struct tl880_dev *tl880dev, unsigned short reference)
{
	tl880_write_register(tl880dev, 0x5400, ((reference & 0x3ff) << 0x11) - 0x1e6e0);
}

static void tl880_init_dll(struct tl880_dev *tl880dev)
{
	tl880_write_register(tl880dev, 0x5d10, 0x0);
	tl880_write_register(tl880dev, 0x5d14, 0xc);
	tl880_write_register(tl880dev, 0x5d0c, 0xe);
	tl880_write_register(tl880dev, 0x5d18, 0x14);
	tl880_write_register(tl880dev, 0x5c00, 0x3);
	tl880_write_register(tl880dev, 0x5c04, 0x303);
	tl880_write_register(tl880dev, 0x5c08, 0x2c);
	tl880_write_register(tl880dev, 0x5c0c, 0x303);
	tl880_write_register(tl880dev, 0x5c10, 0x90);
	tl880_write_register(tl880dev, 0x5c14, 0x306);
	tl880_write_register(tl880dev, 0x5c18, 0x10f);
	tl880_write_register(tl880dev, 0x5c1c, 0x20d);
	tl880_write_register(tl880dev, 0x5c20, 0x301);
	tl880_write_register(tl880dev, 0x5c24, 0x284);
	tl880_write_register(tl880dev, 0x5c28, 0x301);
	tl880_write_register(tl880dev, 0x5c2c, 0x2c);
	tl880_write_register(tl880dev, 0x5c30, 0x303);
	tl880_write_register(tl880dev, 0x5c34, 0x81);
	tl880_write_register(tl880dev, 0x5c38, 0x306);
	tl880_write_register(tl880dev, 0x5c3c, 0x18c);
	tl880_write_register(tl880dev, 0x5c40, 0x214);
	tl880_write_register(tl880dev, 0x5c44, 0x301);
	tl880_write_register(tl880dev, 0x5c48, 0x28d);
	tl880_write_register(tl880dev, 0x5c4c, 0x301);
	tl880_write_register(tl880dev, 0x5c50, 0x100);
	tl880_write_register(tl880dev, 0x5c54, 0x21e);
	tl880_write_register(tl880dev, 0x5c58, 0x301);
	tl880_write_register(tl880dev, 0x5c5c, 0x10f);
	tl880_write_register(tl880dev, 0x5c60, 0x20b);
	tl880_write_register(tl880dev, 0x5c64, 0x301);
	tl880_write_register(tl880dev, 0x5c68, 0x294);
	tl880_write_register(tl880dev, 0x5c6c, 0x301);
	tl880_write_register(tl880dev, 0x5c70, 0x10);
	tl880_write_register(tl880dev, 0x5c74, 0x303);
	tl880_write_register(tl880dev, 0x5c78, 0xff);
	tl880_write_register(tl880dev, 0x5c7c, 0x306);
	tl880_write_register(tl880dev, 0x5c80, 0x294);
	tl880_write_register(tl880dev, 0x5c84, 0x301);
	tl880_write_register(tl880dev, 0x5d10, 0x1);
}

static void tl880_default_dpc_reg(struct tl880_dev *tl880dev)
{
	tl880_write_register(tl880dev, 0x10000, 0x80);
	tl880_write_register(tl880dev, 0x10008, 0);
	tl880_write_register(tl880dev, 0x10010, 0);
	tl880_write_register(tl880dev, 0x10014, 0);
	tl880_write_register(tl880dev, 0x10018, 0);
	tl880_write_register(tl880dev, 0x1001c, 0);
	tl880_write_register(tl880dev, 0x10020, 0);
	tl880_write_register(tl880dev, 0x10024, 0);
	tl880_write_register(tl880dev, 0x10028, 0);
	tl880_write_register(tl880dev, 0x10034, 0x184014);
	tl880_write_register(tl880dev, 0x10040, 0x1200);
	tl880_write_register(tl880dev, 0x10044, 0x108080);
	tl880_write_register(tl880dev, 0x10048, 0);
	tl880_write_register(tl880dev, 0x1004c, 0);
	tl880_write_register(tl880dev, 0x10050, 0);
	tl880_write_register(tl880dev, 0x10054, 0);
	tl880_write_register(tl880dev, 0x10058, 0);
	tl880_write_register(tl880dev, 0x1005c, 0);
	tl880_write_register(tl880dev, 0x10060, 0x80);
	tl880_write_register(tl880dev, 0x10064, 0x80);
	tl880_write_register(tl880dev, 0x10068, 0x80);
	tl880_write_register(tl880dev, 0x1006c, 0x80);
	tl880_write_register(tl880dev, 0x10070, 0);
	tl880_write_register(tl880dev, 0x10034, 0x184014);
	tl880_write_register(tl880dev, 0x10080, 0);
	tl880_write_register(tl880dev, 0x10084, 0);
	tl880_write_register(tl880dev, 0x10088, 0);
	tl880_write_register(tl880dev, 0x1008c, 0);
	tl880_write_register(tl880dev, 0x10090, 0);
	tl880_write_register(tl880dev, 0x10094, 0);
	tl880_write_register(tl880dev, 0x10098, 0);
	tl880_write_register(tl880dev, 0x1009c, 0);
	tl880_write_register(tl880dev, 0x100a0, 0);
	tl880_write_register(tl880dev, 0x100a4, 0);
	tl880_write_register(tl880dev, 0x100a8, 0);
	tl880_write_register(tl880dev, 0x100c0, 0);
	tl880_write_register(tl880dev, 0x100c4, 0);
	tl880_write_register(tl880dev, 0x100c8, 0);
	tl880_write_register(tl880dev, 0x100cc, 0);
	tl880_write_register(tl880dev, 0x100d0, 0);
	tl880_write_register(tl880dev, 0x100d4, 0);
	tl880_write_register(tl880dev, 0x100d8, 0);
	tl880_write_register(tl880dev, 0x10100, 0);
	tl880_write_register(tl880dev, 0x10104, 0);
	tl880_write_register(tl880dev, 0x10108, 0);
	tl880_write_register(tl880dev, 0x10140, 0);
	tl880_write_register(tl880dev, 0x10144, 0);
	tl880_write_register(tl880dev, 0x10148, 0);
	tl880_write_register(tl880dev, 0x1014c, 0);
	tl880_write_register(tl880dev, 0x10150, 0);
	tl880_write_register(tl880dev, 0x10154, 0);
	tl880_write_register(tl880dev, 0x10158, 0);
	tl880_write_register(tl880dev, 0x1015c, 0);
	tl880_write_register(tl880dev, 0x10160, 0);
}


void tl880_init_chip(struct tl880_dev *tl880dev)
{
	/* Clear interrupts */
	tl880_read_register(tl880dev, 0);
	tl880_write_register(tl880dev, 4, 0);

	tl880_init_sdram(tl880dev);
	tl880_init_sys_pll_eq(tl880dev, 0x64);
	tl880_init_mif_pll_eq(tl880dev, 0x64);
	tl880_init_dll(tl880dev);
	tl880_disable_interrupts(tl880dev);
	tl880_default_dpc_reg(tl880dev);

	tl880_init_dpc_pll(tl880dev);
}


/*
 * Init for MIT MyHD
 */
void tl880_init_myhd(struct tl880_dev *tl880dev)
{
	int result = 0;

	if(tl880dev->card_type >= TL880_CARD_MYHD_MDP110 && tl880dev->card_type <= TL880_CARD_MYHD_MDP130) {
		tl880_write_regbits(tl880dev, 0x10194, 0x17, 0, 0xeff00);
	} else if(tl880dev->card_type == TL880_CARD_MYHD_MDP100) {
		tl880_write_regbits(tl880dev, 0x10194, 0xf, 0x8, 0xff);
	}
	tl880_write_regbits(tl880dev, 0x10190, 0x17, 0, 0xffffff);
	tl880_write_regbits(tl880dev, 0x10198, 0xf, 8, 0x10);
	tl880_write_regbits(tl880dev, 0x10190, 0x1b, 0x1a, 3);
	tl880_write_regbits(tl880dev, 0x10194, 0x1b, 0x1a, 0);
	tl880_write_regbits(tl880dev, 0x10198, 0x1b, 0x1a, 0);
	if(tl880dev->card_type >= TL880_CARD_MYHD_MDP110 && tl880dev->card_type <= TL880_CARD_MYHD_MDP130) {
		tl880_write_regbits(tl880dev, 0x10198, 0x17, 0x16, 0);
		tl880_write_regbits(tl880dev, 0x10198, 0x10, 0x10, 0);
		tl880_write_regbits(tl880dev, 0x10198, 0x13, 0x11, 3);
	}
	tl880_set_gpio(tl880dev, 0, 1);
	tl880_set_gpio(tl880dev, 6, 1);
	if(tl880dev->card_type >= TL880_CARD_MYHD_MDP110 && tl880dev->card_type <= TL880_CARD_MYHD_MDP130) {
		tl880_write_regbits(tl880dev, 0x10198, 0x13, 0x13, 1);
	}

	// i2cInit (signals i2c semaphore)
	
	tl880_set_vip(tl880dev, 0);
	tl880_set_vip(tl880dev, 1);

	/* Set the VPX322x power mode:
	 *   register 0xAA
	 *   	bit[1:0]:	Low power mode
	 *   		  00	Active mode, outputs enabled
	 *   		  01	Outputs tri-stated; clock divided by 2; I2C full speed
	 *   		 *10	Outputs tri-stated; clock divided by 4; I2C full speed (< 100kbits/s for 3226F)
	 *   		  11	Outputs tri-stated; clock divided by 8; I2C < 100kbits/s
	 */
	// I2CWrite8(0x86, 0xaa, 2);
	/* Uncomment when i2c_write8 is implemented */
	/*
	i2c_write8(&tl880dev->i2cbuses[0], 0x43, 0xaa, 2);
	*/
	tl880_i2c_write_byte_data(&tl880dev->i2cbuses[0], 0x43, 0xaa, 2);
	tl880_i2c_write_byte_data(&tl880dev->i2cbuses[0], 0x43, 0xaa, 0);
	
	// ConfigVPX();
	// ConfigMSP();
	if((result = request_module("vpx322x"))) {
		printk(KERN_WARNING "tl880: error requesting vpx322x module: %i\n", result);
	}

	if((result = request_module("msp3400"))) {
		printk(KERN_WARNING "tl880: error requesting msp3400 module: %i\n", result);
	}

	if((result = request_module("tuner"))) {
		printk(KERN_WARNING "tl880: error requesting tuner module: %i\n", result);
	}
	
	if((result = request_module("nxt200x"))) {
		printk(KERN_WARNING "tl880: error requesting nxt200x module: %i\n", result);
	}
#if 0
	/* Try to attach to the nxt200x */
	dvb_attach(nxt200x_attach, &nxt200x_demod_config, &tl880dev->i2cbuses[1].i2c_adap);
#endif
	
	// if(*((char*)cJanus+0x16748) == 1) {
	// 	SetNtscAudioClock();
	// } else {
	// 	/* Set the VPX322x Output Multiplexer mode:
	// 	 *   FP register 0x154	Output Multiplexer
	// 	 * 	bit[7:0]:	Multi-purpose bits on port B
	// 	 * 			determines the state of Port B when used as 
	// 	 * 			programmable output
	// 	 * 		  					(0x50)
	// 	 * 	bit[8]:		Activate multi-purpose bits on port B
	// 	 * 			note that double clock mode has to be selected
	// 	 * 			for this option!
	// 	 * 							(on)
	// 	 * 	bit[9]:		Port Mode
	// 	 * 		  0	parallel_out, 'single clock', Port A & B = FO[15:0];
	// 	 * 		  1	'double clock'
	// 	 * 		  	Port A = FO[15:8] / FO[7:0],
	// 	 * 		  	Port B = programmable output/not used
	// 	 * 		  					(on)
	// 	 * 	bit[10]:	switch 'VBI active' qualifier
	// 	 * 		  0	connect 'VBI active' to VACT pin
	// 	 * 		  1	connect 'VBI active to TDO pin
	// 	 * 		  					(off)
	// 	 * 	bit[11]:	reserved (must be set to zero)
	// 	 * 							(off)
	// 	 */
	// 	_VPXWriteFP(0x154, 0x350);
	// }
	tl880_vpx_write_fp(&tl880dev->i2cbuses[0], 0x43, 0x154, 0x350);
	
	if(tl880dev->card_type == TL880_CARD_MYHD_MDP100A) {
		tl880_set_gpio(tl880dev, 5, 0);
	} else if(tl880dev->card_type >= TL880_CARD_MYHD_MDP100) {
		tl880_set_gpio(tl880dev, 5, 0);
		// _VPXWriteFP(0x154, 0x301);
		tl880_vpx_write_fp(&tl880dev->i2cbuses[0], 0x43, 0x154, 0x301);
	}

	// cVsbDemod::SpawnVsbTask();
}

/*
 * I2C/GPIO init for Hauppauge WinTV-HD
 */
void tl880_init_wintv_hd_i2c(struct tl880_dev *tl880dev)
{
	/* Calibrate CPU timer? */
	/* HCWCalibrate(0xafc80); */
	tl880_write_regbits(tl880dev, 0x10190, 0x1b, 0x1b, 1);
	tl880_write_regbits(tl880dev, 0x10190, 0x1a, 0x1a, 1);
	tl880_write_regbits(tl880dev, 0x10190, 7, 0, 0x1f);
	tl880_write_regbits(tl880dev, 0x10190, 0, 0, 1);
	tl880_write_regbits(tl880dev, 0x10190, 2, 2, 1);
	tl880_write_regbits(tl880dev, 0x10190, 1, 1, 1);
	tl880_write_regbits(tl880dev, 0x10190, 3, 3, 1);
	tl880_write_regbits(tl880dev, 0x10194, 3, 3, 1);
	tl880_write_regbits(tl880dev, 0x10194, 2, 2, 0);
	tl880_write_regbits(tl880dev, 0x10194, 1, 1, 1);
	tl880_write_regbits(tl880dev, 0x10194, 0, 0, 0);
	tl880_write_regbits(tl880dev, 0x10194, 4, 4, 1);
	tl880_write_regbits(tl880dev, 0x10198, 4, 4, 1);
}

/*
 * Init for Hauppauge WinTV-HD
 */
void tl880_init_wintv_hd(struct tl880_dev *tl880dev)
{
	tl880_write_regbits(tl880dev, 0x10198, 0xf, 8, 9);
	tl880_write_regbits(tl880dev, 0x10194, 0xf, 8, 0xff);
	tl880_write_regbits(tl880dev, 0x10190, 0xf, 8, 0xff);
	tl880_write_regbits(tl880dev, 0x10198, 0xf, 8, 9);
	tl880_write_regbits(tl880dev, 0x10190, 0x1b, 0x1a, 3);
	tl880_write_regbits(tl880dev, 0x10198, 4, 4, 1);
	udelay(1);
	tl880_write_regbits(tl880dev, 0x10194, 0xf, 0xf, 0);
	
	// tl880_init_wintv_hd_i2c(tl880dev);
	
	tl880_set_vip(tl880dev, 1);
	
	// i2c_write8(tl880dev, 0x86, 0xaa, 2);
	// i2c_write8(tl880dev, 0x86, 0xaa, 0);
	/* ConfigVPX() */
	// if(*((char*)cJanus+0x16748) == 1) {
	// 	SetNtscAudioClock()
	// } else {
	// 	/* Set the VPX322x Output Multiplexer mode:
	// 	 *   FP register 0x154	Output Multiplexer
	// 	 * 	bit[7:0]:	Multi-purpose bits on port B
	// 	 * 			determines the state of Port B when used as 
	// 	 * 			programmable output
	// 	 * 		  					(0x50)
	// 	 * 	bit[8]:		Activate multi-purpose bits on port B
	// 	 * 			note that double clock mode has to be selected
	// 	 * 			for this option!
	// 	 * 							(on)
	// 	 * 	bit[9]:		Port Mode
	// 	 * 		  0	parallel_out, 'single clock', Port A & B = FO[15:0];
	// 	 * 		  1	'double clock'
	// 	 * 		  	Port A = FO[15:8] / FO[7:0],
	// 	 * 		  	Port B = programmable output/not used
	// 	 * 		  					(on)
	// 	 * 	bit[10]:	switch 'VBI active' qualifier
	// 	 * 		  0	connect 'VBI active' to VACT pin
	// 	 * 		  1	connect 'VBI active to TDO pin
	// 	 * 		  					(off)
	// 	 * 	bit[11]:	reserved (must be set to zero)
	// 	 * 							(off)
	// 	 */
	// 	_VPXWriteFP(0x154, 0x350);
	// }
	
	//tl880_set_gpio(tl880dev, 2);
}

/*
 * I2C/GPIO Init for Telemann HiPix (never called?)
 */
void tl880_init_hipix_i2c(struct tl880_dev *tl880dev)
{
	/* Why is it writing to register 0x10000? */
	tl880_write_regbits(tl880dev, 0x10000, 0xc, 0xc, 1);
	tl880_write_regbits(tl880dev, 0x10000, 0xb, 0xb, 0);
	tl880_write_regbits(tl880dev, 0x10190, 0x1a, 0x1a, 1);
	tl880_write_regbits(tl880dev, 0x10190, 0x1b, 0x1b, 1);
}

/*
 * Init for Telemann HiPix
 */
void tl880_init_hipix(struct tl880_dev *tl880dev)
{
	// char writeval = 0xa;
	
	tl880_write_regbits(tl880dev, 0x10198, 0xf, 8, 0xc);
	tl880_write_regbits(tl880dev, 0x10194, 0xf, 8, 0x5f);
	tl880_write_regbits(tl880dev, 0x10190, 0xf, 8, 0xff);
	tl880_write_regbits(tl880dev, 0x10198, 0xf, 8, 0xc);
	tl880_write_regbits(tl880dev, 0x10190, 0x1b, 0x1a, 3);
	tl880_set_gpio(tl880dev, 0, 1);

	/* i2cInit(1) */

	/* tl880_init_hipix_i2c(); */

	/*
	if(gpJanus[0x6faaf0] == 1) {
		tl880_set_vip(tl880dev, 2);
	} else {
		tl880_set_vip(tl880dev, 1);
	}
	*/
	tl880_set_vip(tl880dev, 2);
	
	// i2c_write8(0x86, 0xaa, 2);
	// i2c_write8(0x86, 0xaa, 0);
	// i2c_write8(0x54, 0, 0x10);
	// i2c_write8(0x54, 1, 0x38);
	// i2c_write8(0x54, 7, 0);
	
	/* Loads a bunch of VPX settings */
	// LoadDefaultSettings();
	
	// _VPXWriteFP(0x154, 0x350);
	// i2c_write(0x86, 1, 0xf2, &writeval, 1);
}

void tl880_init_dev(struct tl880_dev *tl880dev)
{
	tl880_init_chip(tl880dev);

	/* Set up I2C bus(es) */
	tl880_init_i2c(tl880dev);

	/* TODO: Change this to switch on TL880_CARD_* */
	switch(tl880dev->subsys_vendor_id) {
		case PCI_SUBSYSTEM_VENDOR_ID_MIT:
			switch(tl880dev->subsys_device_id) {
				case PCI_SUBSYSTEM_DEVICE_ID_MYHD:
				case PCI_SUBSYSTEM_DEVICE_ID_MYHD_MDP120:
				case PCI_SUBSYSTEM_DEVICE_ID_MYHD_MDP130:
					tl880_init_myhd(tl880dev);
					break;
				default:
					break;
			}
			break;
		case PCI_SUBSYSTEM_VENDOR_ID_HAUPPAUGE:
			switch(tl880dev->subsys_device_id) {
				case PCI_SUBSYSTEM_DEVICE_ID_WINTV_HD:
					tl880_init_wintv_hd(tl880dev);
					break;
				default:
					break;
			}
			break;
		case PCI_SUBSYSTEM_VENDOR_ID_TELEMANN:
		case PCI_SUBSYSTEM_VENDOR_ID_ZERO:
			switch(tl880dev->subsys_device_id) {
				case PCI_SUBSYSTEM_DEVICE_ID_HIPIX:
					tl880_init_hipix(tl880dev);
					break;
				default:
					tl880_init_hipix(tl880dev);
					break;
			}
			break;
		default:
			break;
	}

	/* Set NTSC input */
	tl880_init_ntsc_audio(tl880dev);
	tl880_set_ntsc_input(tl880dev, 0);

	/* Initialize DPC2? (whatever that is) */
	tl880_write_register(tl880dev, 0x10180, 0x200);
	tl880_write_register(tl880dev, 0x10184, 0xf7fafcff);
	tl880_write_register(tl880dev, 0x10184, 0xfcf6f4f4);
	tl880_write_register(tl880dev, 0x10184, 0x402a1807);
	tl880_write_register(tl880dev, 0x10184, 0x7e766855);
	tl880_write_register(tl880dev, 0x10184, 0xf7fafcff);
	tl880_write_register(tl880dev, 0x10184, 0xfcf6f4f4);
	tl880_write_register(tl880dev, 0x10184, 0x402a1807);
	tl880_write_register(tl880dev, 0x10184, 0x7e766855);

	tl880_write_register(tl880dev, 0x10180, 0x210);
	tl880_write_register(tl880dev, 0x10184, 0x3c241204);
	tl880_write_register(tl880dev, 0x10184, 0xfbf6f5f1);
	tl880_write_register(tl880dev, 0x10184, 0x53677881);
	tl880_write_register(tl880dev, 0x10184, 0x81786753);
	tl880_write_register(tl880dev, 0x10184, 0x3c241204);
	tl880_write_register(tl880dev, 0x10184, 0xfbf6f5f1);
	tl880_write_register(tl880dev, 0x10184, 0x53677881);
	tl880_write_register(tl880dev, 0x10184, 0x81786753);

	/* Color conversion matrix (values are from a register dump in Windows) */
	tl880_write_register(tl880dev, 0x10180, 0x300);
	tl880_write_register(tl880dev, 0x10184, 0x012A07CB);
	tl880_write_register(tl880dev, 0x10184, 0x07791000);
	tl880_write_register(tl880dev, 0x10184, 0x012A021C);
	tl880_write_register(tl880dev, 0x10184, 0x00008000);
	tl880_write_register(tl880dev, 0x10184, 0x012A0000);
	tl880_write_register(tl880dev, 0x10184, 0x01CB8000);

	printk(KERN_DEBUG "tl880: GPIO at end of init: 0x10190=0x%08lx 0x10194=0x%08lx 0x10198=0x%08lx 0x1019c=0x%08lx\n",
		tl880_read_register(tl880dev, 0x10190), tl880_read_register(tl880dev, 0x10194),
		tl880_read_register(tl880dev, 0x10198), tl880_read_register(tl880dev, 0x1019c));
}


