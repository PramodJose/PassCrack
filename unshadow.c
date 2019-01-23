#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "abstract_types.h"

#define	DEFAULT_OUT_FILE	"passwordfile.txt"
#define	MAX_LINE_LEN	128


//	====================  Function Prototypes  ====================

static void print_usage();
static void merge_files(const char*, const char*, const char*);
static void parse_passwd(user_info_t*, char*);
static void parse_shadow(user_info_t*, char*);


//	====================  Function Definitions  ===================

int main(int argc, char* argv[])
{
	extern char *optarg;
	char *passwd_file, *shadow_file, *out_file = DEFAULT_OUT_FILE;
	char passwd_file_flag = 0, shadow_file_flag = 0;
	int option;

	while((option = getopt(argc, argv, "p:s:o:")) != -1)
	{
		switch(option)
		{
			case 'p':
				passwd_file = optarg;
				passwd_file_flag = 1;
				break;
			case 's':
				shadow_file = optarg;
				shadow_file_flag = 1;
				break;
			case 'o':
				out_file = optarg;
				break;
			case '?':
				print_usage();
		}
	}

	if(passwd_file_flag == 0 || shadow_file_flag == 0)
		print_usage();

	merge_files(passwd_file, shadow_file, out_file);

	return 0;
}


static void print_usage()
{
	printf("Usage:\nunshadow -p <passwd file> -s <shadow file> [-o <output file]\n");
	exit(1);
}


static void merge_files(const char* passwd_file, const char* shadow_file, const char* out_file)
{
	FILE *passwd_fd, *shadow_fd, *out_fd;
	char passwd_line[MAX_LINE_LEN], shadow_line[MAX_LINE_LEN];

	passwd_fd = fopen(passwd_file, "r");
	if(passwd_fd == NULL)
	{
		fprintf(stderr, "Unable to open file %s\n", passwd_file);
		return;
	}

	shadow_fd = fopen(shadow_file, "r");
	if(shadow_fd == NULL)
	{
		fprintf(stderr, "Unable to open file %s\n", shadow_file);
		fclose(passwd_fd);
		return;
	}

	out_fd = fopen(out_file, "wb");
	if(out_fd == NULL)
	{
		fprintf(stderr, "Unable to open output file %s\n", out_file);
		fclose(passwd_fd);
		fclose(shadow_fd);
		return;
	}

	while(fgets(passwd_line, MAX_LINE_LEN, passwd_fd) != NULL && fgets(shadow_line, MAX_LINE_LEN, shadow_fd) != NULL)
	{
		user_info_t user = {0, 0, 0};
		parse_passwd(&user, passwd_line);
		parse_shadow(&user, shadow_line);
		fwrite(&user, sizeof(user), 1, out_fd);
	}

	fclose(passwd_fd);
	fclose(shadow_fd);
	fclose(out_fd);
}


static void parse_passwd(user_info_t* user, char* passwd_line)
{
	int i = 0, pos = 0, colon_count = 0;

	for(; passwd_line[i] != ':' && pos != UNAME_LEN - 1; ++i, ++pos)
		user->user_name[pos] = passwd_line[i];
	user->user_name[pos] = '\0';

	if(passwd_line[i] != ':')
	{
		fprintf(stderr, "The user name field is too large. Increase the UNAME_LEN constant in abstract_types.h\n");
		exit(1);
	}

	for(++i; colon_count < 3; ++i)
		if(passwd_line[i] == ':')
			++colon_count;

	for(pos = 0; passwd_line[i] != ',' && pos != NAME_LEN - 1; ++i, ++pos)
		user->full_name[pos] = passwd_line[i];
	user->full_name[pos] = '\0';

	if(passwd_line[i] != ',')
	{
		fprintf(stderr, "The full name field is too large. Increase the NAME_LEN constant in abstract_types.h\n");
		exit(1);
	}
}


static void parse_shadow(user_info_t* user, char* shadow_line)
{
	int i = 0, pos = 0;

	for(; shadow_line[i] != ':'; ++i);

	for(++i; shadow_line[i] != ':' && pos != HASH_LEN - 1; ++i, ++pos)
		user->hash[pos] = shadow_line[i];
	user->hash[pos] = '\0';

	if(shadow_line[i] != ':')
	{
		fprintf(stderr, "The hash field is too large. Increase the HASH_LEN constant in abstract_types.h\n");
		exit(1);
	}
}
