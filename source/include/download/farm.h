#ifndef FARM_H
#define FARM_H

#include <vector>
#include "download/connection.h"
#include <sys/epoll.h>

class farm {

private:
	int epfd;
	std::vector<connection>& conns;
	download& d;

	static const int MAX_EVENTS = 1024;
	struct epoll_event events[MAX_EVENTS];

public:
	farm(std::vector<connection>& conns, download& d);
	void hatch();
};

#endif // FARM_H
