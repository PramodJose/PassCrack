#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <crypt.h>
#include "abstract_types.h"

#define	MAX_THREADS	8
#define LINE_LEN	512

typedef struct
{
	char *dictionary, *merged, temp_file_name[8];
	dict_type type;
	long int begin, end;
	FILE* out_fh;
	int cracked_count;
} thread_args;


void* thread_entry(void*);


void d_attack(char* restrict dictionary, char* restrict merged, char* restrict out, dict_type type)
{
	FILE *dictionary_fh = fopen(dictionary, "r"), *out_fh = fopen(out, "w"), *thread_in;
	long file_size, fraction, beg = 0, end = 0;
	int i, total_cracked = 0;
	pthread_t thread_id [MAX_THREADS];
	thread_args args [MAX_THREADS];
	char line [LINE_LEN];

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

	fclose(dictionary_fh);

	// joining the threads.
	for(i = 0; i < MAX_THREADS; ++i)
	{
		pthread_join(thread_id[i], NULL);
		total_cracked += args[i].cracked_count;

		thread_in = args[i].out_fh;
		fseek(thread_in, 0, SEEK_SET);

		while(fgets(line, LINE_LEN, thread_in) != NULL)
			fprintf(out_fh, "%s", line);

		fclose(thread_in);
		remove(args[i].temp_file_name);
	}

	fprintf(out_fh, "\n\nTotal passwords cracked = %d\n", total_cracked);
	printf("Total passwords cracked = %d\n", total_cracked);
	fclose(out_fh);
}


void* thread_entry(void* arguments)
{
	thread_args* args = arguments;
	FILE *dictionary_fh = fopen(args->dictionary, "r"), *merged_fh = fopen(args->merged, "r");
	long int end = args->end;
	char d_line [LINE_LEN], *calculated_hash;
	int users_count, i, *users_list;
	float f_dummy;
	struct crypt_data data;
	user_info_t user;
	data.initialized = 0;

	args->cracked_count = 0;
	fseek(dictionary_fh, args->begin, SEEK_SET);

	do
	{
		if(args->type == given_in_class)
			fscanf(dictionary_fh, "%d%f%d%s", &users_count, &f_dummy, &users_count, d_line);
		else
		{
			fgets(d_line, LINE_LEN, dictionary_fh);
			users_count = strlen(d_line);
			d_line[users_count - 1] = '\0';
		}

		//fprintf(args->out_fh, "%s\n", d_line);
		data.initialized = 0;
		calculated_hash = crypt_r(d_line, "$1$GC$", &data);
		users_list = retrieve_users(user_hashes, calculated_hash + 6, &users_count);
		if(users_list != NULL)
		{
			for(i = 0 ; i < users_count; ++i)
			{
				fseek(merged_fh, users_list[i] * sizeof(user), SEEK_SET);
				fread(&user, sizeof(user), 1, merged_fh);
				fprintf(args->out_fh, "%5d  |  %-15s | %-40s | %-50s\n", users_list[i] + 1, user.user_name, user.full_name, d_line);
			}
			args->cracked_count += users_count;
		}

	} while(ftell(dictionary_fh) < end);


	fclose(dictionary_fh);
	fclose(merged_fh);
	return NULL;
}
