/*
 *  TL880/MyHD Sound Driver
 *  Copyright (c)2007 by Mike Bourgeous <nitrogen at users.sourceforge.net>
 *
 *  Based on ALSA dummy driver
 *  Copyright (c) by Jaroslav Kysela <perex@suse.cz>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#include <sound/driver.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/jiffies.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/wait.h>
#include <linux/moduleparam.h>
#include <sound/core.h>
#include <sound/control.h>
#include <sound/tlv.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/rawmidi.h>
#include <sound/initval.h>

#define TL_MODNAME "tl880alsa"
#include "tl880.h"

MODULE_AUTHOR("Mike Bourgeous <nitrogen at users.sourceforge.net");
MODULE_DESCRIPTION("TL880 audio driver");
MODULE_LICENSE("GPL");
MODULE_SUPPORTED_DEVICE("{{ALSA,TL880}}");

#define MAX_PCM_DEVICES		1
#define MAX_PCM_SUBSTREAMS	1
#define MAX_MIDI_DEVICES	0

#define MAX_BUFFER_SIZE		0x80000
#define MAX_PERIOD_SIZE		0x40000

/* TL880 constraints */
#define USE_FORMATS		SNDRV_PCM_FMTBIT_S16_LE
#define USE_CHANNELS_MIN	2
#define USE_CHANNELS_MAX	2
#define USE_PERIODS_MIN		2
#define USE_PERIODS_MAX		2
#define USE_RATE		SNDRV_PCM_RATE_22050 | \
				SNDRV_PCM_RATE_32000 | \
				SNDRV_PCM_RATE_44100 | \
				SNDRV_PCM_RATE_48000 | \
				SNDRV_PCM_RATE_64000 | \
				SNDRV_PCM_RATE_88200 | \
				SNDRV_PCM_RATE_96000
#define USE_RATE_MIN		22050
#define USE_RATE_MAX		96000


#ifndef add_playback_constraints
#define add_playback_constraints(x) 0
#endif
#ifndef add_capture_constraints
#define add_capture_constraints(x) 0
#endif

static int index[SNDRV_CARDS] = SNDRV_DEFAULT_IDX;	/* Index 0-MAX */
static char *id[SNDRV_CARDS] = SNDRV_DEFAULT_STR;	/* ID string for this card */
static int enable[SNDRV_CARDS] = {1, [1 ... (SNDRV_CARDS - 1)] = 1};

module_param_array(index, int, NULL, 0444);
MODULE_PARM_DESC(index, "Index value for TL880 sound device.");
module_param_array(id, charp, NULL, 0444);
MODULE_PARM_DESC(id, "ID string for each TL880 sound device.");
module_param_array(enable, bool, NULL, 0444);
MODULE_PARM_DESC(enable, "Enable/disable each TL880 sound device.");

struct snd_tl880_card {
	struct snd_card *card;		/* ALSA sound card */
	struct snd_pcm *pcm;		/* PCM stream */

	struct tl880_dev *tl880dev;	/* Parent TL880 card */

	struct snd_tl880_card *next;	/* Next card in the list */
};

struct snd_tl880_pcm {
	struct snd_tl880_card *tl880;	/* Parent sound card */

	spinlock_t lock;		/* Lock for this PCM */

	struct timer_list timer;	/* Polling timer used until interrupts are implemented XXX */

	unsigned int period_size;	/* Size of one period */
	unsigned int period_count;	/* Number of periods */

	unsigned int buffer_pos;	/* Current period's offset in the buffer */

	u32 buffer_base;		/* Base address of buffer in TL880 RAM */

	struct snd_pcm_substream *substream; /* PCM substream */
};


static struct snd_tl880_card *cards[SNDRV_CARDS] = {[0 ... (SNDRV_CARDS - 1)] = NULL};

static inline void snd_card_tl880_pcm_timer_start(struct snd_tl880_pcm *local_pcm)
{
	local_pcm->timer.expires = 1 + jiffies;
	add_timer(&local_pcm->timer);
}

static inline void snd_card_tl880_pcm_timer_stop(struct snd_tl880_pcm *local_pcm)
{
	del_timer(&local_pcm->timer);
}

