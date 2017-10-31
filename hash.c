#include "hash.h"

#define SIZELOW 1.1
#define SIZEHIGH 1.7
#define CHARLIMIT 10

void *p_key(void *number)
{
	int i = SIZELOW *(int)number;
	int limit = SIZEHIGH * (int)number;
	for(i;i < limit;i++)
	{
		int j ;
		int i_sqrt = sqrt(i);
		for(j = 0;j < i_sqrt;j++)
		{
			if(i % j != 0)
			{
				return i;
			}
		}
	}
	return number;
}

void *key_hash(void *data,void *n_data,short int checked)
{
	int key = 0;
	if(checked == 1)
	{
		int i_data = (int)data;
		int key_p  = p_key((int)n_data);
		key = i_data % key_p;
		return key;
	}else if(checked == 2)
	{
		char *c_data = (char*)data;
		int i = 0;
		while(i%CHARLIMIT && c_data[i])
		{
			key += c_data[i++];
			key %= (int)n_data;
		}
		return key;
	}else {
	//	fprintf(stderr,"checked error!");
		return key;
	}
}

void *value_hash(void *key,phashNood value,hash_ *hash)
{
	value->next = hash_->values[(int)key]->next;
	hash_->values[(int)key]->next = value->next;
}
