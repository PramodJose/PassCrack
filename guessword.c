#include <stdio.h>
#include <getopt.h>
#include "trie.h"

#define DEFAULT_DICTIONARY	"top250.txt"
#define DEFAULT_MERGED	"passwordfile.txt"
#define DEFAULT_OUT	"allcrackedpasswords.txt"

trie_t user_hashes;
int* bitmap_cracked;

#include "init.c"

void print_usage();
void parse_cmdline(int argc, char* argv[], char** restrict, char** restrict, char** restrict);


int main(int argc, char* argv[])
{
	char* dictionary = DEFAULT_DICTIONARY, *merged = DEFAULT_MERGED, *out = DEFAULT_OUT;
	int *list, count = 0, i;

	parse_cmdline(argc, argv, &dictionary, &merged, &out);
	printf("d: %s, m: %s, o: %s\n", dictionary, merged, out);

	user_hashes = create_trie();
	read_hashes(merged, user_hashes);
	list = retrieve_users(user_hashes, "t2R7Ly1RLRgoF/FcgmZHo0", &count);

	if(list == NULL)
		printf("Did not find anything\n");
	else
		for(i = 0; i < count; ++i)
			printf("%d ", list[i]);

	destroy_trie(user_hashes);
}


void print_usage()
{
	printf("Usage:\nguessword [-d <dictionary file>] [-i <merged file>] [-o <output file]\n");
	exit(EXIT_FAILURE);
}


void parse_cmdline(int argc, char* argv[], char** restrict dictionary, char** restrict merged, char** restrict out)
{
	extern char* optarg;
	int option;

	while((option = getopt(argc, argv, "d:i:o:")) != -1)
	{
		switch(option)
		{
			case 'd':
				*dictionary = optarg;
				break;
			case 'i':
				*merged = optarg;
				break;
			case 'o':
				*out = optarg;
				break;
			case '?':
				print_usage();
		}
	}
}
