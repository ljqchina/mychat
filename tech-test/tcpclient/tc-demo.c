#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>

static int bexit = 0;
void signal_handle(int sig)
{
	printf("SIG:%d\n", sig);
	if(sig == SIGINT)
	{
		bexit = 1;
		printf("SIGINT exit\n");
	}
}

int myconnect(const char *ip, int port)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0); 
	if(sock < 0)
	{
		perror("socket");
		return -1;
	}

	struct sockaddr_in addr; 
	addr.sin_family = AF_INET; 
	addr.sin_port = htons(port); 
	addr.sin_addr.s_addr = inet_addr(ip); 

	socklen_t addr_len = sizeof(addr);
	int fd = connect(sock, (struct sockaddr*)&addr, addr_len);
	if(fd < 0)
	{
		perror("connect");
		return -1;
	}
	return sock;
}

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		fprintf(stderr, "usage ./tc ip port\n");
		return 0;
	}

	signal(SIGPIPE, signal_handle);
	signal(SIGINT, signal_handle);

	char ip[32];
	int port;

	strcpy(ip, argv[1]);
	port = atoi(argv[2]);
	if(port <= 0 || port > 65535)
	{
		fprintf(stderr, "invalid port\n");
		return 0;
	}

	int i = 0;
	int sock = 0;
	while((sock = myconnect(ip, port)) < 0)
	{
		printf("reconnectting %d ...\n", ++i);
		sleep(2);
	}

	i = 0;
	char buf[1024];
	while(bexit == 0)
	{
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "{\"msgtype\":%d}", ++i);
		printf("%s\n", buf);
		int len = strlen(buf);
		int ret = send(sock, buf, len, 0);
		if(ret < 0)
		{
			printf("write error\n");
			if(errno == EINTR)
			{
				printf("send EINTR\n");
				continue;
			}
			//break;
		}
		else if(ret == 0)
		{
			printf("close, exit\n");
			//break;
		}

		//reconnect
		if(ret <= 0)
		{
			i = 0;
			close(sock);
			while((sock = myconnect(ip, port)) < 0 && bexit == 0)
			{
				printf("reconnectting %d ...\n", ++i);
				sleep(2);
			}
			printf("reconnect success, sock:%d\n", sock);
			i = 0;
			continue;
		}
		sleep(3);
		if(errno == EINTR)
		{
			printf("sleep EINTR\n");
			continue;
		}
	}
	close(sock);
	printf("end\n");
	return 0;
}

