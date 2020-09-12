#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include <netinet/in.h>
#include <vector>
#include <string>
#include "buffer.h"

class udp_client {

private:
	struct sockaddr_in servaddr; 
	int fd;
	static const int MAXLINE = 1024;
	char buff[MAXLINE];

public:
	udp_client(std::string address, int port);
	void send(buffer message);
	buffer receive();
};

#endif // UDP_CLIENT_H
