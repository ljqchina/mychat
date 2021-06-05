#ifndef __TCPSERVER_H__

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>
#include <event2/event_struct.h>

#include <map>
#include "common.h"

typedef void (*readcb_t)(struct bufferevent *bev, void *arg);
typedef void (*closecb_t)(struct bufferevent *bev, void *arg);

class TcpServer{
public:
	TcpServer(struct event_base *pbase);
	~TcpServer();
	int Start(uint16_t port);
	void Stop();
	void SetReadcb(readcb_t cb, void *arg);
	void SetClosecb(closecb_t cb, void *arg);
//private:
	void *readcb_arg;
	readcb_t readcb;
	void *closecb_arg;
	closecb_t closecb;
	struct event_base *m_pBase;
	struct evconnlistener *m_pListener;
};

#endif //__TCPSERVER_H__

