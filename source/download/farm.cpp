#include "download/farm.h"
#include <stdexcept>
#include <unistd.h>
#include <string.h>
#include <iostream>

using namespace std;

farm::farm(vector<connection>& conns, download& d): conns(conns), d(d) {

	epfd = epoll_create(1);
	if (epfd < 0) {

		string what = strerror(errno);
		throw runtime_error(what);
	}
}

void farm::hatch() {

	for(int i=0;i<conns.size();i++) {

		struct epoll_event ev;
		ev.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLPRI | EPOLLERR | EPOLLHUP;
		ev.data.u32 = i;
		int res = epoll_ctl(epfd, EPOLL_CTL_ADD, conns[i].socket.fd, &ev);

		if (res < 0) {

			string what = strerror(errno);
			throw runtime_error(what);
		}
	}

	while(!d.is_done()) {

		int nfd = epoll_wait(epfd, events, MAX_EVENTS, -1);
		if (nfd < 0) {

			string what = strerror(errno);
			throw runtime_error(what);
		}

		for(int i=0;i<nfd;i++) {

			int mask = events[i].events;
			int idx = events[i].data.u32;

			if(mask & (EPOLLRDHUP | EPOLLPRI | EPOLLERR | EPOLLHUP)) {

				conns[idx].socket.close();

				// Todo retry to connect the socket here

			} else if (mask & (EPOLLOUT | EPOLLIN)) {

				// Todo put this in a worker thread
				try {
					conns[idx].ready();
				} catch (exception& e) {
					cout<<"ready() threw: "<<e.what()<<endl;
				}

				if (mask & EPOLLOUT) {

					struct epoll_event ev;
					ev.events = EPOLLIN | EPOLLRDHUP | EPOLLPRI | EPOLLERR | EPOLLHUP;
					ev.data.u32 = idx;

					int res = epoll_ctl(epfd, EPOLL_CTL_MOD, conns[idx].socket.fd, &ev);
					if (res < 0) {

						string what = strerror(errno);
						throw runtime_error(what);
					}
				}

			} else {

				throw runtime_error("unknown ready event in mask");
			}
		}
	}

	if (::close(epfd) < 0) {

		string what = strerror(errno);
		throw runtime_error(what);
	}
}
