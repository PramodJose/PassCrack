#include <stdio.h>
#include <getopt.h>
#include "trie.h"

#define DEFAULT_DICTIONARY	"top250.txt"
#define DEFAULT_MERGED	"passwordfile.txt"
#define DEFAULT_OUT	"allcrackedpasswords.txt"

trie_t user_hashes;
int* bitmap_cracked;

#include "stage1.c"
#include "d_attack.c"


int main(int argc, char* argv[])
{
	char* dictionary = DEFAULT_DICTIONARY, *merged = DEFAULT_MERGED, *out = DEFAULT_OUT;
	int users_count;

	parse_cmdline(argc, argv, &dictionary, &merged, &out);

	user_hashes = create_trie();
	users_count = read_hashes(merged, user_hashes);
	bitmap_cracked = calloc(users_count, sizeof(int));

	d_attack(dictionary, out, given_in_class);

	destroy_trie(user_hashes);
	free(bitmap_cracked);
	return 0;
}
