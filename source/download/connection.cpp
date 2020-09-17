#include "tracker/tracker.h"
#include "tracker/tcp.h"
#include "parsing/buffer.h"
#include <algorithm>
#include "download/peer_id.h"
#include <stdexcept>
#include "download/connection.h"
#include "download/message.h"
#include <cassert>


// Todo enlever
#include <iostream>
#include <thread>

using namespace std;

mutex connection::m;

connection::connection(const peer& p, torrent& t, download& d): p(p), d(d), t(t), buff(buffer()), 
	handshake(true), choked(true), socket(p.host, p.port) {}

void connection::start_download() {

	socket.send(message::build_handshake(t));

	cout<<"tid "<< this_thread::get_id() <<":\t handshake sent"<<endl;

	buffer b;

	while(!socket.closed()) {

		b = get_message(socket);

		if(handshake) {

			// ignore b
			// Todo do some checks on the handshake
			cout<<"tid "<< this_thread::get_id() <<":\t handshake received"<<endl;

			socket.send(message::build_interested());
			handshake = false;

		} else {

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
						piece_handler(b);
						break;
					default:
						break;
				}
			}
		}
	}
}

void connection::choke_handler() {

	cout<<"tid "<< this_thread::get_id() <<":\t choke received"<<endl;
	socket.close();
}

void connection::unchoke_handler() {

	cout<<"tid "<< this_thread::get_id() <<":\t unchoke received"<<endl;
	choked = false;
	request_piece();
}

void connection::have_handler(buffer& b) {

	unsigned int piece = getBE32(b,5);
	if(piece >= t.pieces) 
		throw runtime_error("have message contains invalid piece");

	cout<<"tid "<< this_thread::get_id() <<":\t have received, piece = "<<piece<<endl;
	enqueue(piece);

	if(q.size() == 1) {
		request_piece();
	}
}

void connection::bitfield_handler(buffer& b) {

	cout<<"tid "<< this_thread::get_id() <<":\t bitfield received"<<endl;
	bool empty = q.empty();

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

	if(empty) {
		request_piece();
	}
}

void connection::request_piece() {

	// Todo make this thread safe

	if(choked) return;

	while(q.size() > 0) {

		job j = q.front();
		q.pop();

		unique_lock<mutex> lock(m);

		assert(j.begin % BLOCK_SIZE == 0);

		if(d.is_needed(j.index, j.begin / BLOCK_SIZE)) {

			assert(j.begin % BLOCK_SIZE == 0);
			d.add_requested(j.index, j.begin / BLOCK_SIZE);
			lock.unlock();

			socket.send(message::build_request(j.index, j.begin, j.length));
			cout<<"tid "<< this_thread::get_id() <<":\t request sent, piece = "<<j.index<<", block = "<<j.begin<<endl;
			
			break;

		} else {
			lock.unlock();
		}
	}
}

void connection::piece_handler(buffer& b) {

	unsigned int index = getBE32(b, 5);
	unsigned int begin = getBE32(b, 9);

	b.erase(b.begin(), b.begin() + 9);

	assert(begin % BLOCK_SIZE == 0);
	d.write_to_file(index, begin, b);

	unique_lock<mutex> lock(m);

	assert(begin % BLOCK_SIZE == 0);
	d.add_received(index, begin / BLOCK_SIZE);

	cout<<"tid "<< this_thread::get_id() <<":\t piece received, piece = "<<index<<", block = "<<begin<<", completed = "<<100.0*d.completed()<<endl;

	if(d.is_done()) {

		cout<<"Download complete!"<<endl;
		lock.unlock();
		socket.close();

	}else{
		
		lock.unlock();
		request_piece();
	}
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

	assert(piece < t.pieces);

	int n_blocks = t.get_n_blocks(piece);
	for(int i=0;i<n_blocks;i++) {
		cout<<"added to queue: piece: "<<piece<<", block: "<<i*t.BLOCK_SIZE<<endl;
		q.push(job(piece, i*t.BLOCK_SIZE, t.get_block_length(piece, i)));
	}
}
