#ifndef TORRENT_H
#define TORRENT_H

#include "buffer.h"

class torrent {

private:
	buffer info_hash;

public:
	torrent(const char* filename);
};

#endif // TORRENT_H
