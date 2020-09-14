#include "tcp.h"
#include <stdexcept>
#include <sys/errno.h>
#include <string.h>
#include <unistd.h>

void tcp::close() {

	if(::close(fd) < 0) {
		std::string what = strerror(errno);
		throw std::runtime_error(what);
	}
	closed_flag = true;
}

bool tcp::closed() {

	return closed_flag;
}