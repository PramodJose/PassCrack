//#include <pthreads.h>
#define	MAX_THREADS	8

typedef enum {standard, given_in_class} dict_type;

void debug(char* restrict dictionary, long beg, long end, int id);

void d_attack(char* restrict dictionary, dict_type type)
{
	FILE* dictionary_in = fopen(dictionary, "r");
	long file_size, beg = 0, end = 0;
	int i, pos_in_file;

	fseek(dictionary_in, 0, SEEK_END);
	file_size = ftell(dictionary_in);

	for(i = 1; i <= MAX_THREADS; ++i)
	{
		if(i != MAX_THREADS)
		{
			pos_in_file = i * (file_size / MAX_THREADS);
			fseek(dictionary_in, pos_in_file, SEEK_SET);

			while(fgetc(dictionary_in) != '\n');

			end = ftell(dictionary_in) - 1;
		}
		else
			end = file_size - 1;

		//call to thread
		debug(dictionary, beg, end, i);
		beg = end + 1;
	}

	fclose(dictionary_in);
}


void debug(char* restrict dictionary, long beg, long end, int id)
{
	FILE* fin = fopen(dictionary, "r");
	char buffer[64];
	int count = 0;

	fseek(fin, beg, SEEK_SET);

	printf("My ID is %d\n", id);
	do
	{
		fgets(buffer, 64, fin);
		printf("%s", buffer);
		++count;
	} while(ftell(fin) < end);

	printf("count: %d\n\n", count);
	fprintf(stderr, "count: %d\n\n", count);

	fclose(fin);
}