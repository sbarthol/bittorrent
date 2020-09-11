#ifndef UDP_H
#define UDP_H

#include <netinet/in.h>
#include <vector>
#include <string>

class udp_client {

private:
	struct sockaddr_in servaddr; 
	int fd;
	static const int MAXLINE = 1024;
	char buff[MAXLINE];

public:
	udp_client(std::string address, int port);
	void send(std::string message);
	std::string receive();
};

#endif // UDP_H
