#ifndef TORRENT_H
#define TORRENT_H

#include "parsing/buffer.h"
#include "tracker/url.h"
#include "parsing/bencode.h"

class torrent {	

public:

	buffer info_hash;
	long long length;
	url_t url;

	int pieces;

	torrent(const std::string& filename);

	int get_piece_length(int piece);
	int get_n_blocks(int piece);
	int get_block_length(int piece, int block_index);

	static const int BLOCK_SIZE = (1<<14);

private:

	int piece_length;

	buffer get_bytes(const std::string& filename);
	buffer get_hash_info(const bencode::item& item);
	long long get_length(const bencode::item& item);

};

#endif // TORRENT_H
