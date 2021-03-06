#include "tcp.h"

int passive_server(int port,int queue)
{
    int server_sockfd = socket(AF_INET,SOCK_STREAM, 0);

    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(port);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);


    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1)
    {
        perror("bind");
        exit(1);
    }
  
    if(listen(server_sockfd,queue) == -1)
    {
        perror("listen");
        exit(1);
    }
    printf("port %\n",port);
    return server_sockfd;
}

int base64_encode(char *in_str, int in_len, char *out_str)
{
    BIO *b64, *bio;
    BUF_MEM *bptr = NULL;
    size_t size = 0;

    if (in_str == NULL || out_str == NULL)
        return -1;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, in_str, in_len);
    BIO_flush(bio);

    BIO_get_mem_ptr(bio, &bptr);
    memcpy(out_str, bptr->data, bptr->length);
    out_str[bptr->length-1] = '\0';
    size = bptr->length;

    BIO_free_all(bio);
    return size;
}

int _readline(char* allbuf,int level,char* linebuf)
{
    int len = strlen(allbuf);
    for (;level<len;++level)
    {
        if(allbuf[level]=='\r' && allbuf[level+1]=='\n')
            return level+2;
        else
            *(linebuf++) = allbuf[level];
    }
    return -1;
}

int shakehands(int cli_fd)
{
    int level = 0;
   
    char buffer[BUFFER_SIZE];
   
    char linebuf[256];
 
    char sec_accept[32];
 
    unsigned char sha1_data[SHA_DIGEST_LENGTH+1]={0};
  
    char head[BUFFER_SIZE] = {0};

    if (read(cli_fd,buffer,sizeof(buffer))<=0)
        perror("read");
    printf("request\n");
    printf("%s\n",buffer);

    do {
        memset(linebuf,0,sizeof(linebuf));
        level = _readline(buffer,level,linebuf);

        if (strstr(linebuf,"Sec-WebSocket-Key")!=NULL)
        {
            strcat(linebuf,GUID);
           
            SHA1((unsigned char*)&linebuf+19,strlen(linebuf+19),(unsigned char*)&sha1_data);
          
            base64_encode(sha1_data,strlen(sha1_data),sec_accept);

            sprintf(head, "HTTP/1.1 101 Switching Protocols\r\n" \
                          "Upgrade: websocket\r\n" \
                          "Connection: Upgrade\r\n" \
                          "Sec-WebSocket-Accept: %s\r\n" \
                          "\r\n",sec_accept);

            printf("response\n");
            printf("%s",head);
            if (write(cli_fd,head,strlen(head))<0)
                perror("write");

            break;
        }
    }while((buffer[level]!='\r' || buffer[level+1]!='\n') && level!=-1);
    return 0;
}

int recv_frame_head(int fd,frame_head* head)
{
    char one_char;
  
    if (read(fd,&one_char,1)<=0)
    {
        perror("read fin");
        return -1;
    }
    head->fin = (one_char & 0x80) == 0x80;
    head->opcode = one_char & 0x0F;
    if (read(fd,&one_char,1)<=0)
    {
        perror("read mask");
        return -1;
    }

    head->mask = (one_char & 0x80) == 0X80;

    head->payload_length = one_char & 0x7F;

    if (head->payload_length == 126)
    {
        char extern_len[2];
        if (read(fd,extern_len,2)<=0)
        {
            perror("read extern_len");
            return -1;
        }
        head->payload_length = (extern_len[0]&0xFF) << 8 | (extern_len[1]&0xFF);
    }
    else if (head->payload_length == 127)
    {
        char extern_len[8],temp;
        int i;
        if (read(fd,extern_len,8)<=0)
        {
            perror("read extern_len");
            return -1;
        }
        for(i=0;i<4;i++)
        {
            temp = extern_len[i];
            extern_len[i] = extern_len[7-i];
            extern_len[7-i] = temp;
        }
        memcpy(&(head->payload_length),extern_len,8);
    }

    if (read(fd,head->masking_key,4)<=0)
    {
        perror("read masking-key");
        return -1;
    }

    return 0;
}

void umask(char *data,int len,char *mask)
{
    int i;
    for (i=0;i<len;++i)
        *(data+i) ^= *(mask+(i%4));
}

int send_frame_head(int fd,frame_head* head)
{
    char *response_head;
    int head_length = 0;
    if(head->payload_length<126)
    {
        response_head = (char*)malloc(2);
        response_head[0] = 0x81;
        response_head[1] = head->payload_length;
        head_length = 2;
    }
    else if (head->payload_length<0xFFFF)
    {
        response_head = (char*)malloc(4);
        response_head[0] = 0x81;
        response_head[1] = 126;
        response_head[2] = (head->payload_length >> 8 & 0xFF);
        response_head[3] = (head->payload_length & 0xFF);
        head_length = 4;
    }
    else
    {

        response_head = (char*)malloc(12);

        head_length = 12;
    }

    if(write(fd,response_head,head_length)<=0)
    {
        perror("write head");
        return -1;
    }

    free(response_head);
    return 0;
}


