#include "tracker.h"
#include <stdlib.h> 
#include <time.h>
#include <vector>
#include "udp.h"
#include "url.h"
#include <algorithm>
#include <stdexcept>
#include "peer_id.h"

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

buffer tracker::get_peers(const torrent& t) {

	srand(time(NULL));
	
	if (t.url.protocol == url_t::UDP) {

		udp client(t.url.host, t.url.port);
		client.send(build_conn_req_udp());
		buffer b = client.receive();

		client.send(build_ann_req_udp(b, t));
		return client.receive();

	} else if (t.url.protocol == url_t::HTTP) {

		http request(t.url);
		build_ann_req_http(request, t);
		return request.get();
	}

	throw runtime_error("protocol not recognized");
}
