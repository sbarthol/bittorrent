#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <netinet/in.h>
#include <vector>
#include <string>
#include "parsing/buffer.h"

class transport {

private:
	struct sockaddr_in servaddr; 
	static const int MAXLINE = 1500;
	char buff[MAXLINE];
	bool closed_flag;

protected:
	transport(std::string address, int port, int type, bool blocking = true);

public:
	void send(buffer message);
	buffer receive();
	int fd;
	void close();
	bool closed();
};

#endif // TRANSPORT_H
