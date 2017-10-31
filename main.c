#include "tcp.h"

int main()
{
    int ser_fd = passive_server(5210,20);

    struct sockaddr_in client_addr;
    socklen_t addr_length = sizeof(client_addr);
    int conn = accept(ser_fd,(struct sockaddr*)&client_addr, &addr_length);

    shakehands(conn);

    int count = 10;
    while (count--)
    {
        frame_head head;
        int rul = recv_frame_head(conn,&head);
        if (rul < 0)
            break;
        printf("fin=%d\nopcode=0x%X\nmask=%d\npayload_len=%llu\n",head.fin,head.opcode,head.mask,head.payload_length);

        send_frame_head(conn,&head);
      
        char payload_data[1024] = {0};
        int size = 0;
        do {
            int rul;
            rul = read(conn,payload_data,1024);
            if (rul<=0)
                break;
            size+=rul;

            umask(payload_data,size,head.masking_key);
            printf("recive:%s",payload_data);

            if (write(conn,payload_data,rul)<=0)
                break;
        }while(size<head.payload_length);
        printf("\n-----------\n");

    }

    close(conn);
    close(ser_fd);
}
