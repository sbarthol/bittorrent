#ifndef CONNECTION_H
#define CONNECTION_H

#include "parsing/torrent.h"
#include "tracker/tracker.h"
#include "parsing/buffer.h"
#include "tracker/tcp.h"
#include <vector>
#include <queue>
#include "download/download.h"
#include <mutex>

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
	static std::mutex m;

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
