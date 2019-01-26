#define _GNU_SOURCE
#include <stdio.h>
#include <getopt.h>
#include <time.h>
#include "trie.h"

trie_t user_hashes;
int* bitmap_cracked;
typedef enum {standard, given_in_class} dict_type;

#define DEFAULT_DICTIONARY	"top250.txt"
#define DEFAULT_MERGED	"passwordfile.txt"
#define DEFAULT_OUT	"allcrackedpasswords.txt"
#define DEFAULT_DICT_TYPE given_in_class

#include "stage1.c"
#include "d_attack.c"


int main(int argc, char* argv[])
{
	struct timespec start = {0, 0}, end = {0, 0};
	char* dictionary = DEFAULT_DICTIONARY, *merged = DEFAULT_MERGED, *out = DEFAULT_OUT;
	dict_type type = DEFAULT_DICT_TYPE;
	int users_count;
	FILE *out_fh;
	long int seconds_elapsed;

	clock_gettime(CLOCK_MONOTONIC, &start);

	parse_cmdline(argc, argv, &dictionary, &merged, &out, &type);

	user_hashes = create_trie();
	users_count = read_hashes(merged, user_hashes);
	bitmap_cracked = calloc(users_count, sizeof(int));

	d_attack(dictionary, merged, out, type);

	destroy_trie(user_hashes);
	free(bitmap_cracked);

	clock_gettime(CLOCK_MONOTONIC, &end);
	seconds_elapsed = end.tv_sec - start.tv_sec;

	out_fh = fopen(out, "a");

	if(seconds_elapsed < 2)				// We can offer a little more precision if the seconds elapsed is less than 2.
	{
		double precise_delta = (end.tv_nsec - start.tv_nsec) / 10E9;
		printf("Execution time: %lf seconds\n", precise_delta);
		fprintf(out_fh, "Execution time: %lf seconds\n", precise_delta);
	}
	else if(seconds_elapsed > 600)
	{
		float minutes_elapsed = seconds_elapsed / 60.0;
		printf("Execution time: %lf minutes\n", minutes_elapsed);
		fprintf(out_fh, "Execution time: %lf minutes\n", minutes_elapsed);
	}
	else
	{
		printf("Execution time: %ld seconds\n", seconds_elapsed);
		fprintf(out_fh, "Execution time: %ld seconds\n", seconds_elapsed);
	}
	
	fclose(out_fh);
	printf("The output has been stored in %s\n", out);
	return 0;
}
