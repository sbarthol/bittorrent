#include "tracker/transport.h"
#include <stdio.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h> 
#include <vector>
#include <sys/errno.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

using namespace std;

transport::transport(string address, int port, int type, bool blocking) {

	fd = socket(AF_INET, type | (blocking ? 0 : SOCK_NONBLOCK), 0);
	if (fd < 0) { 

		string what = strerror(errno);
		throw runtime_error(what);
	} 

	bzero(&servaddr, sizeof(servaddr)); 

	struct hostent *server;
	server = gethostbyname(address.c_str());
	if (server == NULL) {
	    
		throw runtime_error("host not found");
	}
	      
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(port); 
	servaddr.sin_addr = *((struct in_addr *)server->h_addr);

	if (connect(fd, (struct sockaddr*)&servaddr,sizeof(servaddr)) < 0) {

		if (errno != EINPROGRESS) {
			string what = strerror(errno);
			throw runtime_error(what);
		}
	}
}

void transport::send(buffer message) {

	ssize_t n = ::send(fd, message.data(), message.size(), 0);

	if(n < 0) {
		string what = strerror(errno);
		throw runtime_error(what);
	}
}

buffer transport::receive() {

	ssize_t n = recv(fd, buff, MAXLINE, 0);

	if(n < 0) {
		string what = strerror(errno);
		throw runtime_error(what);
	}
	
	return buffer(buff,buff+n);
}

void transport::close() {

	if(closed_flag){
		return;
	}

	if(::close(fd) < 0) {
		std::string what = strerror(errno);
		throw std::runtime_error(what);
	}
	closed_flag = true;
}

bool transport::closed() {

	return closed_flag;
}
