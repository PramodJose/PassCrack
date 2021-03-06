#ifndef _ABSTRACT_TYPES_H
#define _ABSTRACT_TYPES_H 1

#define	UNAME_LEN	32
#define	NAME_LEN	64
#define	HASH_LEN	32


//	==========================  Structs/Types  ==========================

typedef struct
{
	char user_name[UNAME_LEN], full_name[NAME_LEN], hash[HASH_LEN];
} user_info_t;

char algo_n_salt[UNAME_LEN + NAME_LEN + HASH_LEN];

#endif