static int snd_card_tl880_pcm_trigger(struct snd_pcm_substream *substream, int cmd)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct snd_tl880_pcm *local_pcm = runtime->private_data;
	struct tl880_dev *tl880dev = local_pcm->tl880->tl880dev;
	int err = 0;

	spin_lock(&local_pcm->lock);
	switch (cmd) {
		case SNDRV_PCM_TRIGGER_START:
			tl880_clear_sdram(tl880dev, local_pcm->buffer_base, local_pcm->buffer_base + local_pcm->period_size * local_pcm->period_count, 0);
			tl880_write_register(tl880dev, 0x302c, local_pcm->buffer_base);
		case SNDRV_PCM_TRIGGER_RESUME:
			snd_card_tl880_pcm_timer_start(local_pcm);
			tl880_write_regbits(tl880dev, 0x3004, 1, 1, 0);
			break;
		case SNDRV_PCM_TRIGGER_STOP:
		case SNDRV_PCM_TRIGGER_SUSPEND:
			tl880_write_regbits(tl880dev, 0x3004, 1, 1, 1);
			snd_card_tl880_pcm_timer_stop(local_pcm);
			break;

		case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
			tl880_write_regbits(tl880dev, 0x3004, 1, 1, 0);
			break;
		case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
			tl880_write_regbits(tl880dev, 0x3004, 1, 1, 0);
			break;
		default:
			err = -EINVAL;
			break;
	}
	spin_unlock(&local_pcm->lock);

	return 0;
}

static int snd_card_tl880_pcm_prepare(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct snd_tl880_pcm *local_pcm = runtime->private_data;
	struct tl880_dev *tl880dev = local_pcm->tl880->tl880dev;

	local_pcm->period_size = snd_pcm_lib_buffer_bytes(substream);
	local_pcm->period_count = snd_pcm_lib_period_bytes(substream);
	local_pcm->buffer_pos = 0;

	local_pcm->buffer_base = 0x1000;

	tl880_clear_sdram(tl880dev, 0x1000, 0x1000 + local_pcm->period_size * local_pcm->period_count, 0);
	tl880_write_register(tl880dev, 0x3020, 0x1000); 						// Buffer start
	tl880_write_register(tl880dev, 0x3024, 0x1000 + local_pcm->period_size * local_pcm->period_count - 0x10);	// Buffer end - 0x10
	tl880_write_register(tl880dev, 0x3028, 0); 							// Capture offset
	tl880_write_register(tl880dev, 0x302c, 0x1000);							// Buffer position
	tl880_write_register(tl880dev, 0x3030, 0x1000 + local_pcm->period_size);			// Interrupt position

	return 0;
}

static void snd_card_tl880_pcm_timer_function(unsigned long data)
{
	struct snd_tl880_pcm *local_pcm = (struct snd_tl880_pcm *)data;
	struct tl880_dev *tl880dev = local_pcm->tl880->tl880dev;
	unsigned long flags;

	u32 reg3020, reg302c, reg3030;
	
	spin_lock_irqsave(&local_pcm->lock, flags);

	local_pcm->timer.expires = 1 + jiffies;
	add_timer(&local_pcm->timer);

	reg3020 = tl880_read_register(tl880dev, 0x3020); // Buffer start
	reg302c = tl880_read_register(tl880dev, 0x302c); // Buffer playback position
	reg3030 = tl880_read_register(tl880dev, 0x3030); // Interrupt position

	/* Check for elapsed PCM period */
	if(reg302c > reg3030 || reg302c < local_pcm->buffer_pos) {

		/* Update the interrupt trigger position */
		reg3030 += local_pcm->period_size;
		if(reg3030 > (reg3020 + local_pcm->period_size * local_pcm->period_count)) {
			reg3030 = reg3020 + local_pcm->period_size;
		}

		/* Update the current playback buffer position (which is one period behind the buffer to be written) */
		local_pcm->buffer_pos += local_pcm->period_size;
		if(local_pcm->buffer_pos >= (reg3020 + local_pcm->period_size * (local_pcm->period_count - 1))) {
			local_pcm->buffer_pos = reg3020;
		}

		spin_unlock_irqrestore(&local_pcm->lock, flags);
		snd_pcm_period_elapsed(local_pcm->substream);
	} else {
		spin_unlock_irqrestore(&local_pcm->lock, flags);
	}
}

static snd_pcm_uframes_t snd_card_tl880_pcm_pointer(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct snd_tl880_pcm *local_pcm = runtime->private_data;

	u32 reg302c = tl880_read_register(local_pcm->tl880->tl880dev, 0x302c);

	return bytes_to_frames(runtime, (reg302c - local_pcm->buffer_base) % local_pcm->period_size);
}

