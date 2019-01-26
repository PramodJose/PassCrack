#define _GNU_SOURCE
#include <stdio.h>
#include <getopt.h>
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
	clock_t start_time = clock();
	float time_taken;
	char* dictionary = DEFAULT_DICTIONARY, *merged = DEFAULT_MERGED, *out = DEFAULT_OUT;
	dict_type type = DEFAULT_DICT_TYPE;
	int users_count;
	FILE *out_fh;

	parse_cmdline(argc, argv, &dictionary, &merged, &out, &type);

	user_hashes = create_trie();
	users_count = read_hashes(merged, user_hashes);
	bitmap_cracked = calloc(users_count, sizeof(int));

	d_attack(dictionary, merged, out, type);

	destroy_trie(user_hashes);
	free(bitmap_cracked);
	time_taken = clock() - start_time;
	time_taken = time_taken/CLOCKS_PER_SEC;

	out_fh = fopen(out, "a");
	if(time_taken > 600)
	{
		time_taken /= 60;
		printf("Execution time: %lf minutes\n", time_taken);
		fprintf(out_fh, "Execution time: %lf minutes\n", time_taken);
	}
	else
	{
		printf("Execution time: %lf seconds\n", time_taken);
		fprintf(out_fh, "Execution time: %lf seconds\n", time_taken);
	}
	
	fclose(out_fh);
	printf("The output has been stored in %s\n", out);
	return 0;
}
