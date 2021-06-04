#ifndef CONN_H_
#define CONN_H_

#include <event2/event_struct.h>

typedef struct Conn_ {
    int userid;
    struct bufferevent *bev;    
}Conn;

#endif //CONN_H_

