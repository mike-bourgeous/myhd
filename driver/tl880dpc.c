/* 
 * Video mode stuff for TL880 cards
 *
 * (c) 2003 Mike Bourgeous <nitrogen@slimetech.com>
 */
#include "tl880.h"

void tl880_set_dpc_pll_const(struct tl880_dev *tl880dev, unsigned long a, unsigned char b, unsigned char c)
{
	write_register(tl880dev, 0x5800, (((((a & 0x3ff) << 8) | (b & 0x1f)) << 4) | (c & 3)) << 4);
}

/* XXX: This function is not finished! */
void tl880_init_dpc_pll(struct tl880_dev *tl880dev)
{
	int i;
	
	write_register(tl880dev, 0x10014, 0x80000000);

	/* set_dpc_clock */
	{
		/* write_register(tl880dev, 0x5800, 0x60070); */
		/* set_dpc_pll_const(0x14, 2, 1) */
		{
			int arg0 = 0x14, arg4 = 0x2, arg8 = 0x1;

			arg0 &= 0x3ff;
			arg0 <<= 8;
			arg4 &= 0x1f;
			arg0 += arg4;
			arg0 <<= 4;
			arg8 &= 3;
			arg0 += arg8;
			arg0 <<= 4;
			//write_register(tl880dev, 0x5800, arg0);
			/* This value makes 1024x768p exactly 60Hz 48.3kHz */
			write_register(tl880dev, 0x5800, 0x00160320);
		}
		write_register(tl880dev, 0x5d14, 0x0);
	}

	for(i = 0; i < 10; i++) {
		read_register(tl880dev, 0x10014);
	}

	write_register(tl880dev, 0x10014, 0);
}

