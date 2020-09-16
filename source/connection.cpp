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
	build_handshake(t).print();

	buffer b;

	while(!socket.closed()) {

		b = get_message(socket);

		if(handshake) {

			// ignore b
			cout<<"handshake received:"<<endl;
			b.print();

			socket.send(build_interested());
			handshake = false;

		} else {

			cout<<"something received:"<<endl;
			b.print();

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


}

void connection::unchoke_handler() {

	
}

void connection::have_handler(buffer& b, tcp& socket) {

	unsigned int piece = b.getBE32(5);
	if(!requested[piece]) {

		// Todo what to write here
		socket.send(build_request(piece, 0, 1));
		requested[piece] = true;
	}
}

void connection::bitfield_handler(buffer& b, tcp& socket) {

	cout<<"aaa"<<endl;
	unsigned int n_bytes = b.getBE32(0) - 1;
	cout<<"bbb"<<endl;
	if(n_bytes != (t.pieces + 7) / 8) 
		throw runtime_error("bitfield has wrong number of bytes");
	cout<<"ccc"<<endl;
	for(long long piece = 0; piece < t.pieces; piece++) {
		cout<<"ddd"<<endl;
		if(b[5+(piece>>3)]&(1<<(7-(piece%8)))) {
			cout<<"eee"<<endl;
			if(!requested[piece]) {
				cout<<"fff"<<endl;
				// Todo what to write here
				cout<<"pieces = "<<t.pieces<<endl;
				cout<<"req size = "<<requested.size()<<endl;
				cout<<"piece = "<<piece<<endl;
				socket.send(build_request(piece, 0, 1));
				cout<<"fgfgfgf"<<endl;
				requested[piece] = true;

				cout<<"ggg"<<endl;
			}
		}
	}
}

void connection::piece_handler() {

	
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
	b.setBE32(1+bitfield.size(), 0);
	b[4]=5;
	copy(bitfield.begin(), bitfield.end(), b.begin()+5);

	return b;
}

buffer connection::build_request(unsigned int index, 
				unsigned int begin, unsigned int length) {

	cout<<"1"<<endl;

	buffer b(17);
	cout<<"2"<<endl;
	b[3]=13;
	cout<<"3"<<endl;
	b[4]=6;
	cout<<"4"<<endl;

	b.setBE32(index, 5);

	cout<<"5"<<endl;
	b.setBE32(begin, 9);
	cout<<"6"<<endl;
	b.setBE32(length, 13);
	cout<<"7"<<endl;

	b.print();

	return b;
}

buffer connection::build_piece(unsigned int index, 
				unsigned int begin, const buffer& block) {

	buffer b(13 + block.size());
	b.setBE32(9 + block.size(), 0);
	b[4]=7;

	b.setBE32(index, 5);
	b.setBE32(begin, 9);
	copy(block.begin(), block.end(), b.begin() + 13);

	return b;
}

buffer connection::build_cancel(unsigned int index, 
				unsigned int begin, unsigned int length) {

	buffer b(17);
	b[3]=13;
	b[4]=8;

	b.setBE32(index, 5);
	b.setBE32(begin, 9);
	b.setBE32(length, 13);

	return b;
}

buffer connection::build_port(unsigned int port) {

	buffer b(7);
	b[3]=3;
	b[4]=9;

	b.setBE16(port, 5);

	return b;
}

buffer connection::get_message(tcp& client) {

	auto length = [this](){
		return handshake ? buff[0] + 49 : buff.getBE32(0) + 4;
	};

	while(buff.size() < 4 || buff.size() < length()) {

		buffer b = client.receive();
		copy(b.begin(), b.end(), back_inserter(buff));
	}

	buffer msg(buff.begin(), buff.begin() + length());
	buff.erase(buff.begin(), buff.begin() + length());

	return msg;
}