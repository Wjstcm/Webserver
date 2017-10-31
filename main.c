#include "server.h"
#include "threadpool.h"

int main(int argc, char *argv[])
{
	int sockfd = 0;				
	int connfd = 0;
	int err_log = 0;
	struct sockaddr_in my_addr;	
	unsigned short port = 5210;
	pthread_t thread_id;

	char recv_buf[10] = "";	

	system("clear");

	printf("\n\t\t\t Successful startup Msg Server \n");
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);  
	if(sockfd < 0)
	{
		perror("socket error");
		exit(-1);
	}

	bzero(&my_addr, sizeof(my_addr));	   
	my_addr.sin_family = AF_INET;
	my_addr.sin_port   = htons(port);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	err_log = bind(sockfd, (struct sockaddr*)&my_addr, sizeof(my_addr));
	if(err_log != 0)
	{
		perror("bind");
		close(sockfd);		
		exit(-1);
	}

	err_log = listen(sockfd, 10);
	if( err_log != 0)
	{
		perror("listen");
		close(sockfd);		
		exit(-1);
	}

	pool_init(10);

	while(1)
	{

		char cli_ip[INET_ADDRSTRLEN] = "";	  
		struct sockaddr_in client_addr;		  
		socklen_t cliaddr_len = sizeof(client_addr);   

		connfd = accept(sockfd, (struct sockaddr*)&client_addr, &cliaddr_len);  
		if(connfd < 0)
		{
			perror("accept this time");
			continue;
		}

		if(recv(connfd, recv_buf, 10, 0) > 0)
		{
			if(recv_buf[0] == 'G' && recv_buf[5] == ' ')
			{
				if(connfd > 0)
				{
					pool_add_worker (client_fun, (void *)connfd);  
				}
			}else {
				//close(connfd);
			}
		}else {
			//close(connfd);
		}
	}

	close(sockfd);
	pool_destroy ();  
	return 0;
}
