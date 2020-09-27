#ifndef CONNECTION_H
#define CONNECTION_H

#include "parsing/torrent.h"
#include "tracker/tracker.h"
#include "parsing/buffer.h"
#include "tracker/tcp.h"
#include <vector>
#include <queue>
#include "download/download.h"

class connection {

private:
	void handle(buffer& msg);

	void choke_handler();
	void unchoke_handler();
	void have_handler(buffer& b);
	void bitfield_handler(buffer& b);
	void piece_handler(buffer& b);

	void request_piece();
	void enqueue(int piece);

	buffer buff;
	const peer& p;
	torrent& t;
	bool choked;
	bool handshake;
	bool connected;
	download& d;

	struct job {
		int index;
		int begin;
		int length;

		job(int i, int b, int l): index(i), begin(b), length(l) {}
	};

	std::queue<job>q;

public:
	tcp socket;
	connection(const peer& p, torrent& t, download& d);
	void ready();
};

#endif // CONNECTION_H
