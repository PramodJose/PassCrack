#define _GNU_SOURCE
#include <stdio.h>
#include <getopt.h>
#include <time.h>
#include "trie.h"

trie_t user_hashes = NULL;
int *bitmap_cracked = NULL, total_cracked = 0, users_count = 0;
typedef enum {standard, given_in_class} dict_type;
typedef enum {start_timer, stop_timer} timer_state;

#define DEFAULT_DICTIONARY	"top250.txt"
#define DEFAULT_MERGED	"passwordfile.txt"
#define DEFAULT_OUT	"allcrackedpasswords.txt"
#define DEFAULT_DICT_TYPE given_in_class

#include "stage1.c"
#include "d_attack.c"


int main(int argc, char* argv[])
{
	char* dictionary = DEFAULT_DICTIONARY, *merged = DEFAULT_MERGED, *out = DEFAULT_OUT;
	dict_type type = DEFAULT_DICT_TYPE;
	FILE *dict_fh, *merged_fh, *out_fh;

	timer(start_timer, NULL, NULL);
	parse_cmdline(argc, argv, &dictionary, &merged, &out, &type);
	setup(dictionary, &dict_fh, merged, &merged_fh, out, &out_fh);

	d_attack(dictionary, dict_fh, merged, out_fh, type);

	timer(stop_timer, out_fh, out);
	teardown(dict_fh, merged_fh, out_fh);
	return 0;
}
