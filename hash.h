#ifndef _HASH_H_
#define _HASH_H_

#include <math.h>
#include <stdlib.h>

typedef struct Hash_Nood
{
	void* data;
	struct Hash_Nood *next;
}hashNood,*phashNood;

typedef struct _hash
{
	void* number;
	phashNood *values;
}hash_;

void *p_key(void *number);
void *key_hash(void *data,void *n_data,short int checked);
void *value_hash(void *key,phashNood value,hash_ *hash);

#endif
