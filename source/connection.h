#ifndef CONNECTION_H
#define CONNECTION_H

#include "torrent.h"
#include "tracker.h"
#include "buffer.h"
#include "tcp.h"
#include <vector>
#include <queue>

class connection {

private:
	buffer get_message(tcp& client);

	buffer build_handshake(const torrent& t);
	buffer build_keep_alive();
	buffer build_choke();
	buffer build_unchoke();
	buffer build_interested();
	buffer build_not_interested();
	buffer build_have(const buffer& payload);
	buffer build_bitfield(const buffer& bitfield);
	buffer build_request(unsigned int index, 
				unsigned int begin, unsigned int length);
	buffer build_piece(unsigned int index, 
				unsigned int begin, const buffer& block);
	buffer build_cancel(unsigned int index, 
				unsigned int begin, unsigned int length);
	buffer build_port(unsigned int port);

	void choke_handler();
	void unchoke_handler();
	void have_handler(buffer& b, tcp& socket);
	void bitfield_handler(buffer& b, tcp& socket);
	void piece_handler(tcp& socket);

	void request_piece(tcp& socket);

	buffer buff;
	const peer& p;
	const torrent& t;
	bool handshake;
	std::queue<long long>q;
	std::vector<bool>& requested;

public:
	connection(const peer& p, const torrent& t, std::vector<bool>& requested);
	void download();
};

#endif // CONNECTION_H
