#ifndef TRACKER_H
#define TRACKER_H

#include "torrent.h"
#include "udp.h"
#include "buffer.h"
#include <vector>
#include <string>
#include "http.h"

struct peer;

class tracker {

private:
	static buffer build_conn_req_udp();
	static buffer build_ann_req_udp(const buffer& b, const torrent& t);
	static void build_ann_req_http(http& request, const torrent& t);
	static std::vector<peer> get_peers(const buffer& b);

public:
	static std::vector<peer> get_peers(const torrent& e);
};

struct peer {

	std::string host;
	int port;

	peer(std::string host, int port): host(host), port(port) {};
	void download();
};

#endif // TRACKER_H
