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

	parse_cmdline(argc, argv, &dictionary, &merged, &out, &type);

	user_hashes = create_trie();
	users_count = read_hashes(merged, user_hashes);
	bitmap_cracked = calloc(users_count, sizeof(int));

	d_attack(dictionary, out, type);

	destroy_trie(user_hashes);
	free(bitmap_cracked);
	time_taken = clock() - start_time;
	time_taken = time_taken/CLOCKS_PER_SEC;
	printf("Execution time: %lf seconds\n", time_taken);
	return 0;
}
