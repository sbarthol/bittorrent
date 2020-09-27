#ifndef TCP_H
#define TCP_H

#include "tracker/transport.h"
#include <sys/types.h>

class tcp: public transport {

public:
	tcp(std::string address, int port, bool blocking = true): 
		transport(address, port, SOCK_STREAM, blocking) {}
};

#endif // TCP_H
