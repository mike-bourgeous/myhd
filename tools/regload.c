#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
	int regfd, unkfd, ifd;
	int bytes;
	unsigned char *regspace;
	unsigned char *unkspace;

	if(argc < 2 || argc > 3) {
		printf("Reloads tl880 registers and optionally the 64kb \"unknown purpose\" region\n");
		printf("Usage: %s reg_file [unk_file]\n", argv[0]);
		return -1;
	}

	if((regfd = open("/dev/tl880/reg0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/reg0");
		return -1;
	}

	if((unkfd = open("/dev/tl880/unk0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/unk0");
		close(regfd);
	}
		
	printf("Mapping register space\n");
	if(!(regspace = mmap(0, 0x00100000, PROT_READ | PROT_WRITE, MAP_SHARED, regfd, 0))) {
		perror("Failed to mmap region 2 (reg)");
		close(regfd);
		close(unkfd);
		return -1;
	}
	printf("Mapping unknown region\n");
	if(!(unkspace = mmap(0, 0x00010000, PROT_READ | PROT_WRITE, MAP_SHARED, unkfd, 0))) {
		perror("Failed to mmap region 3 (unk)");
		munmap(regspace, 0x00100000);
		close(regfd);
		close(unkfd);
		return -1;
	}

	printf("Writing register space\n");
	if((ifd = open(argv[1], O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) {
		fprintf(stderr, "Failed to open output file %s: %s\n", argv[1], strerror(errno));
		munmap(regspace, 0x00100000);
		munmap(unkspace, 0x00010000);
		close(regfd);
		close(unkfd);
		return -1;
	}
	if((bytes = read(ifd, regspace, 0x00100000)) == -1) {
		perror("Failed to read region 2");
		munmap(regspace, 0x00100000);
		munmap(unkspace, 0x00010000);
		close(regfd);
		close(unkfd);
		close(ifd);
		return -1;
	}
	printf("Read %i bytes\n", bytes);
	close(ifd);

	if(argc == 3) {
		printf("Writing unknown region\n");
		if((ifd = open(argv[2], O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) {
			fprintf(stderr, "Failed to open output file %s: %s\n", argv[2], strerror(errno));
			munmap(regspace, 0x00100000);
			munmap(unkspace, 0x00010000);
			close(regfd);
			close(unkfd);
			return -1;
		}
		if(read(ifd, unkspace, 0x00010000) != 0x00010000) {
			perror("Failed to read region 3");
			munmap(regspace, 0x00100000);
			munmap(unkspace, 0x00010000);
			close(regfd);
			close(unkfd);
			close(ifd);
			return -1;
		}
		close(ifd);
	}
	
	printf("Unmapping register space\n");
	munmap(regspace, 0x00100000);
	printf("Unmapping unknown space\n");
	munmap(unkspace, 0x00010000);
	close(regfd);
	close(unkfd);

	return 0;
}

