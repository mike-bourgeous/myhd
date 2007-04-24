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

/*
void __stdcall	SetBits(__u32 *,long,long,long,unsigned	long)
arg_0	       = dword ptr  8 value
high_bit	       = dword ptr  10h 
low_bit	       = dword ptr  14h
setvalue	       = dword ptr  18h

i.e.:	SetBits(&somevalue, 0x3008, 0, 0, 1);
	SetBits(&somevalue, 0x10008, 9, 9, 0);
	SetBits(&somevalue, 0x10008, 5, 5, 1);
	SetBits(&somevalue, 0x10008, 0xe, 0xd, 0);
*/

static __u32 *regspace = NULL;
static int memfd = 0;

int map_regspace()
{
	if((memfd = open("/dev/tl880/reg0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/reg0");
		return -1;
	}
	if(!(regspace = mmap(0, 0x00100000, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, 0))) {
		perror("Failed to mmap tl880 register space");
		close(memfd);
		return -1;
	}

	return 0;
}

void unmap_regspace()
{
	if(munmap(regspace, 0x00100000)) {
		perror("Unable to unmap tl880 register space");
	}
	if(close(memfd)) {
		perror("Unable to close /dev/tl880/reg0");
	}
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


void set_bits(__u32 *value, __u32 reg, int high_bit, int low_bit, __u32 setvalue)
{
	register __u32 mask = 0;

	/* set bits from high_bit to low_bit in mask to 1 */
	mask = ~(0xFFFFFFFF << (high_bit - low_bit + 1)) << low_bit;
	setvalue <<= low_bit;
	setvalue &= mask;
	mask = ~mask & *value;
	setvalue |= mask;
	*value = setvalue;
}

int main(int argc, char *argv[])
{
	__u32 value;

	if(argc != 5) {
		fprintf(stderr, "Usage: %s oldvalue(hex) high_bit(hex) low_bit(hex) bitvalue(hex)\n", argv[0]);
		return -1;
	}
	
	if(map_regspace()) {
		return -1;
	}

	value = strtoul(argv[1], NULL, 16);

	set_bits(&value, 0, strtoul(argv[2], NULL, 16), strtoul(argv[3], NULL, 16), strtoul(argv[4], NULL, 16));

	printf("New value is 0x%08x\n", value);

	unmap_regspace();

	return 0;
}


