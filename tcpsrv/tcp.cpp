/*
  This example program provides a trivial server program that listens for TCP connections on port 9995.  When they arrive, it writes a short message to
  each client connection, and closes each connection once it is flushed.

  Where possible, it exits cleanly in response to a SIGINT (ctrl-c).
*/


#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "tcp.h"

static void conn_readcb(struct bufferevent *bev, void *user_data)
{
	TcpServer *ts = (TcpServer*)user_data;
	//fprintf(stderr, "conn_readcb: ts:%p, cb:%p\n", ts, ts->readcb);
	if (ts && ts->readcb)
		ts->readcb(bev, ts->readcb_arg);
}

static void conn_writecb(struct bufferevent *bev, void *user_data)
{
	return;

	struct evbuffer *output = bufferevent_get_output(bev);
	if (evbuffer_get_length(output) == 0) {
		printf("flushed answer\n");
		bufferevent_free(bev);
	}
}

static void conn_eventcb(struct bufferevent *bev, short events, void *user_data)
{
	printf("conn_eventcb event:0x%02x\n", events);
	TcpServer *ts = (TcpServer *)user_data;
	if (events & BEV_EVENT_EOF) {
		printf("Connection closed.\n");
	} else if (events & BEV_EVENT_ERROR) {
		printf("Got an error on the connection: %s\n",
		    strerror(errno));/*XXX win32*/
	}
	/* None of the other events can happen here, since we haven't enabled
	 * timeouts */
	if (events != BEV_EVENT_CONNECTED) {
		bufferevent_free(bev);
		if (ts && ts->closecb)
			ts->closecb(bev, ts->closecb_arg);
	}
}

static void listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sa, int socklen, void *user_data)
{
	printf("listener:%p, fd:%d\n", user_data, fd);
	TcpServer *ts = static_cast<struct TcpServer *>(user_data);

	struct bufferevent *pBev = bufferevent_socket_new(ts->m_pBase, fd, BEV_OPT_CLOSE_ON_FREE);
	if (!pBev) {
		fprintf(stderr, "Error constructing bufferevent!");
		event_base_loopbreak(ts->m_pBase);
		return;
	}
	bufferevent_setcb(pBev, conn_readcb, conn_writecb, conn_eventcb, user_data);
	bufferevent_enable(pBev, EV_WRITE);
	bufferevent_enable(pBev, EV_READ);

	char str[100];
	for(int i=4; i<100; i++)
		strcat(str+i, "a");
	memcpy(str, "0100", 4);
	bufferevent_write(pBev, str, strlen(str));
}

TcpServer::TcpServer(struct event_base *pbase)
	: m_pBase(pbase)
	, m_pListener(nullptr)
	, readcb_arg(nullptr)
	, readcb(nullptr)
{
}

TcpServer::~TcpServer()
{
	Stop();
}

int TcpServer::Start(uint16_t port)
{
	struct sockaddr_in sin = {0};

	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	m_pListener = evconnlistener_new_bind(m_pBase, listener_cb, (void *)this, LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1, (struct sockaddr*)&sin, sizeof(sin));
	if (!m_pListener) {
		fprintf(stderr, "Could not create a listener!\n");
		return 1;
	}

	return 0;
}

void TcpServer::Stop()
{
	if (m_pListener)
		evconnlistener_free(m_pListener);

	printf("TcpServer stop done\n");
}

void TcpServer::SetReadcb(readcb_t cb, void *arg)
{
	readcb = cb;
	readcb_arg = arg;
}

void TcpServer::SetClosecb(closecb_t cb, void *arg)
{
	closecb = cb;
	closecb_arg = arg;
}
