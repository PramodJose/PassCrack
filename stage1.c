#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#define MSG_LEN	128
#define SEPARATOR_COUNT	3

void print_usage();
void parse_cmdline(int argc, char* argv[], char** restrict, char** restrict, char** restrict, dict_type* restrict);
int read_hashes(FILE*, trie_t restrict);
void setup(const char* restrict, FILE** restrict, const char* restrict, FILE** restrict, const char* restrict, FILE** restrict);

void teardown(FILE* restrict, FILE* restrict, FILE* restrict);
void timer(timer_state, FILE*, const char* restrict);


void print_usage()
{
	printf("Usage:\nguessword [-d <dictionary file>] [-i <merged file>] [-o <output file>] [-m <s (standard)|c (given in class)>] -lpthread -lcrypt\n");
	exit(EXIT_FAILURE);
}


void parse_cmdline(int argc, char* argv[], char** restrict dictionary, char** restrict merged, char** restrict out, dict_type* restrict type)
{
	extern char* optarg;
	int option, errsv;
	char message [MSG_LEN];

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

	snprintf(message, MSG_LEN, "The file %s cannot be opened for reading\n", *dictionary);
	if(euidaccess(*dictionary, R_OK) != 0)
	{
		perror(message);
		exit(EXIT_FAILURE);
	}

	snprintf(message, MSG_LEN, "The file %s cannot be opened for reading\n", *merged);
	if(euidaccess(*merged, R_OK) != 0)
	{
		perror(message);
		exit(EXIT_FAILURE);
	}
	
	snprintf(message, MSG_LEN, "The file %s cannot be opened for reading and writing\n", *out);
	//	if file exists and does not have required permissions, then show the error.
	if(euidaccess(*out, F_OK) == 0 && euidaccess(*out, R_OK | W_OK) != 0)
	{
		perror(message);
		exit(EXIT_FAILURE);
	}
}


int read_hashes(FILE* merged_fh, trie_t restrict user_hashes)
{
	user_info_t user;
	int i = 1;

	for(; fread(&user, sizeof(user), 1, merged_fh) == 1; ++i)
		add_pair(user_hashes, user.hash, i);

	fseek(merged_fh, 0, SEEK_SET);
	return (i - 1);
}


void setup(const char* restrict dictionary, FILE** restrict dict_fh, const char* restrict merged, FILE** restrict merged_fh, const char* restrict out, FILE** restrict out_fh)
{
	*dict_fh = fopen(dictionary, "r");
	*merged_fh = fopen(merged, "rb");
	*out_fh = fopen(out, "w");

	fread(algo_n_salt, sizeof(algo_n_salt), 1, *merged_fh);

	user_hashes = create_trie();
	users_count = read_hashes(*merged_fh, user_hashes);
	bitmap_cracked = calloc(users_count, sizeof(int));

	fprintf(*out_fh, "-------+------------------+------------------------------------------+----------------------------------------------------+\n");
	fprintf(*out_fh, "%5s  |  %-15s | %-40s | %-50s |\n", "NUM", "USER NAME", "FULL NAME", "PASSWORD");
	fprintf(*out_fh, "-------+------------------+------------------------------------------+----------------------------------------------------+\n");
}


void teardown(FILE* restrict dict_fh, FILE* restrict merged_fh, FILE* restrict out_fh)
{
	fclose(dict_fh);
	fclose(merged_fh);
	fclose(out_fh);

	destroy_trie(user_hashes);
	free(bitmap_cracked);
}


void timer(timer_state state, FILE* out_fh, const char* restrict out)
{
	static struct timespec start = {0, 0}, end = {0, 0};
	if(state == start_timer)
		clock_gettime(CLOCK_MONOTONIC, &start);
	else
	{
		long int seconds_elapsed;
		clock_gettime(CLOCK_MONOTONIC, &end);
		seconds_elapsed = end.tv_sec - start.tv_sec;

		fprintf(out_fh, "-------+------------------+------------------------------------------+----------------------------------------------------+\n");
		fprintf(out_fh, "\n\nTotal passwords cracked = %d\n", total_cracked);
		printf("Total passwords cracked = %d\n", total_cracked);

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
		
		printf("The output has been stored in %s\n", out);
	}
}
