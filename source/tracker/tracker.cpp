#include "tracker/tracker.h"
#include <stdlib.h> 
#include <time.h>
#include <vector>
#include "tracker/udp.h"
#include "tracker/url.h"
#include <algorithm>
#include <stdexcept>
#include "download/peer_id.h"
#include "parsing/bencode.h"

using namespace std;

buffer tracker::build_conn_req_udp() {

	const int SIZE_CONN = 16;
	const int RANDOM_SIZE = 4;

	unsigned char msg[SIZE_CONN] = {0x00, 0x00, 0x04, 0x17,
							0x27, 0x10, 0x19, 0x80,
							0x00, 0x00, 0x00, 0x00};	

	for(int i=0;i<RANDOM_SIZE;i++){
		msg[SIZE_CONN-RANDOM_SIZE+i]=rand() % 256;
	}

	return buffer(msg,msg+SIZE_CONN);
}

void tracker::build_ann_req_http(http& request, const torrent& t) {

	// info_hash
	request.add_argument("info_hash", t.info_hash);

	//peer_id
	request.add_argument("peer_id", peer_id::get());

	// port
	request.add_argument("port", "6881");

	// uploaded
	request.add_argument("uploaded", "0");

	// downloaded
	request.add_argument("downloaded", "0");

	// compact
	request.add_argument("compact", "1");

	// left
	request.add_argument("left", to_string(t.length));
}

buffer tracker::build_ann_req_udp(const buffer& b, const torrent& t) {

	const int SIZE_ANN = 98;

	buffer buff(SIZE_ANN, 0x00);

	// connection id
	copy(b.begin()+8, b.begin()+16, buff.begin());

	// action
	buff[11]=1;

	// transaction id
	copy(b.begin()+4,b.begin()+8, buff.begin()+12);

	// info hash
	copy(t.info_hash.begin(), t.info_hash.end(), buff.begin()+16);

	// peer id
	buffer id = peer_id::get();
	copy(id.begin(), id.end(), buff.begin()+36);

	// left
	long long x = t.length;
	for(int i=0;i<4;i++){
		buff[64+8-i-1] = x % 256;
		x/=256;
	}

	// key
	for(int i=0;i<4;i++){
		buff[88+i] = rand() % 256;
	}

	// num_want
	for(int i=0;i<4;i++){
		buff[92+i] = 0xff;
	}

	// port
	// Todo: allow port between 6881 and 6889
	int port = 6881;
	buff[97] = port % 256;
	buff[96] = (port / 256) % 256;

	return buff;
}

vector<peer> tracker::get_peers(const torrent& t) {

	if (t.url.protocol == url_t::UDP) {

		udp client(t.url.host, t.url.port);
		client.send(build_conn_req_udp());
		buffer b = client.receive();

		client.send(build_ann_req_udp(b, t));
		buffer c = client.receive();

		const int PEER_OFFSET = 20;
		c.erase(c.begin(), c.begin() + PEER_OFFSET);

		return get_peers(c);

	} else if (t.url.protocol == url_t::HTTP) {

		http request(t.url);
		build_ann_req_http(request, t);
		buffer encoded = request.get();

		if(encoded.size() == 0) {
			throw runtime_error("tracker responded with 0 bytes");
		}

		bencode::item item = bencode::parse(encoded);
		buffer peer_bytes = item.get_buffer("peers");
		return get_peers(peer_bytes);
	}

	throw runtime_error("protocol not recognized");
}

vector<peer> tracker::get_peers(const buffer& b) {

	const int PEER_BYTE_SIZE = 6;
	buffer::size_type n = b.size();

	if(n % PEER_BYTE_SIZE != 0) throw runtime_error("peer list not a multiple of 6");

	vector<peer> peers;
	for(auto i = 0; i < n; i+=6) {

		string host = to_string(b[i]);
		for(int j=1;j<4;j++){
			host += "." + to_string(b[i+j]);
		}

		int port = getBE16(b,i+4);

		peers.push_back(peer(host, port));
	}

	return peers;
}

