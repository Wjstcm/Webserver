#include "server.h"

char* cmd_system(const char* command)
{
	static char* result = "";
	FILE *fpRead;
	fpRead = popen(command, "r");
	char cgi_buf[MAXDATASIZE/LOWN];
	memset(cgi_buf,0,MAXDATASIZE/LOWN);
	char buf[128];
	memset(buf,'\0',sizeof(buf));
	
	while(fgets(buf,128-1,fpRead)!=NULL)
	{
		strcat(cgi_buf,buf);
	}

	result=cgi_buf;
	//if(fpRead != NULL)
	//	pclose(fpRead);
	return result;
}

void *sublime_post(char *buf)
{
	setenv("REQUEST_METHOD","POST",1);
	char *data = malloc(BUFSIZE/LOWN);
	memset(data,0,BUFSIZE/LOWN);
	int i = 0,j = 0;
	int flag_k = FALSE;
	int flag_v = FALSE;
	int flag = FALSE;

	for(i = 0,j = 0;i < strlen(buf);i++)
	{
		if(buf[i-3] == '\r')
		{
			if(buf[i-1] == '\r')	
			{
				flag = TRUE;
				continue;		
			}
		}

		if(flag)
		{		
			if(buf[i] == '=')
			{
				data[j++] = ':';
				flag_v = TRUE;
				flag_k = FALSE;
				continue;
			}

			if(buf[i] == '&')
			{
				data[j++] = ';';
				flag_v = FALSE;
				flag_k = TRUE;
				continue;
			}

			if(buf[i] == '\r' || i == strlen(buf)-1)
			{
				data[j++] = buf[i];	
				data[j++] = ';';
				flag_k = FALSE;
				flag_v = FALSE;
				setenv("QUERY_STRING", data, 1);
				free(data);
				return (void *)1;
			}
			data[j++] = buf[i];	
		}
	}
	return (void *)1;
}

void *sublime_cgi(char *buf,char *data)
{
	int i = 0,j =0;
	int flag = FALSE;

	for(i = 2;i < strlen(buf);i++)
	{
		if(buf[i-2] == ' ' && buf[i-1] == '/' && buf[i] != ' ')
		{
			flag = TRUE;
		}
		if(flag)
		{
			data[j++] = buf[i];
			if(buf[i] == '.')
			{
				if(buf[i+1] != 'c' && buf[i+2] != 'g' &&buf[i+3] != 'i')
				{
					memset(data,0,strlen(data));
					break;
				}
			}
		}
		if(flag && buf[i] == ' ')
		{
			break;			
		}
	}
	return (void *)1;
}

int getFileSize(char str[])  
{  
    FILE *fp = fopen(str, "rb");  
    fseek(fp, 0, SEEK_END);  
    int size =  ftell(fp);  
    fclose(fp);  
    return size;  
} 

void *ergmsg(char *str_con)
{
	char *strs = (char *)malloc(BUFSIZE);
	char *str_len = malloc(BUFSIZE/LOWN); 
	memset(str_len,0,BUFSIZE/LOWN);
	int len; 

    len = len_str(str_con);
    sprintf(str_len, "%d", len); 
   	
    memset(strs, 0, BUFSIZE); 
    strcat(strs, "HTTP/1.1 200 OK\r\n");
	//strcat(strs, "Connection:Keep-Alive\n"); 
	strcat(strs, "Content-Type:text/html\r\n"); 
    strcat(strs, "Content-Length:"); 	
    strcat(strs, str_len); 
    strcat(strs, "\r\n\r\n"); 

    //free(str_len);

	return strs;
}

void *httphead(char strs[])
{
	char *str = (char *)malloc(BUFSIZE);
	memset(str,0,BUFSIZE);
	char *str_len; 
	socklen_t len; 

    str_len=(char*)malloc(128); 
    memset(str_len,0,128);
    len = getFileSize(strs);
    sprintf(str_len, "%d", len); 

    memset(str, 0, BUFSIZE); 
    strcat(str, "HTTP/1.1 200 OK\r\n");
	strcat(str, "Content-Type:text/html;charset=utf-8\r\n"); 
    strcat(str, "Content-Length:"); 	
    strcat(str, str_len); 
    strcat(str, "\n\n"); 

    free(str_len);

	return str;
}

void *httpcontent(char str[])
{
	int size = getFileSize(str);
	if(FILESIZE < size)
	{
		printf("\tFILESIZE < SIZE\n");
		exit(1);
		return 0;
	}

	char *pfile = (char *)malloc(FILESIZE);

	FILE *fp = fopen(str, "rb");
	fseek(fp,0L,SEEK_SET);
	fread(pfile,size,1,fp);
	fclose(fp);
	return pfile;
}

