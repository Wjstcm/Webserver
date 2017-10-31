#include <stdio.h>
#include "cgic.h"
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/types.h>

#define MSGSIZE 1024
#define COUNTSIZE 8

#define IOCTL_LED_ON    0
#define IOCTL_LED_OFF   1

void *parse();
void *collection(int i,char *info,char *data);

int cgiMain() 
{
   	//char *method = getenv("REQUEST_METHOD");
   	char *string = getenv("QUERY_STRING");

   	int checked = 0;
   	while(*string)
   	{
   		if(*string == ':' || *string == ';');
   		{
   			checked++;
   		}
   		string++;
   	}

   	if(checked < 6)
   	{
   		fprintf(cgiOut, "4,4,msg,\n");
   	}else if (checked == 6)
   	{
   			parse(string);
   	}else {
   		fprintf(cgiOut, "4,4,msg,\n");
   	}
   	
    return 0;
}

void *parse(char *data)
{
	
	int num = 0;
	int i = 0;

	char *selfcount = (char *)malloc(COUNTSIZE);
	char *sendcount =  (char *)malloc(COUNTSIZE);
	char *sendmsg = (char *)malloc(MSGSIZE);

	for(i ;i < strlen(data);i++)
	{
		if(data[i] == ':')
		{
			if(num == 0)
			{
				i = (int)collection(i,selfcount,data);
			}else if(num == 1)
			{
				i =  (int)collection(i,sendcount,data);
			}else if(num == 2)
			{
				i =  (int)collection(i,sendmsg,data);
			}else {
				break;
			}
			num++;
		}
	}

	fprintf(cgiOut, "%s,%s,%s,\n",sendcount,sendmsg,selfcount);
	free(selfcount);
	free(sendcount);
	free(sendmsg);

}

void *collection(int i,char *info,char *data)
{
	i++;

	while(data[i] != ';')
	{
		char *c = (char *)malloc(sizeof("a"));
		sprintf(c, "%c", data[i++]); 
		strcat(info,c);
		free(c);
	}

	return (void *)i;
}