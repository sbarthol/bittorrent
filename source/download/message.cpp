#include "download/message.h"
#include "download/peer_id.h"

using namespace std;

buffer message::build_handshake(const torrent& t) {

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

buffer message::build_keep_alive() {

	return buffer(4);
}

buffer message::build_choke() {

	buffer b(5);
	b[3]=1;
	b[4]=0;

	return b;
}

buffer message::build_unchoke() {

	buffer b(5);
	b[3]=1;
	b[4]=1;

	return b;
}

buffer message::build_interested() {

	buffer b(5);
	b[3]=1;
	b[4]=2;

	return b;
}

buffer message::build_not_interested() {

	buffer b(5);
	b[3]=1;
	b[4]=3;

	return b;
}

buffer message::build_have(const buffer& payload) {

	buffer b(9);
	b[3]=5;
	b[4]=4;
	copy(payload.begin(), payload.end(), b.begin()+5);

	return b;
}

buffer message::build_bitfield(const buffer& bitfield) {

	buffer b(5+bitfield.size());
	setBE32(1+bitfield.size(), b, 0);
	b[4]=5;
	copy(bitfield.begin(), bitfield.end(), b.begin()+5);

	return b;
}

buffer message::build_request(unsigned int index, 
				unsigned int begin, unsigned int length) {

	buffer b(17);
	
	b[3]=13;
	b[4]=6;

	for(int i=0;i<4;i++) {

		b[5+3-i] = index & 0xff;
		index >>= 8;
	}

	for(int i=0;i<4;i++) {

		b[9+3-i] = begin & 0xff;
		begin >>= 8;
	}

	for(int i=0;i<4;i++) {

		b[13+3-i] = length & 0xff;
		length >>= 8;
	}

	return b;
}

buffer message::build_piece(unsigned int index, 
				unsigned int begin, const buffer& block) {

	buffer b(13 + block.size());
	setBE32(9 + block.size(), b, 0);
	b[4]=7;

	setBE32(index, b, 5);
	setBE32(begin, b, 9);
	copy(block.begin(), block.end(), b.begin() + 13);

	return b;
}

buffer message::build_cancel(unsigned int index, 
				unsigned int begin, unsigned int length) {

	buffer b(17);
	b[3]=13;
	b[4]=8;

	setBE32(index, b, 5);
	setBE32(begin, b, 9);
	setBE32(length, b, 13);

	return b;
}

buffer message::build_port(unsigned int port) {

	buffer b(7);
	b[3]=3;
	b[4]=9;

	setBE16(port, b, 5);

	return b;
}