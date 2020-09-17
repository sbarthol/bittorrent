#ifndef MESSAGE_H
#define MESSAGE_H

#include "parsing/torrent.h"
#include "parsing/buffer.h"

class message {

public:
	message() = delete;

	static buffer build_handshake(const torrent& t);
	static buffer build_keep_alive();
	static buffer build_choke();
	static buffer build_unchoke();
	static buffer build_interested();
	static buffer build_not_interested();
	static buffer build_have(const buffer& payload);
	static buffer build_bitfield(const buffer& bitfield);
	static buffer build_request(unsigned int index, 
				unsigned int begin, unsigned int length);
	static buffer build_piece(unsigned int index, 
				unsigned int begin, const buffer& block);
	static buffer build_cancel(unsigned int index, 
				unsigned int begin, unsigned int length);
	static buffer build_port(unsigned int port);
};

#endif // MESSAGE_H
