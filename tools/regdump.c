#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
	int ifdr, ifdu, ofd;
	unsigned char *regspace;
	unsigned char *unkspace;

	if(argc < 2 || argc > 3) {
		printf("Dumps tl880 registers and optionally the 64kb \"unknown purpose\" region\n");
		printf("Usage: %s reg_file [unk_file]\n", argv[0]);
		return -1;
	}

	if((ifdr = open("/dev/tl880/reg0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/reg0");
		return -1;
	}

	if((ifdu = open("/dev/tl880/unk0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/unk0");
		close(ifdr);
	}
		
	printf("Mapping register space\n");
	if(!(regspace = mmap(0, 0x00100000, PROT_READ | PROT_WRITE, MAP_SHARED, ifdr, 0))) {
		perror("Failed to mmap region 2 (reg)");
		close(ifdr);
		close(ifdu);
		return -1;
	}
	printf("Mapping unknown region\n");
	if(!(unkspace = mmap(0, 0x00010000, PROT_READ | PROT_WRITE, MAP_SHARED, ifdu, 0))) {
		perror("Failed to mmap region 3 (unk)");
		munmap(regspace, 0x00100000);
		close(ifdr);
		close(ifdu);
		return -1;
	}

	printf("Writing register space\n");
	if((ofd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) {
		fprintf(stderr, "Failed to open output file %s: %s\n", argv[1], strerror(errno));
		munmap(regspace, 0x00100000);
		munmap(unkspace, 0x00010000);
		close(ifdr);
		close(ifdu);
		return -1;
	}
	if(write(ofd, regspace, 0x00100000) != 0x00100000) {
		perror("Failed to write region 2\n");
		munmap(regspace, 0x00100000);
		munmap(unkspace, 0x00010000);
		close(ifdr);
		close(ifdu);
		close(ofd);
		return -1;
	}
	close(ofd);

	if(argc == 3) {
		printf("Writing unknown region\n");
		if((ofd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) {
			fprintf(stderr, "Failed to open output file %s: %s\n", argv[2], strerror(errno));
			munmap(regspace, 0x00100000);
			munmap(unkspace, 0x00010000);
			close(ifdr);
			close(ifdu);
			return -1;
		}
		if(write(ofd, unkspace, 0x00010000) != 0x00010000) {
			perror("Failed to write region 3\n");
			munmap(regspace, 0x00100000);
			munmap(unkspace, 0x00010000);
			close(ifdr);
			close(ifdu);
			close(ofd);
			return -1;
		}
		close(ofd);
	}
	
	printf("Unmapping register space\n");
	munmap(regspace, 0x00100000);
	printf("Unmapping unknown space\n");
	munmap(unkspace, 0x00010000);
	close(ifdr);
	close(ifdu);

	return 0;
}

