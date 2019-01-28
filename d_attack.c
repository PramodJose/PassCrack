#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <crypt.h>
#include "abstract_types.h"

#define	MAX_THREADS	8
#define DICT_LINE_LEN	512


typedef struct
{
	char *dictionary, *merged, temp_file_name[8];
	dict_type type;
	long int begin, end;
	FILE* out_fh;
	int cracked_count;
} thread_args;


void* thread_entry(void*);
void d_attack(char*, FILE* restrict, char*, FILE* restrict, dict_type);


void d_attack(char* dictionary, FILE* restrict dictionary_fh, char* merged, FILE* restrict out_fh, dict_type type)
{
	FILE *thread_in;
	long file_size, fraction, beg = 0, end = 0;
	int i;
	pthread_t thread_id [MAX_THREADS];
	thread_args args [MAX_THREADS];
	char line [DICT_LINE_LEN];

	fseek(dictionary_fh, 0, SEEK_END);
	file_size = ftell(dictionary_fh);
	fraction = file_size / MAX_THREADS;

	for(i = 0; i < MAX_THREADS; ++i)
	{
		if(i != MAX_THREADS - 1)
		{
			end = (i + 1) * fraction;
			fseek(dictionary_fh, end, SEEK_SET);

			while(fgetc(dictionary_fh) != '\n');

			end = ftell(dictionary_fh) - 1;
		}
		else
			end = file_size - 1;

		//call to thread
		args[i].dictionary = dictionary;
		args[i].type = type;
		args[i].begin = beg;
		args[i].end = end;
		args[i].merged = merged;
		strcpy(args[i].temp_file_name, "XXXXXX");
		args[i].out_fh = fdopen(mkstemp(args[i].temp_file_name), "w+");

		pthread_create(&thread_id[i], NULL, thread_entry, (void*) &args[i]);

		beg = end + 1;
	}

	fseek(dictionary_fh, 0, SEEK_SET);

	// joining the threads.
	for(i = 0; i < MAX_THREADS; ++i)
	{
		pthread_join(thread_id[i], NULL);
		total_cracked += args[i].cracked_count;

		thread_in = args[i].out_fh;
		fseek(thread_in, 0, SEEK_SET);

		while(fgets(line, DICT_LINE_LEN, thread_in) != NULL)
			fprintf(out_fh, "%s", line);

		fclose(thread_in);
		remove(args[i].temp_file_name);
	}
}


void* thread_entry(void* arguments)
{
	thread_args* args = arguments;
	FILE *dictionary_fh = fopen(args->dictionary, "r"), *merged_fh = fopen(args->merged, "rb");
	long int end = args->end;
	char d_line [DICT_LINE_LEN], *calculated_hash;
	int users_count, i, *users_list, algo_len = strlen(algo_n_salt);
	float f_dummy;
	struct crypt_data data;
	user_info_t user;

	args->cracked_count = 0;
	fseek(dictionary_fh, args->begin, SEEK_SET);

	do
	{
		if(args->type == given_in_class)
			fscanf(dictionary_fh, "%d%f%d%s", &users_count, &f_dummy, &users_count, d_line);
		else
		{
			fgets(d_line, DICT_LINE_LEN, dictionary_fh);
			users_count = strlen(d_line);
			d_line[users_count - 1] = '\0';
		}

		data.initialized = 0;
		calculated_hash = crypt_r(d_line, algo_n_salt, &data);
		users_list = retrieve_users(user_hashes, calculated_hash + algo_len, &users_count);
		if(users_list != NULL)
		{
			for(i = 0 ; i < users_count; ++i)
			{
				fseek(merged_fh, users_list[i] * sizeof(user), SEEK_SET);
				fread(&user, sizeof(user), 1, merged_fh);
				fprintf(args->out_fh, "%5d  |  %-15s | %-40s | %-50s |\n", users_list[i], user.user_name, user.full_name, d_line);
				bitmap_cracked[users_list[i] - 1] = 1;
			}
			args->cracked_count += users_count;
		}

	} while(ftell(dictionary_fh) < end);


	fclose(dictionary_fh);
	fclose(merged_fh);
	return NULL;
}
