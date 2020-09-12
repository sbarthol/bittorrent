#include "udp_client.h"
#include <sys/socket.h>
#include <stdio.h>
#include <stdexcept>
#include <sys/socket.h>
#include <vector>
#include <sys/errno.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>

using namespace std;

udp_client::udp_client(string address, int port) {

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { 

		string what = strerror(errno);
		throw runtime_error(what);
	} 

	memset(&servaddr, 0, sizeof(servaddr)); 

	struct hostent *server;
	server = gethostbyname(address.c_str());
	if (server == NULL) {
	    
		throw runtime_error("host not found");
	}
	      
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(port); 
	servaddr.sin_addr = *((struct in_addr *)server->h_addr);

	if (connect(fd, (struct sockaddr*)&servaddr,sizeof(servaddr)) < 0) {

		string what = strerror(errno);
		throw runtime_error(what);
	}
}

void udp_client::send(buffer message) {

	ssize_t n = ::send(fd, message.data(), message.size(), 0);

	if(n < 0) {
		string what = strerror(errno);
		throw runtime_error(what);
	}
}

buffer udp_client::receive() {

	ssize_t n = recv(fd, buff, MAXLINE, 0);

	if(n < 0) {
		string what = strerror(errno);
		throw runtime_error(what);
	}
	
	return buffer(buff,buff+n);
}
