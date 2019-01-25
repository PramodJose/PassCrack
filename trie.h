#ifndef _TRIE_H
#define _TRIE_H 1

#include <malloc.h>					// for dynamic memory allocations
#include <stdio.h>					// for writing to streams
#include <stdlib.h>					// for exit(), in case abnormal conditions rear their ugly head :P
#include <stdbool.h>
#include "abstract_types.h"

#define	DEFAULT_CHILD_ARR_SIZE	8
#define	DEFAULT_USERID_ARR_SIZE	4


//	============================  Structs/Types  ============================

typedef enum {non_leaf, leaf} node_type;
struct trie_node;										// Forward declaration
typedef struct trie_node* trie_t;

typedef struct char_n_addr								// Holds a hash character-address pair
{
	char hash_ch;
	struct trie_node *addr;
} char_n_addr;


typedef struct trie_node								// Holds information related to a Trie Node.
{
	node_type type;
	int element_count, dyn_alloc_size;
	union
	{
		char_n_addr *children;
		int *user_id;
	};
} trie_node;


//	====================  Function Prototypes  ====================

static trie_node* create_trie_node(node_type);
trie_t create_trie();
void add_pair(trie_t restrict, const char* restrict, int);
int* retrieve_worker(trie_t restrict, const char* restrict, int* restrict, char* restrict, int);
int* retrieve_users(trie_t restrict, const char* restrict, int* restrict);
void destroy_trie(trie_t restrict);


//	============================  Functions/Operations  ============================

static trie_node* create_trie_node(node_type type)
{
	trie_node* node = malloc(sizeof(trie_node));

	node->type = type;
	node->element_count = 0;
	node->dyn_alloc_size = 0;
	node->children = NULL;

	return node;
}


trie_t create_trie()
{
	return create_trie_node(non_leaf);
}


void add_pair(trie_t restrict root, const char* restrict hash, int user_id)
{
	int i;
	trie_node* next_node = NULL;

	if(*hash == '\0')
	{
		int pos = root->element_count;
		++root->element_count;

		if(root->element_count > root->dyn_alloc_size)
		{
			if(root->dyn_alloc_size != 0)
				root->dyn_alloc_size <<= 1;
			else
			{
				root->dyn_alloc_size = DEFAULT_USERID_ARR_SIZE;
				root->type = leaf;
			}

			root->user_id = realloc(root->user_id, root->dyn_alloc_size * sizeof(int));

			if(root->user_id == NULL)
			{
				fprintf(stderr, "Error during re-allocation in function add_pair() in file trie.h\n");
				exit(EXIT_FAILURE);
			}
		}
		root->user_id[pos] = user_id;
		return;
	}

	for(i = 0; i < root->element_count; ++i)
		if(root->children[i].hash_ch == *hash)
		{
			next_node = root->children[i].addr;
			break;
		}

	if(next_node == NULL)
	{
		int pos = root->element_count;
		++root->element_count;

		if(root->element_count > root->dyn_alloc_size)
		{
			if(root->dyn_alloc_size != 0)
				root->dyn_alloc_size <<= 1;
			else
				root->dyn_alloc_size = DEFAULT_CHILD_ARR_SIZE;

			root->children = realloc(root->children, root->dyn_alloc_size * sizeof(char_n_addr));

			if(root->children == NULL)
			{
				fprintf(stderr, "Error during re-allocation in function add_pair() in file trie.h\n");
				exit(EXIT_FAILURE);
			}
		}

		next_node = create_trie_node(non_leaf);
		root->children[pos].hash_ch = *hash;
		root->children[pos].addr = next_node;
	}

	add_pair(next_node, hash + 1, user_id);
}


/*	Return the base address	of an array containing the list of user IDs (this array is on the heap and will be
**	freed when destroy_trie() is called). The count of user IDs is stored in the argument user_count.
**	NULL is returned in case the hash does not exist in the trie.
*/
int* retrieve_users(trie_t restrict root, const char* restrict hash, int* restrict user_count)
{
	int pos_in_hash = 0, i;
	bool found;

	for(; hash[pos_in_hash] != '\0'; ++pos_in_hash)
	{
		for(i = 0, found = false; i < root->element_count; ++i)
			if(hash[pos_in_hash] == root->children[i].hash_ch)
			{
				found = true;
				break;
			}

		if(found)
			root = root->children[i].addr;
		else
		{
			*user_count = 0;
			return NULL;
		}
	}

	*user_count = root->element_count;
	return root->user_id;
}


void destroy_trie(trie_t restrict root)
{
	if(root->type != leaf)
	{
		for(int i = 0; i < root->element_count; ++i)
			destroy_trie(root->children[i].addr);
		free(root->children);
	}
	else
		free(root->user_id);
	free(root);
	root = NULL;
}

#endif
