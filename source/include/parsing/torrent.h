#ifndef TORRENT_H
#define TORRENT_H

#include "parsing/buffer.h"
#include "tracker/url.h"
#include "parsing/bencode.h"
#include <string>

class torrent {	

public:

	buffer info_hash;
	long long length;
	url_t url;
	std::string name;

	unsigned int piece_length;
	unsigned int pieces;

	torrent(const std::string& filename);

	unsigned int get_piece_length(unsigned int piece);
	unsigned int get_n_blocks(unsigned int piece);
	unsigned int get_block_length(unsigned int piece, unsigned int block_index);

	static const unsigned int BLOCK_SIZE = (1<<14);

private:

	

	buffer get_bytes(const std::string& filename);
	buffer get_hash_info(const bencode::item& item);
	long long get_length(const bencode::item& item);

};

#endif // TORRENT_H
