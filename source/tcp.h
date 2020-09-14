#ifndef TCP_H
#define TCP_H

#include "transport.h"
#include <sys/types.h>

class tcp: public transport {

private:
	bool closed_flag;

public:
	tcp(std::string address, int port): 
		transport(address, port, SOCK_STREAM), closed_flag(false) {}
	void close();
	bool closed();
};

#endif // TCP_H