static struct snd_pcm_hardware snd_card_tl880_playback =
{
	.info =			(SNDRV_PCM_INFO_INTERLEAVED |
				 SNDRV_PCM_INFO_RESUME |
				 SNDRV_PCM_INFO_HALF_DUPLEX), // TODO: mmap using SNDRV_PCM_INFO_MMAP_IOMEM?
	.formats =		USE_FORMATS,
	.rates =		USE_RATE,
	.rate_min =		USE_RATE_MIN,
	.rate_max =		USE_RATE_MAX,
	.channels_min =		USE_CHANNELS_MIN,
	.channels_max =		USE_CHANNELS_MAX,
	.buffer_bytes_max =	MAX_BUFFER_SIZE,
	.period_bytes_min =	64,
	.period_bytes_max =	MAX_PERIOD_SIZE,
	.periods_min =		USE_PERIODS_MIN,
	.periods_max =		USE_PERIODS_MAX,
	.fifo_size =		0,
};

static void snd_card_tl880_runtime_free(struct snd_pcm_runtime *runtime)
{
	kfree(runtime->private_data);
}

/* Allocate a hardware buffer (from previously preallocated memory), set HW parameters */
static int snd_card_tl880_hw_params(struct snd_pcm_substream *substream,
				    struct snd_pcm_hw_params *hw_params)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct snd_tl880_pcm *local_pcm = runtime->private_data;
	struct tl880_dev *tl880dev = local_pcm->tl880->tl880dev;

	runtime->dma_area = tl880dev->memspace + local_pcm->buffer_base;
	runtime->dma_addr = tl880dev->memphys + local_pcm->buffer_base;
	runtime->dma_bytes = MAX_BUFFER_SIZE;

	local_pcm->period_size = params_period_bytes(hw_params);
	local_pcm->period_count = params_periods(hw_params);
	local_pcm->buffer_pos = 0;

	tl880_write_register(tl880dev, 0x3020, local_pcm->buffer_base); 						// Buffer start
	tl880_write_register(tl880dev, 0x3024, local_pcm->buffer_base + local_pcm->period_size * local_pcm->period_count - 0x10);	// Buffer end - 0x10
	tl880_write_register(tl880dev, 0x3028, 0); 							// Capture offset
	tl880_write_register(tl880dev, 0x302c, local_pcm->buffer_base);							// Buffer position
	tl880_write_register(tl880dev, 0x3030, local_pcm->buffer_base + local_pcm->period_size);			// Interrupt position

	return 0;
}

static int snd_card_tl880_hw_free(struct snd_pcm_substream *substream)
{
	return 0;
}

static struct snd_tl880_pcm *snd_tl880_new_stream(struct snd_pcm_substream *substream)
{
	struct snd_tl880_pcm *local_pcm;

	if(CHECK_NULL(substream) || TL_ASSERT((local_pcm = kzalloc(sizeof(*local_pcm), GFP_KERNEL)) != NULL)) {
		return NULL;
	}

	init_timer(&local_pcm->timer);
	local_pcm->timer.data = (unsigned long) local_pcm;
	local_pcm->timer.function = snd_card_tl880_pcm_timer_function;
	spin_lock_init(&local_pcm->lock);
	local_pcm->substream = substream;
	return local_pcm;
}

static int snd_card_tl880_playback_open(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct snd_tl880_pcm *local_pcm;
	int err;

	if(CHECK_NULL(substream)) {
		return -EINVAL;
	}

	if(TL_ASSERT((local_pcm = snd_tl880_new_stream(substream)) != NULL)) {
		return -ENOMEM;
	}

	runtime->private_data = local_pcm;
	runtime->private_free = snd_card_tl880_runtime_free;
	runtime->hw = snd_card_tl880_playback;

	if(TL_ASSERT((err = add_playback_constraints(runtime)) >= 0)) {
		kfree(local_pcm);
		return err;
	}

	return 0;
}

static int snd_card_tl880_playback_close(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct snd_tl880_pcm *local_pcm = runtime->private_data;

	tl880_clear_sdram(local_pcm->tl880->tl880dev, local_pcm->buffer_base, local_pcm->buffer_base + local_pcm->period_size * local_pcm->period_count, 0);

	kfree(local_pcm);

	return 0;
}

static int snd_card_tl880_playback_copy(struct snd_pcm_substream *substream, int channel,
		               snd_pcm_uframes_t pos, void *src, snd_pcm_uframes_t count)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct snd_tl880_pcm *local_pcm = runtime->private_data;

	if(TL_ASSERT(channel >= 0)) {
		return -EINVAL;
	}

	tl880_memcpy(
			local_pcm->tl880->tl880dev, 
			src, 
			local_pcm->buffer_base + frames_to_bytes(runtime, pos), 
			frames_to_bytes(runtime, count)
			);

	return 0;
}

