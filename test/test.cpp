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
#include <string>

#include "Protocol.h"

using namespace std;

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

//注册消息
void register_user(int sock)
{
    string msg;
    Protocol pt;
    RegisterInfo ri;

    ri.version = "1.0.0.0";
    ri.msgType = 2001;
    ri.userId = "13000000001";
    ri.nickName = "abc";
    ri.password = "123";
    ri.sex = 1;
    ri.age = 20;

    pt.PackRegisterReq(ri, msg);
    send(sock, msg.data(), msg.size(), 0);

    fprintf(stderr, "%s:%s\n", __func__, msg.data());
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

    //1.注册用户
    register_user(sock);
    sleep(3);
    register_user(sock);

	close(sock);
	printf("end\n");
	return 0;
}
