#ifndef _SERVER_H_
#define _SERVER_H_ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>				
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>					
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/types.h>

#define BUFSIZE  1024
#define FILESIZE 4096
#define LOWN 1

#define TRUE 1
#define FALSE 0

#define CHARTOINT 10
#define MAXDATASIZE 1024

int CYOIS(int x);
void *sublime_post(char *buf);
char* cmd_system(const char* command);
void *sublime_cgi(char *buf,char *data);
int getFileSize(char str[]);
void *ergmsg(char *str_con);
void *httphead(char strs[]);
void *httpcontent(char str[]);
void *client_fun(void *arg);
void *msgs( char *result,int checked);
int len_str(char *s);

#endif