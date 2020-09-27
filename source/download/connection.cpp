#include "tracker/tracker.h"
#include "tracker/tcp.h"
#include "parsing/buffer.h"
#include <algorithm>
#include "download/peer_id.h"
#include <stdexcept>
#include "download/connection.h"
#include "download/message.h"
#include <cassert>
#include <unistd.h>

using namespace std;

connection::connection(const peer& p, torrent& t, download& d): p(p), d(d), t(t), buff(buffer()), 
	handshake(true), choked(true), connected(false), socket(p.host, p.port, false) {}

void connection::handle(buffer& msg) {

	if (handshake) {

		handshake = false;
		socket.send(message::build_interested());

	} else if (msg.size() < 4) {

		throw runtime_error("message size less than 4");

	} else if (msg.size() == 4) {

		// keep alive message

	} else {

		switch (msg[4]) {
			case 0:
				choke_handler();
				break;
			case 1:
				unchoke_handler();
				break;
			case 4:
				have_handler(msg);
				break;
			case 5:
				bitfield_handler(msg);
				break;
			case 7:
				piece_handler(msg);
				break;
			default:
				break;
		}
	}
}

void connection::choke_handler() {

	socket.close();
}

void connection::unchoke_handler() {

	choked = false;
	request_piece();
}

void connection::have_handler(buffer& b) {

	unsigned int piece = getBE32(b,5);
	if(piece >= t.pieces) 
		throw runtime_error("have message contains invalid piece");

	enqueue(piece);
	request_piece();
}

void connection::bitfield_handler(buffer& b) {

	unsigned int n_bytes = getBE32(b,0) - 1;
	if(n_bytes != (t.pieces + 7) / 8) 
		throw runtime_error("bitfield has wrong number of bytes");
	
	for(int i=0;i<n_bytes;i++) {

		unsigned char byte = b[5+i];

		for(int j=0;j<8;j++) {
			if(byte & (1<<(7-j))) {
				enqueue(i*8 + j);
			}
		}
	}

	request_piece();
}

void connection::request_piece() {

	if(choked) return;

	download::job j;

	try {
		j = d.pop_job();
	} catch(...) {
		socket.close();
		return;
	}

	assert(j.begin % download::BLOCK_SIZE == 0);
	socket.send(message::build_request(j.index, j.begin, j.length));
}

void connection::piece_handler(buffer& b) {

	unsigned int block_size = getBE32(b, 0) - 9;
	if (block_size > download::BLOCK_SIZE) {
		throw runtime_error("received a wrong blocksize");
	}

	unsigned int index = getBE32(b, 5);
	unsigned int begin = getBE32(b, 9);

	b.erase(b.begin(), b.begin() + 13);
	
	// Todo verify hash corresponds

	assert(b.size() == block_size);
	if (begin % download::BLOCK_SIZE != 0) {
		throw runtime_error("received a wrong begin");
	}

	d.add_received(index, begin / download::BLOCK_SIZE, b);
	request_piece();
}

void connection::ready() {

	if (!connected) {

		connected = true;
		socket.send(message::build_handshake(t));
		return;
	}

	auto length = [this](){
		return handshake ? buff[0] + 49 : getBE32(buff,0) + 4;
	};

	buffer b = socket.receive();
	copy(b.begin(), b.end(), back_inserter(buff));

	if (buff.size() >= 4 && buff.size() >= length()) {

		buffer msg(buff.begin(), buff.begin() + length());
		buff.erase(buff.begin(), buff.begin() + length());

		handle(msg);
	}
}

void connection::enqueue(int piece) {

	assert(piece < t.pieces);

	int n_blocks = t.get_n_blocks(piece);
	for(int i=0;i<n_blocks;i++) {
		d.push_job(download::job(piece, i*download::BLOCK_SIZE, t.get_block_length(piece, i)));
	}
}
