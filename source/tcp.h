#ifndef TCP_H
#define TCP_H

#include "transport.h"
#include <sys/types.h>

class tcp: public transport {

public:
	tcp(std::string address, int port): transport(address, port, SOCK_STREAM) {}
};

#endif // TCP_H
