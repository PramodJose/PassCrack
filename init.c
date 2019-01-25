int read_hashes(const char* restrict merged_file, trie_t user_hashes)
{
	FILE* merged_fd = fopen(merged_file, "rb");
	user_info_t user;
	int i = 0, j, count_dollars;

	for(; fread(&user, sizeof(user), 1, merged_fd) == 1; ++i)
	{
		for(j = 0, count_dollars = 0; count_dollars != 3; ++j)
			if(user.hash[j] == '$')
				++count_dollars;

		add_pair(user_hashes, user.hash + j, i);
	}

	fclose(merged_fd);
	return i;
}
