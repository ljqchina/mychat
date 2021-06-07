#include <iostream>
#include <string>
#include <signal.h>
#include "MyChat.h"
#include "db.h"

using namespace std;

static void signal_cb(evutil_socket_t sig, short events, void *user_data)
{
	struct event_base *base = (struct event_base *)user_data;
	struct timeval delay = { 2, 0 };

	printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");

	event_base_loopexit(base, &delay);
}

int main()
{
	struct event_base *base;
	struct event *signal_event;

	base = event_base_new();

	signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)base);
	event_add(signal_event, nullptr);

	const char *dbfile = "/home/ljq/data/mychat.db";
	int ret = db::open(dbfile);
	printf("db::open ret:%d\n", ret);

	//work relation
	MyChat mc(base);
	mc.Init();
	mc.Start();

	event_base_dispatch(base);

	event_free(signal_event);
	event_base_free(base);

	ret = db::close();
	printf("db::close ret:%d\n", ret);
	return 0;
}