void *msgs( char *result,int checked)
{
	static char *msg = "";
	int j = 0,i,check = 0;
	char msgsg[BUFSIZE] = {0};

	for(i = 0;i < len_str(result);i++)
	{
		printf("%c\n",result[i]);
		if(result[i] == ',')
		{
			check++;
			j = 0;
			if(checked == check)	
			{
				msg = msgsg;
				printf("msgs %s\n", msgsg);
				printf("msg %s\n", msg);
				return msg;
			}
			memset(msgsg,0,BUFSIZE);
			continue;	
		}
		msgsg[j++] = result[i];
	}
}

int len_str(char *str)
{
    int i = 0;      
    while ( str[i++] != '\0')
        ;
    return i;
}

int CYOIS(int x)
{
	int CTOIS = 1;
	while(--x)
	{
		CTOIS *= CHARTOINT;
	}
	return CTOIS;
}

void *client_fun(void *arg)
{

	int recv_len = 0;
	char recv_buf[BUFSIZE] = "";	
	memset(recv_buf,0,BUFSIZE);
	int connfd = (int)arg; 

	printf("\t\t Client  %d \n",connfd);

	char *head = httphead("index.html");

	char *connect =  httpcontent("index.html");
    strcat(connect,"\r\n\r\n"); 

	char *data = (char *)malloc(BUFSIZE/LOWN);
	memset(data,0,BUFSIZE/LOWN);

	while(1)
	{

		START:memset(recv_buf,0,BUFSIZE);
		if((recv_len = recv(connfd, recv_buf, sizeof(recv_buf), 0)) > 0)
		{
			//isexit = 0;
			
			memset(data,0,BUFSIZE);
			sublime_cgi(recv_buf,data);

			if(strlen(data) < 2)
			{
				send(connfd, head, strlen(head), 0);
				send(connfd, connect, strlen(connect), 0); 
			}else {
				
				sublime_post(recv_buf);

				char *result = cmd_system(data);

				char *scount = (char *)malloc(BUFSIZE/LOWN/LOWN);
				char *samsg = (char *)malloc(BUFSIZE);
				char *fcount = (char *)malloc(BUFSIZE/LOWN/LOWN);

				int j = 0,i,check = 0;
				char *tranmsg = (char *)malloc(BUFSIZE);
				memset(tranmsg,'\0',BUFSIZE);

				char *fromsendmsg = (char *)malloc(BUFSIZE + LOWN);
				memset(fromsendmsg,0,BUFSIZE + LOWN);

				int sdConnfd = 0;

				for(i = 0;i < len_str(result);i++)
				{
					if(result[i] == ',')
					{
						check++;
						if(check == 1)
						{
							scount = tranmsg;
							int CInum = len_str(scount)-1;
							while(*scount)
						    {
						        sdConnfd += (*scount - 0x30) * CYOIS(CInum--);
						        scount++;
						    }		
						    if(sdConnfd == 0)
						    {
						    	printf("0 START\n");
						    	goto START;
						    }				
						}else if(check == 2)
						{
							samsg = tranmsg;
							strncat(fromsendmsg,samsg,len_str(samsg));
							strcat(fromsendmsg,":");
						}else if(check == 3)
						{
							fcount = tranmsg;
							strncat(fromsendmsg,fcount,len_str(fcount));
							strcat(fromsendmsg,"\r\n");
						}
						j = 0;
						memset(tranmsg,'\0',BUFSIZE);					
						continue;
					}
					tranmsg[j++] = result[i];
				}

				printf("%s\n", fromsendmsg);

				printf("sdConnfd %d\n", sdConnfd);
				char *erg = ergmsg(fromsendmsg);
				send(sdConnfd, erg, len_str(erg), 0); 
				if ( send(sdConnfd, fromsendmsg, len_str(fromsendmsg), 0) > 0)
				{
					char *ergok = ergmsg("OK");
					send(connfd, ergok, len_str(ergok), 0); 
					send(sdConnfd, "OK", len_str("OK"), 0);
					free(ergok);
				}else {
					char *ergerr = ergmsg("ERROR");
					send(connfd, ergerr, len_str(ergerr), 0); 
					send(sdConnfd, "ERROR", len_str("ERROR"), 0);
					free(ergerr);
				}

				free(erg);
				free(tranmsg);
				free(fromsendmsg);
			}
		}else {
			// isexit++;
			// if(isexit > 50000)
			// {
			// 	break;
			// }
		}
	}

	free(connect);
	free(head);
	free(data);
	close(connfd);	
	printf("close\n");
	return 	NULL;
}