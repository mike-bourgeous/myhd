/* 
 * Input selection stuff
 *
 * (c) 2003 Mike Bourgeous <nitrogen@slimetech.com>
 */
#include "tl880.h"

void tl880_set_video_source(struct tl880_dev *tl880dev, unsigned char a, unsigned char b)
{
	__u16 cmdarg = 0;

	if(!tl880dev) {
		printk(KERN_ERR "tl880: NULL tl880dev in tl880_set_video_source\n");
		return;
	}
	
	if(a == 1) {
		if(tl880dev->card_type == 1) {
			// _VPXWriteFP(0x154, 0x331);
			tl880_vpx_write_fp(&(tl880dev->i2cbuses[0]), 0x43, 0x154, 0x331);
		} else if(tl880dev->card_type >= 2) {
			// MspSetInput(9);
			cmdarg = TVAUDIO_INPUT_EXTERN;
			tl880_call_i2c_clients(tl880dev, VIDIOC_S_AUDIO, &cmdarg);
		}
		a = 1;
	} else if(a == 3) {
		if(tl880dev->card_type == 1) {
			// _VPXWriteFP(0x154, 0x311);
			tl880_vpx_write_fp(&(tl880dev->i2cbuses[0]), 0x43, 0x154, 0x311);
		} else 	if(tl880dev->card_type >= 2) {
			// MspSetInput(9);
			cmdarg = TVAUDIO_INPUT_EXTERN;
			tl880_call_i2c_clients(tl880dev, VIDIOC_S_AUDIO, &cmdarg);
		}
		a = 3;
	} else {
		if(tl880dev->card_type == 1) {
			// _VPXWriteFP(0x154, 0x331);
			tl880_vpx_write_fp(&(tl880dev->i2cbuses[0]), 0x43, 0x154, 0x331);
		} else if(tl880dev->card_type >= 2) {
			// MspSetInput(6);
		        cmdarg = TVAUDIO_INPUT_TUNER;
			tl880_call_i2c_clients(tl880dev, VIDIOC_S_AUDIO, &cmdarg);
		}
		a = 2;
	}

	// _VPXSetVideoSource(a, b);
}


void tl880_set_ntsc_input(struct tl880_dev *tl880dev, int input)
{
	switch(input) {
		default:
		case 0:
			tl880_set_video_source(tl880dev, 2, 4);
			break;
		case 1:
			tl880_set_video_source(tl880dev, 3, 4);
			break;
		case 2:
			tl880_set_video_source(tl880dev, 1, 5);
			break;
	}
}


