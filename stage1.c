void print_usage();
void parse_cmdline(int argc, char* argv[], char** restrict, char** restrict, char** restrict, dict_type* restrict);
int read_hashes(const char* restrict, trie_t restrict);


void print_usage()
{
	printf("Usage:\nguessword [-d <dictionary file>] [-i <merged file>] [-o <output file] [-m s (standard)|c (given in class)] -lpthread -lcrypt\n");
	exit(EXIT_FAILURE);
}


void parse_cmdline(int argc, char* argv[], char** restrict dictionary, char** restrict merged, char** restrict out, dict_type* restrict type)
{
	extern char* optarg;
	int option;

	while((option = getopt(argc, argv, "d:i:o:m:")) != -1)
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
			case 'm':
				if(optarg[0] == 's')
					*type = standard;
				else if(optarg[0] == 'c')
					*type = given_in_class;
				break;
			case '?':
				print_usage();
		}
	}
}

int read_hashes(const char* restrict merged_file, trie_t restrict user_hashes)
{
	FILE* merged_fd = fopen(merged_file, "rb");
	user_info_t user;
	int i = 0, j = 6, count_dollars;

	for(; fread(&user, sizeof(user), 1, merged_fd) == 1; ++i)
	{
		// Uncomment the following code if you want to generalise the finding of the $ signs.
		//	In that case, make j = 0 (or just remove it), from the declaration above.
		/*for(j = 0, count_dollars = 0; count_dollars != 3; ++j)
			if(user.hash[j] == '$')
				++count_dollars;*/

		add_pair(user_hashes, user.hash + j, i);
	}

	fclose(merged_fd);
	return i;
}
