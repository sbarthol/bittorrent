#include "tracker.h"
#include "tcp.h"
#include "buffer.h"
#include <algorithm>
#include "peer_id.h"
#include <stdexcept>
#include "connection.h"
#include "message.h"


// Todo enlever
#include <iostream>

using namespace std;

connection::connection(const peer& p, torrent& t, download& d): p(p), d(d), t(t), buff(buffer()), 
	handshake(true), choked(true), socket(p.host, p.port) {}

void connection::start_download() {

	socket.send(message::build_handshake(t));

	cout<<"handshake sent:"<<endl;
	print(message::build_handshake(t));

	buffer b;

	while(!socket.closed()) {

		b = get_message(socket);

		if(handshake) {

			// ignore b
			// do some checks on the handshake
			cout<<"handshake received:"<<endl;
			print(b);

			socket.send(message::build_interested());
			handshake = false;

		} else {

			cout<<"something received:"<<endl;
			print(b);

			if(b.size() < 4) {
				throw runtime_error("message size less than 4");
			} else if (b.size() == 4) {
				// keep alive
			} else {

				switch (b[4]) {
					case 0:
						choke_handler();
						break;
					case 1:
						unchoke_handler();
						break;
					case 4:
						have_handler(b);
						break;
					case 5:
						bitfield_handler(b);
						break;
					case 7:
						piece_handler();
						break;
					default:
						break;
				}
			}
		}
	}
}

void connection::choke_handler() {

	socket.close();
	throw runtime_error("choked message received");
}

void connection::unchoke_handler() {

	choked = false;
	request_piece();
}

void connection::have_handler(buffer& b) {

	unsigned int piece = getBE32(b,5);
	enqueue(piece);

	if(q.size() == 1) {
		request_piece();
	}
}

void connection::bitfield_handler(buffer& b) {

	unsigned int n_bytes = getBE32(b,0) - 1;
	if(n_bytes != (t.pieces + 7) / 8) 
		throw runtime_error("bitfield has wrong number of bytes");
	
	for(long long piece = 0; piece < t.pieces; piece++) {
		
		if(b[5+(piece>>3)]&(1<<(7-(piece%8)))) {
			enqueue(piece);
		}
	}

	if(q.size() == 1) {
		request_piece();
	}
}

void connection::request_piece() {

	// Todo make this thread safe

	if(choked) return;

	while(q.size() > 0) {

		job j = q.front();
		q.pop();

		if(d.is_needed(j.index, j.begin)) {
			socket.send(message::build_request(j.index, j.begin, j.length));
			d.add_requested(j.index, j.begin);
			break;
		}
	}
}

void connection::piece_handler() {

	q.pop();
	request_piece();
}

buffer connection::get_message(tcp& client) {

	auto length = [this](){
		return handshake ? buff[0] + 49 : getBE32(buff,0) + 4;
	};

	while(buff.size() < 4 || buff.size() < length()) {

		buffer b = client.receive();
		copy(b.begin(), b.end(), back_inserter(buff));
	}

	buffer msg(buff.begin(), buff.begin() + length());
	buff.erase(buff.begin(), buff.begin() + length());

	return msg;
}

void connection::enqueue(int piece) {

	int n_blocks = t.get_n_blocks(piece);
	for(int i=0;i<n_blocks;i++) {
		q.push(job(piece, i*t.BLOCK_SIZE, t.get_block_length(piece, i)));
	}
}
