#ifndef CONNECTION_H
#define CONNECTION_H

#include "torrent.h"
#include "tracker.h"
#include "buffer.h"
#include "tcp.h"
#include <vector>
#include <queue>
#include "download.h"

class connection {

private:
	buffer get_message(tcp& client);

	void choke_handler();
	void unchoke_handler();
	void have_handler(buffer& b);
	void bitfield_handler(buffer& b);
	void piece_handler(buffer& b);

	void request_piece();

	void enqueue(int piece);

	const int BLOCK_SIZE = (1<<14);

	buffer buff;
	const peer& p;
	torrent& t;
	bool choked;
	bool handshake;
	download& d;
	tcp socket;

	struct job {
		int index;
		int begin;
		int length;

		job(int i, int b, int l): index(i), begin(b), length(l) {}
	};

	std::queue<job>q;

public:
	connection(const peer& p, torrent& t, download& d);
	void start_download();
};

#endif // CONNECTION_H
