#include <iostream>
#include <string>
#include <signal.h>
#include "tcp.h"

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

	tcp_server *ts = new tcp_server(base);

	ts->start(4567);

	signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)base);
	event_add(signal_event, nullptr);
	
	event_base_dispatch(base);

	delete ts;
	event_free(signal_event);
	event_base_free(base);

	return 0;
}

