/* 
 * Upload firmware to tl880 card
 * (c) 2003 Mike Bourgeous
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <asm/byteorder.h>
#include <linux/byteorder/swab.h>
#include <linux/byteorder/generic.h>

static __u32 *regspace = NULL;
static int memfd = 0;

int map_regspace()
{
	if((memfd = open("/dev/tl880/reg0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/reg0");
		return -1;
	}
	if((regspace = mmap(0, 0x00100000, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, 0)) < 0) {
		perror("Failed to mmap tl880 register space");
		close(memfd);
		return -1;
	}

	return 0;
}

void unmap_regspace()
{
	munmap(regspace, 0x00100000);
	close(memfd);
}

void write_register(__u32 reg, __u32 value)
{
	if(!regspace) {
		return;
	}
	regspace[reg / 4] = __cpu_to_le32(value);
}

__u32 read_register(__u32 reg)
{
	__u32 value;
	if(!regspace) {
		return 0;
	}
	value = __le32_to_cpu(regspace[reg / 4]);
	return value;
}

int upload_mcu_firmware(int insfd, int datfd)
{
	__u32 inslen = 0, datlen = 0;
	struct stat statbuf;
	unsigned short *insbuf, *datbuf;
	int i;

	if(fstat(insfd, &statbuf)) {
		perror("Unable to stat insfd");
		return -1;
	}

	inslen = statbuf.st_size / 2;

	if(fstat(datfd, &statbuf)) {
		perror("Unable to stat datfd");
		return -1;
	}

	datlen = statbuf.st_size / 2;

	if(!(insbuf = calloc(inslen, 2))) {
		perror("Unable to allocate insbuf");
		return -1;
	}

	if(!(datbuf = calloc(datlen, 2))) {
		perror("Unable to allocate datbuf");
		free(insbuf);
		return -1;
	}

	if(read(insfd, insbuf, inslen * 2) != inslen * 2) {
		perror("Unable to read insbuf from insfd");
		free(insbuf);
		free(datbuf);
		return -1;
	}
	
	if(read(datfd, datbuf, datlen * 2) != datlen * 2) {
		perror("Unable to read datbuf from datfd");
		free(insbuf);
		free(datbuf);
		return -1;
	}

	write_register(0x1d800, 5);
	write_register(0x1d804, 0);

	for(i = 0; i < inslen; i++) {
		write_register(0x18000 + i * 4, insbuf[i]);
	}
	for(i = 0; i < datlen; i++) {
		write_register(0x1c000 + i * 4, datbuf[i]);
	}

	write_register(0x1c2f0, 0);
	write_register(0x1d804, 0);
	write_register(0x1d800, 6);

	/* 
	 * I don't really know what this is supposed to accomplish, but it's
	 * what the Windows driver does after uploading the firmware
	 */
	while(!read_register(0x1d804) || read_register(0x1d800) != 5) {
		printf("waiting on register 0x1d804 and 0x1d800\n");
		if(read_register(0x1d804) == 1) {
			break;
		}
		sleep(1);
	}

	write_register(0x1d804, 0);

	free(insbuf);
	free(datbuf);

	return 0;
}

