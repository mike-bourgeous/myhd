/* hexcount.c - utility for use in bash scripts */
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	unsigned long start;
	unsigned long skip;
	unsigned long end;
	unsigned long i;
	
	if(argc != 4) {
		printf("Counts in hexadecimal.  Usage:\n\n\t%s start skip end\n", argv[0]);
		return -1;
	}

	start = strtoul(argv[1], NULL, 16);
	skip = strtoul(argv[2], NULL, 16);
	end = strtoul(argv[3], NULL, 16);

	for(i = start; i <= end; i += skip) {
		printf("%08lx\n", i);
	}

	return 0;
}
