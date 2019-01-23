#ifndef _ABSTRACT_TYPES_H
#define _ABSTRACT_TYPES_H 1

#define	UNAME_LEN	32
#define	NAME_LEN	64
#define	HASH_LEN	32


//	==========================  Structs  ==========================

typedef struct
{
	char user_name[UNAME_LEN], full_name[NAME_LEN], hash[HASH_LEN];
}user_info_t;

#endif
