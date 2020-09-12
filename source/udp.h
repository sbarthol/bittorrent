#ifndef UDP_H
#define UDP_H

#include <netinet/in.h>
#include <vector>
#include <string>
#include "buffer.h"

class udp {

private:
	struct sockaddr_in servaddr; 
	int fd;
	static const int MAXLINE = 1024;
	char buff[MAXLINE];

public:
	udp(std::string address, int port);
	void send(buffer message);
	buffer receive();
};

#endif // UDP_H
