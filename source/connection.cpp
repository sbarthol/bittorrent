#include "tracker.h"
#include "tcp.h"
#include "buffer.h"
#include <algorithm>
#include "peer_id.h"
#include <stdexcept>
#include "connection.h"



#include <iostream>

using namespace std;

connection::connection(const peer& p, const torrent& t): 
	p(p), t(t), buff(buffer()), handshake(true) {}

void connection::download() {

	tcp tcp(p.host, p.port);
	tcp.send(build_handshake(t));

	cout<<"handshake sent:"<<endl;
	print(build_handshake(t));

	buffer b;

	while(!tcp.closed()) {

		b = get_message(tcp);

		if(handshake) {

			// ignore b
			cout<<"handshake received:"<<endl;
			print(b);

			tcp.send(build_interested());
			handshake = false;

		} else {

			cout<<"something received:"<<endl;
			print(b);

			// message handler
		}
	}
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

	setBE32(index, b, 5);
	setBE32(begin, b, 9);
	setBE32(length, b, 13);

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
		return handshake ? buff[0] + 49 : getBE32(buff, 0) + 4;
	};

	while(buff.size() < 4 || buff.size() < length()) {

		buffer b = client.receive();
		copy(b.begin(), b.end(), back_inserter(buff));
	}

	buffer msg(buff.begin(), buff.begin() + length());
	buff.erase(buff.begin(), buff.begin() + length());

	return msg;
}