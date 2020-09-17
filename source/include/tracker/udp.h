#ifndef UDP_H
#define UDP_H

#include "tracker/transport.h"
#include <sys/types.h>

class udp: public transport {

public:
	udp(std::string address, int port): transport(address, port, SOCK_DGRAM) {}
};

#endif // UDP_H