#if 0
void tl880_dpc_video_sync_dvd(struct tl880_dev *tl880dev)
{
	static unsigned long val = 1;
	static unsigned long field_select = 0;
	static unsigned long delay_cnt = 0;
	
	edi = gpJanus;
	esi = cJanus->0x10388;
	dpc_hsync_cnt++;
	ebx = 0;
	if((cJanus->0x10388)->0x26c != 0) {
		goto loc_294ea;
	}

	/* val = (cJanus->0x10388)->0x270; */
	val ^= 1;
	
	if(read_regbits(tl880dev, 0x10004, 2, 2)) {
		goto loc_294c6;
	}
	
	if(val == 3 || val == 1) {
		write_regbits(tl880dev, 0x10000, 0, 0, 1);
	} else {
		write_regbits(tl880dev, 0x10000, 0, 0, 0);
	}
	
	var_10 = 1;

	goto loc_29511;

loc_294c6:
	if(val == 3 || val == 1) {
		write_regbits(tl880dev, 0x10000, 0, 0, 0);
	} else {
		write_regbits(tl880dev, 0x10000, 0, 0, 1);
	}

loc_294e1:
	var_10 = 0;

	goto loc_295c9;

loc_294ea:
	ebp = 1;

	field_select ^= 1;

	write_regbits(tl880dev, 0x10000, 0, 0, field_select);

	var_10 = 1;

loc_29511:
	ebp = &(cJanus->0xf4);
	if(cJanus->0xf4 == 0 || cJanus->0xf8 == 0) {
		goto loc_29589;
	}

	/* StartMcu() */

	cJanus->0xf4 = 0;
	if((cJanus->0x10388)->0x27c == 0) {
		goto loc_29551;
	}
	if((cJanus->0x10388)->0x284 == 0) {
		goto loc_29563;
	}

	/* StartVpip(cJanus->0x10388) */
	
	(cJanus->0x10388)->0xb70 = 0;

	goto loc_29563;

loc_29511:
	if((cJanus->0x10388)->0x284 == 0) {
		goto loc_29563;
	}

	(cJanus->0x10388)->0xb70 = 1;

loc_29563:
	if((cJanus->0x10388)->0xd4 != 3) {
		goto loc_29578;
	}

	(cJanus->0x10388)->0xb68 = 2;
	goto loc_2957e;

loc_29578:
	(cJanus->0x10388)->0xb68 += 1;

loc_2957e:
	delay_cnt = 0;
	goto loc_296b7;

loc_29589:
	if(delay_cnt > 0) {
		goto loc_295c3;
	}

	ebp = &(cJanus->0x10377);
	if(cJanus->0x10377 == 0) /* byte */ {
		goto loc_295a9;
	}

	/* EnableBVDO() */

	(cJanus->0x10377)->0 = 0; /* byte */

loc_295a9:
	ebp = &(cJanus->0x10364);
	if(cJanus->0x10364 == 0) /* byte */ {
		goto loc_295c9;
	}

	/* EnableAux() */

	(cJanus->0x10364) = 0; /* byte */

	goto loc_295c9;

loc_295c3:
	delay_cnt--;

loc_295c9:
	if((cJanus->0x10388)->0xb6c == 0 || cJanus->0xf8 != 0) {
		goto loc_296b7;
	}

	ecx = (cJanus->0x10388)->0xba0;
	eax = &((cJanus->0x10388)->0xba0);
	if((cJanus->0x10388)->0xba0 <= 0) {
		goto loc_295f9;
	}
	/*
	ecx -= 1;
	[eax] = ecx;
	*/
	(cJanus->0x10388)->0xba0 -= 1;

	goto loc_296b7;

loc_295f9:
	if((cJanus->0x10388)->0x27c == 0) {
		goto loc_29653;
	}

	if(1 /*DpcSync(&(cJanus->0x10388), var_10)*/) {
		goto loc_296b7;
	}

	/* UpdateDispBufReg((cJanus->0x10388)->0x1d4, (cJanus->0x10388)->0x1d8); */

	if((cJanus->0x10388)->0x284 == 0) {
		goto loc_29698;
	}

	ebp = &(cJanus->0x10388)->0xb70;
	if((cJanus->0x10388)->0xb70 == 0) {
		goto loc_29698;
	}

	/* UpdateVpipBufReg(&(cJanus->0x10388)); */

	/* StartVpip(&(cJanus->0x10388)); */

	(cJanus->0x10388)->0 = 0;

	goto loc_29698;

loc_29653:
	if((cJanus->0x10388)->0x28c == 0 || (cJanus->0x10388)->0x284 == 0) {
		goto loc_29675;
	}

	if(1 /* DeinterlaceSync(&(cJanus->0x10388), var_10); */) {
		goto loc_296b7;
	}

	goto loc_29698;

loc_29675:

	if(1 /* DpcSync(&(cJanus->0x10388), var_10) */) {
		goto loc_296b7;
	}

	/* UpdateDispBufReg((cJanus->0x10388)->0x1d4, (cJanus->0x10388)->0x1d8); */

loc_29698:
	/* eax = devGetSTCRaw(); // Demux_GetSTC() */

	ecx = 0x5a;
	edx = 0;
	eax /= 0x5a;
	(cJanus->0x10388)->0xba4 = eax;

	/* StartMcu(); */

	(cJanus->0x10388)->0xb6c = 0;

loc_296b7:
}

#endif

void tl880_dpc_video_sync(struct tl880_dev *tl880dev)
{
	unsigned int playing_dvd = 0;

	if(playing_dvd) {
		// tl880_dpc_video_sync_dvd(tl880dev);
	} else {
		// tl880_dpc_video_sync_tv(tl880dev);
	}
}

int tl880_dpc_int(struct tl880_dev *tl880dev)
{
	tl880dev->dpc_count++;

	tl880dev->dpc_type = read_register(tl880dev, 0x1000c) & read_register(tl880dev, 0x10008);

	printk(KERN_DEBUG "tl880: dpc interrupt: 0x%04lx\n", 
		tl880dev->dpc_type);


	if(tl880dev->dpc_type & 4) {
		/* isr_VideoSync(); */
	}

	if(tl880dev->dpc_type & 0x20) {
		/* IntDpcEof0 */
	}

	if(tl880dev->dpc_type & 0x40) {
		/* IntDpcEof1 */
	}

	tl880dev->dpc_type = 0;

	/* Since this interrupt isn't handled yet, disable it */
	write_register(tl880dev, 0x10008, 0);

	return 0;
}


