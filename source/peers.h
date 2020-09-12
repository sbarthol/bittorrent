#ifndef PEERS_H
#define PEERS_H

#include "bencode.h"
#include "udp.h"
#include "buffer.h"
#include <vector>
#include <string>

class peers {

private:
	static buffer build_conn_req();
	static buffer build_ann_req(const buffer& b);

public:
	static std::vector<std::string> get(bencode::item e);
};

#endif // PEERS_H
