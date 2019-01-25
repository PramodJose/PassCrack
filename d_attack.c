#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <crypt.h>

#define	MAX_THREADS	8
#define LINE_LEN	400

typedef struct
{
	char *dictionary, temp_file_name[8];
	dict_type type;
	long int begin, end;
	FILE* out_fh;
	int cracked_count;
} thread_args;


void* thread_entry(void*);


void d_attack(char* restrict dictionary, char* restrict out, dict_type type)
{
	FILE *dictionary_fh = fopen(dictionary, "r"), *out_fh = fopen(out, "w"), *thread_in;
	long file_size, fraction, beg = 0, end = 0;
	int i;
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
		strcpy(args[i].temp_file_name, "XXXXXX");
		args[i].out_fh = fdopen(mkstemp(args[i].temp_file_name), "w+");
		printf("%d -> %s\n", i, args[i].temp_file_name);

		pthread_create(&thread_id[i], NULL, thread_entry, (void*) &args[i]);

		beg = end + 1;
	}

	fclose(dictionary_fh);

	// joining the threads.
	for(i = 0; i < MAX_THREADS; ++i)
	{
		printf("I\'m waiting\n");
		pthread_join(thread_id[i], NULL);

		thread_in = args[i].out_fh;
		fseek(thread_in, 0, SEEK_SET);

		while(fgets(line, LINE_LEN, thread_in) != NULL)
			fprintf(out_fh, "%s", line);

		fclose(thread_in);
		remove(args[i].temp_file_name);
	}

	fclose(out_fh);
}


void* thread_entry(void* arguments)
{
	thread_args* args = arguments;
	FILE* dictionary_fh = fopen(args->dictionary, "r");
	long int end = args->end;
	char d_line [LINE_LEN], *calculated_hash;
	int users_count, i, *users_list;
	float f_dummy;
	struct crypt_data data;
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
				fprintf(args->out_fh, "%d ", users_list[i]);
			fprintf(args->out_fh, "%s\n\n", d_line);
		}

	} while(ftell(dictionary_fh) < end);


	fclose(dictionary_fh);
	return NULL;
}
