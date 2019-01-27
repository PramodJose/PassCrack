#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define SUB_LINE_LEN	64

typedef struct
{
	char sub[8][5];		// Assuming there are a maximum of 8 total substitutions = 2 (upper + lower case) and 6 (substitutions). 
	int count;			// And that each substitution string is not longer than 5 chars (including NULL).
} sub_t;

sub_t alpha_sub[26];	// Never declare this as a local variable. read_substitutions() relies on the fact that global strings are
						// initialised with NULLs and count is initialised to 0.


void read_substitutions(const char* restrict rule)
{
	FILE* rule_fh = NULL;
	bool rule_present = true;
	char toggle_case = ' ';
	int i, sub_count;

	if(rule == NULL)
		rule_present = false;
	else
		rule_fh = fopen(rule, "r");

	for(i = 0; i < 26; ++i)		// repeat for 26 chars.
	{
		alpha_sub[i].sub[0][0] = 'a' + i;
		alpha_sub[i].sub[1][0] = 'A' + i;
		alpha_sub[i].count = 2;

		if(rule_present)
		{
			fscanf(rule_fh, "%d", &sub_count);

			if(sub_count != 0)
			{
				int next_pos = alpha_sub[i].count;
				for(; sub_count != 0; --sub_count, ++next_pos)
					fscanf(rule_fh, "%s", alpha_sub[i].sub[next_pos]);

				alpha_sub[i].count = next_pos;
			}
		}
	}

	if(rule_present)
		fclose(rule_fh);
}
