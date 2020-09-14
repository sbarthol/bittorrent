#ifndef TCP_H
#define TCP_H

#include "transport.h"
#include <sys/types.h>

class tcp: public transport {

private:
	bool closed;

public:
	tcp(std::string address, int port): 
		transport(address, port, SOCK_STREAM), closed(false) {}
	void close();
};

#endif // TCP_H
