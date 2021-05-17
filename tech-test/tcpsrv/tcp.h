#ifndef __TCPSERVER_H__

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>

typedef void (*readcb_t)(struct bufferevent *bev, void *arg);

class tcp_server {
public:
	tcp_server(struct event_base *pbase);
	~tcp_server(); 
	int start(uint16_t port);
	void stop();
	void setReadcb(readcb_t cb, void *arg);
//private:
	void *readcb_arg;
	readcb_t readcb;
	struct event_base *base;
	struct bufferevent *bev;
	struct evconnlistener *listener;
};

#endif //__TCPSERVER_H__
