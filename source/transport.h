#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <netinet/in.h>
#include <vector>
#include <string>
#include "buffer.h"

class transport {

private:
	struct sockaddr_in servaddr; 
	static const int MAXLINE = 1500;
	char buff[MAXLINE];

protected:
	transport(std::string address, int port, int type);
	int fd;

public:
	void send(buffer message);
	buffer receive();
};

#endif // TRANSPORT_H
