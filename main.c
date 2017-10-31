#include "hash.h"

int main(int argc,char *argv[])
{
	struct _hash has = (struct _hash)malloc(sizeof(struct _hash));
	has->number = 20;
	has->values = malloc(has->number*2);
	phashNood phash = malloc(sizeof(phashNood));
	phash->data = 100;
	value_hash(key_hash(phash->data,10,1),
			phash,
			&has);
	free(has);
	free(phash);
	return 0;
}
