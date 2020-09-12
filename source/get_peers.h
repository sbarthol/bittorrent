#ifndef GET_PEERS_H
#define GET_PEERS_H

#include "bencode.h"
#include "udp_client.h"
#include "buffer.h"

class get_peers {

private:
	

public:
	static buffer build_conn_req();
	static buffer get(bencode::item e);
};

#endif // GET_PEERS_H
