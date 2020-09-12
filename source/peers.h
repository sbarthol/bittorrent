#ifndef PEERS_H
#define PEERS_H

#include "bencode.h"
#include "udp_client.h"
#include "buffer.h"

class peers {

private:
	

public:
	// Todo this is private
	static buffer build_conn_req();
	static buffer get(bencode::item e);
};

#endif // PEERS_H
