#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev.h>
#include "tl880.h"

typedef struct {
	char name[10];
	unsigned long freq;
} band_entry_t;

// Modified from bands.h in the tvtime source distribution (tvtime.sf.net)
const band_entry_t us_bcast[] = {
    { "2",   55250 }, { "3",   61250 }, { "4",   67250 },
    { "5",   77250 }, { "6",   83250 }, { "7",  175250 },
    { "8",  181250 }, { "9",  187250 }, { "10", 193250 },
    { "11", 199250 }, { "12", 205250 }, { "13", 211250 },
    { "14", 471250 }, { "15", 477250 }, { "16", 483250 },
    { "17", 489250 }, { "18", 495250 }, { "19", 501250 },
    { "20", 507250 }, { "21", 513250 }, { "22", 519250 },
    { "23", 525250 }, { "24", 531250 }, { "25", 537250 },
    { "26", 543250 }, { "27", 549250 }, { "28", 555250 },
    { "29", 561250 }, { "30", 567250 }, { "31", 573250 },
    { "32", 579250 }, { "33", 585250 }, { "34", 591250 },
    { "35", 597250 }, { "36", 603250 }, { "37", 609250 },
    { "38", 615250 }, { "39", 621250 }, { "40", 627250 },
    { "41", 633250 }, { "42", 639250 }, { "43", 645250 },
    { "44", 651250 }, { "45", 657250 }, { "46", 663250 },
    { "47", 669250 }, { "48", 675250 }, { "49", 681250 },
    { "50", 687250 }, { "51", 693250 }, { "52", 699250 },
    { "53", 705250 }, { "54", 711250 }, { "55", 717250 },
    { "56", 723250 }, { "57", 729250 }, { "58", 735250 },
    { "59", 741250 }, { "60", 747250 }, { "61", 753250 },
    { "62", 759250 }, { "63", 765250 }, { "64", 771250 },
    { "65", 777250 }, { "66", 783250 }, { "67", 789250 },
    { "68", 795250 }, { "69", 801250 }, { "70", 807250 },
    { "71", 813250 }, { "72", 819250 }, { "73", 825250 },
    { "74", 831250 }, { "75", 837250 }, { "76", 843250 },
    { "77", 849250 }, { "78", 855250 }, { "79", 861250 },
    { "80", 867250 }, { "81", 873250 }, { "82", 879250 },
    { "83", 885250 }, {   "",      0 }
};

void usage(char *name)
{
	printf("Usage:\n");
	printf("\t%s channel\n\n", name);
	printf("Where channel is one of the us-bcast channels (02-83)\n");
}

int main(int argc, char *argv[])
{
	int devfd;
	unsigned long freq;
	int i;

	if((argc >= 2 && !strncmp(argv[1], "--help", 6)) || argc != 2) {
		usage(argv[0]);
		return -1;
	}

	if((devfd = open("/dev/tl880/reg0", O_RDWR)) < 0) {
		perror("Unable to open /dev/tl880/reg0");
		return -1;
	}

	for(i = 0; us_bcast[i].freq != 0 && strncmp(us_bcast[i].name, argv[1], 10); i++) {
	}
	freq = us_bcast[i].freq;

	if(freq == 0) {
		fprintf(stderr, "Invalid channel: %s\n", argv[1]);
		close(devfd);
		return -1;
	}

	printf("Changing to channel %s, %0.4f MHz (ioctl value %ld)\n", 
			argv[1], (float)freq / 1000.0, freq / 16);

	freq *= 16;
	freq /= 1000;
	if(ioctl(devfd, VIDIOCSFREQ, &freq)) {
		perror("Unable to change frequency");
		close(devfd);
		return -1;
	}

	close(devfd);

	return 0;
}