int upload_tsd_firmware(int insfd, int datfd)
{
	__u32 inslen = 0, datlen = 0;
	struct stat statbuf;
	unsigned short *insbuf, *datbuf;
	int i;
	__u32 reg;

	if(fstat(insfd, &statbuf)) {
		perror("Unable to stat insfd");
		return -1;
	}

	inslen = statbuf.st_size / 2;

	/*
	if(fstat(datfd, &statbuf)) {
		perror("Unable to stat datfd");
		return -1;
	}

	datlen = statbuf.st_size / 2;
	*/

	if(!(insbuf = calloc(inslen, 2))) {
		perror("Unable to allocate insbuf");
		return -1;
	}

	if(!(datbuf = calloc(datlen, 2))) {
		perror("Unable to allocate datbuf");
		free(insbuf);
		return -1;
	}

	if(read(insfd, insbuf, inslen * 2) != inslen * 2) {
		perror("Unable to read insbuf from insfd");
		free(insbuf);
		free(datbuf);
		return -1;
	}
	
	/*
	if(read(datfd, datbuf, datlen * 2) != datlen * 2) {
		perror("Unable to read datbuf from datfd");
		free(insbuf);
		free(datbuf);
		return -1;
	}
	*/

	for(i = 0; i < inslen; i++) {
		write_register(0x20000 + i * 4, insbuf[i]);
	}

	for(i = 0; i < 0x600; i++) {
		write_register(0x24000 + i * 4, 0);
	}

	reg = 0x24570;

	write_register(reg - 8, 0x3fff);
	write_register(reg - 4, 0xa0);
	write_register(reg, 0);
	write_register(reg + 4, 0);
	write_register(reg + 8, 0);

	write_register(0x25718, 0);
	write_register(0x2571c, 0);
	write_register(0x25720, 0);
	write_register(0x25724, 0);
	write_register(0x25728, 0);
	write_register(0x2572C, 0);
	write_register(0x25730, 0);
	write_register(0x25734, 0);
	write_register(0x25738, 0);
	write_register(0x2573C, 0);
	write_register(0x25740, 0);
	write_register(0x25744, 0);
	write_register(0x25748, 0);
	write_register(0x2574C, 0);
	write_register(0x25750, 0);
	write_register(0x25754, 0);
	write_register(0x25758, 0);
	write_register(0x2575C, 0);
	write_register(0x25760, 0);
	write_register(0x25764, 0);
	write_register(0x257F0, 0);
	write_register(0x257F4, 0);
	write_register(0x257F8, 0);
	write_register(0x257FC, 0);
	write_register(0x27810, 0x5000);

	/* last queued interrupt */
	read_register(0x27814);


	write_register(0x26804, 0);
	write_register(0x26800, 6);
	
	while(read_register(0x26804) != 0 && read_register(0x26800) != 5) {
		if(read_register(0x26804) == 1) {
			write_register(0x26804, 0);
			write_register(0x26800, 6);
			printf("Waiting on 0x26804 and 0x26800\n");
			break;
		}
	}

	write_register(0x26800, 0);

	free(insbuf);
	free(datbuf);
	return 0;
}

int main()
{
	int tsd_ins, mcu_ins, mcu_dat/*, tsd_dat */;
	
	if((tsd_ins = open("tsdfirmware.bin", O_RDONLY)) < 0) {
		perror("Unable to open tsdfirmware.bin");
		return -1;
	}
	/*
	if((tsd_dat = open("tsddata.bin", O_RDONLY)) < 0) {
		perror("Unable to open tsddata.bin");
		close(tsd_ins);
		return -1;
	}
	*/
	if((mcu_ins = open("mcufirmware.bin", O_RDONLY)) < 0) {
		perror("Unable to open mcufirmware.bin");
		close(tsd_ins);
		/* close(tsd_dat); */
		return -1;
	}
	if((mcu_dat = open("mcudata.bin", O_RDONLY)) < 0) {
		perror("Unable to open mcudata.bin");
		close(tsd_ins);
		/* close(tsd_dat); */
		close(mcu_ins);
		return -1;
	}

	if(map_regspace()) {
		close(tsd_ins);
		/* close(tsd_dat); */
		close(mcu_ins);
		close(mcu_dat);
		return -1;
	}
	
	write_register(0x6000, 0xc000);

	upload_mcu_firmware(mcu_ins, mcu_dat);
	upload_tsd_firmware(tsd_ins, 0);

	/* Start tsd? */
	write_register(0x24040, 1);

	unmap_regspace();
	close(tsd_ins);
	/* close(tsd_dat); */ 
	close(mcu_ins);
	close(mcu_dat);
	return 0;
}

