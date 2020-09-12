#ifndef TRACKER_H
#define TRACKER_H

#include "torrent.h"
#include "udp.h"
#include "buffer.h"
#include <vector>
#include <string>

class tracker {

private:
	static buffer build_conn_req();
	static buffer build_ann_req(const buffer& b, const torrent& t);

public:
	static buffer get_peers(const torrent& e);
};

#endif // TRACKER_H