static int snd_card_tl880_playback_silence(struct snd_pcm_substream *substream, int channel,
		snd_pcm_uframes_t pos, snd_pcm_uframes_t count)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct snd_tl880_pcm *local_pcm = runtime->private_data;
	
	if(TL_ASSERT(channel >= 0)) {
		return -EINVAL;
	}

	tl880_clear_sdram(
			local_pcm->tl880->tl880dev, 
			local_pcm->buffer_base + frames_to_bytes(runtime, pos), 
			local_pcm->buffer_base + frames_to_bytes(runtime, pos) + frames_to_bytes(runtime, count),
			0
			);

	return 0;
}

static struct snd_pcm_ops snd_card_tl880_playback_ops = {
	.open =			snd_card_tl880_playback_open,
	.close =		snd_card_tl880_playback_close,
	.ioctl =		snd_pcm_lib_ioctl,
	.hw_params =		snd_card_tl880_hw_params,
	.hw_free =		snd_card_tl880_hw_free,
	.prepare =		snd_card_tl880_pcm_prepare,
	.trigger =		snd_card_tl880_pcm_trigger,
	.pointer =		snd_card_tl880_pcm_pointer,
	.copy = 		snd_card_tl880_playback_copy,
	.silence = 		snd_card_tl880_playback_silence,
};

static int __devinit snd_card_tl880_pcm(struct snd_tl880_card *tl880, int device)
{
	struct snd_pcm *pcm;
	int err;

	if(TL_ASSERT((err = snd_pcm_new(tl880->card, "TL880 PCM", device, 1, 0, &pcm)) == 0)) {
		return err;
	}

	tl880->pcm = pcm;

	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_PLAYBACK, &snd_card_tl880_playback_ops);

	pcm->private_data = tl880;
	pcm->info_flags = 0;
	snprintf(pcm->name, 80, "tl880 %d PCM", device);

	/* TODO: preallocate TL880 card memory here */

	return 0;
}


static int n_cards = 0;

static int __devinit snd_tl880_add_card(int dev)
{
	struct snd_card *card;
	struct snd_tl880_card *tl880;
	int err;

	if(!enable[dev]) {
		printk(KERN_DEBUG "tl880alsa: card %d audio is not enabled\n", dev + 1);
		return -ENOENT;
	}

	/* Create a new sound card structure */
	if(TL_ASSERT((card = snd_card_new(index[dev], id[dev], THIS_MODULE, sizeof(struct snd_tl880_card))) != NULL)) {
		return -ENOMEM;
	}

	tl880 = card->private_data;
	tl880->card = card;
	tl880->tl880dev = find_tl880(dev);

	/* Add a PCM to the card */
	if(TL_ASSERT((err = snd_card_tl880_pcm(tl880, dev)) == 0)) {
		snd_card_free(card);
		return err;
	}

	strncpy(card->driver, "tl880", 16);
	strncpy(card->shortname, "tl880", 32);
	snprintf(card->longname, 80, "tl880 %d", dev + 1);

	if(TL_ASSERT((err = snd_card_register(card)) == 0)) {
		snd_card_free(card);
		return err;
	}

	cards[dev] = tl880;

	return 0;
}

static void __devexit snd_tl880_remove_card(int dev)
{
	if(cards[dev] != NULL) {
		snd_card_free(cards[dev]->card); // This frees the snd_card and the snd_card_tl880
		cards[dev] = NULL;
	}
}

static int __init snd_tl880_init(void)
{
	int i, err = 0;

	n_cards = tl880_card_count();
	
	printk(KERN_DEBUG "tl880alsa: Found %d tl880 cards.\n", n_cards);

	/* Go through all TL880s, adding ALSA card definitions for each one */
	for(i = 0; i < n_cards && i < SNDRV_CARDS; i++) {
		if(TL_ASSERT((err = snd_tl880_add_card(i)) == 0)) {
			if(err != -ENOENT) {
				goto _cleanup;
			} else {
				continue;
			}
		}
	}

	return 0;

_cleanup:
	/* Init failed, so clean any cards that went through */
	printk(KERN_DEBUG "tl880alsa: Failed to initialize TL880 alsa driver\n");
	for(i = 0; i < n_cards; i++) {
		snd_tl880_remove_card(i);
	}
	return err;
}

static void __exit snd_tl880_exit(void)
{
	int i;

	for(i = 0; i < n_cards && i < SNDRV_CARDS; i++) {
		snd_tl880_remove_card(i);
	}
}

module_init(snd_tl880_init)
module_exit(snd_tl880_exit)


