#ifndef TORRENT_H
#define TORRENT_H

#include "buffer.h"
#include "url.h"
#include "bencode.h"

class torrent {	

public:

	buffer info_hash;
	long long length;
	url_t url;

	torrent(const std::string& filename);

private:

	buffer get_bytes(const std::string& filename);
	buffer get_hash_info(const bencode::item& item);
	long long get_length(const bencode::item& item);

};

#endif // TORRENT_H
