#include "tracker.h"
#include "tcp.h"
#include "buffer.h"
#include <algorithm>
#include "peer_id.h"
#include <stdexcept>
#include "connection.h"


// Todo enlever
#include <iostream>

using namespace std;

connection::connection(const peer& p, const torrent& t, vector<bool>& requested): p(p), t(t), buff(buffer()), 
	handshake(true), requested(requested) {}

void connection::download() {

	tcp socket(p.host, p.port);
	socket.send(build_handshake(t));

	cout<<"handshake sent:"<<endl;
	print(build_handshake(t));

	buffer b;

	while(!socket.closed()) {

		b = get_message(socket);

		if(handshake) {

			// ignore b
			cout<<"handshake received:"<<endl;
			print(b);

			socket.send(build_interested());
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
						have_handler(b,socket);
						break;
					case 5:
						bitfield_handler(b,socket);
						break;
					case 7:
						piece_handler(socket);
						break;
					default:
						break;
				}
			}
		}
	}
}

void connection::choke_handler() {


}

void connection::unchoke_handler() {

	
}

void connection::have_handler(buffer& b, tcp& socket) {

	unsigned int piece = getBE32(b,5);
	q.push(piece);

	if(q.size() == 1) {
		request_piece(socket);
	}
}

void connection::bitfield_handler(buffer& b, tcp& socket) {

	unsigned int n_bytes = getBE32(b,0) - 1;
	if(n_bytes != (t.pieces + 7) / 8) 
		throw runtime_error("bitfield has wrong number of bytes");
	
	for(long long piece = 0; piece < t.pieces; piece++) {
		
		if(b[5+(piece>>3)]&(1<<(7-(piece%8)))) {
			q.push(piece);
		}
	}

	if(q.size() == 1) {
		request_piece(socket);
	}
}

void connection::request_piece(tcp& socket) {

	// Todo make this thread safe

	long long piece = q.front();
	if(requested[piece]) {
		q.pop();
	}else{
		socket.send(build_request(piece, 0, 1));
	}
}

void connection::piece_handler(tcp& socket) {

	q.pop();
	request_piece(socket);
}

buffer connection::build_handshake(const torrent& t) {

	buffer hs;
	hs.push_back(19);

	string version = "BitTorrent protocol";
	copy(version.begin(), version.end(), back_inserter(hs));

	const int RESERVED_SIZE = 8;
	for(int i=0;i<RESERVED_SIZE;i++) {
		hs.push_back(0x00);
	}

	buffer hash = t.info_hash;
	copy(hash.begin(), hash.end(), back_inserter(hs));

	buffer id = peer_id::get();
	copy(id.begin(), id.end(), back_inserter(hs));

	return hs;
}

buffer connection::build_keep_alive() {

	return buffer(4);
}

buffer connection::build_choke() {

	buffer b(5);
	b[3]=1;
	b[4]=0;

	return b;
}

buffer connection::build_unchoke() {

	buffer b(5);
	b[3]=1;
	b[4]=1;

	return b;
}

buffer connection::build_interested() {

	buffer b(5);
	b[3]=1;
	b[4]=2;

	return b;
}

buffer connection::build_not_interested() {

	buffer b(5);
	b[3]=1;
	b[4]=3;

	return b;
}

buffer connection::build_have(const buffer& payload) {

	buffer b(9);
	b[3]=5;
	b[4]=4;
	copy(payload.begin(), payload.end(), b.begin()+5);

	return b;
}

buffer connection::build_bitfield(const buffer& bitfield) {

	buffer b(5+bitfield.size());
	setBE32(1+bitfield.size(), b, 0);
	b[4]=5;
	copy(bitfield.begin(), bitfield.end(), b.begin()+5);

	return b;
}

buffer connection::build_request(unsigned int index, 
				unsigned int begin, unsigned int length) {

	buffer b(17);
	
	b[3]=13;
	b[4]=6;

	for(int i=0;i<4;i++) {

		b[5+3-i] = index % 256;
		index /= 256;
	}

	for(int i=0;i<4;i++) {

		b[9+3-i] = begin % 256;
		begin /= 256;
	}

	for(int i=0;i<4;i++) {

		b[13+3-i] = length % 256;
		length /= 256;
	}

	print(b);
	return b;
}

buffer connection::build_piece(unsigned int index, 
				unsigned int begin, const buffer& block) {

	buffer b(13 + block.size());
	setBE32(9 + block.size(), b, 0);
	b[4]=7;

	setBE32(index, b, 5);
	setBE32(begin, b, 9);
	copy(block.begin(), block.end(), b.begin() + 13);

	return b;
}

buffer connection::build_cancel(unsigned int index, 
				unsigned int begin, unsigned int length) {

	buffer b(17);
	b[3]=13;
	b[4]=8;

	setBE32(index, b, 5);
	setBE32(begin, b, 9);
	setBE32(length, b, 13);

	return b;
}

buffer connection::build_port(unsigned int port) {

	buffer b(7);
	b[3]=3;
	b[4]=9;

	setBE16(port, b, 5);

	return b;
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