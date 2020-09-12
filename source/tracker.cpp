#include "tracker.h"
#include <stdlib.h> 
#include <time.h>
#include <vector>
#include "udp.h"
#include "url.h"
#include <algorithm>

using namespace std;

buffer tracker::build_conn_req() {

	#define SIZE_CONN 16
	#define RANDOM_SIZE 4

	unsigned char msg[SIZE_CONN] = {0x00, 0x00, 0x04, 0x17,
							0x27, 0x10, 0x19, 0x80,
							0x00, 0x00, 0x00, 0x00};	

	for(int i=0;i<RANDOM_SIZE;i++){
		msg[SIZE_CONN-RANDOM_SIZE+i]=rand() % 256;
	}

	return buffer(msg,msg+SIZE_CONN);
}

buffer tracker::build_ann_req(const buffer& b, const torrent& t) {

	#define SIZE_ANN 98

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
	// Todo: id stays the same until app closes
	string my_id = "-SA0001-";
	copy(my_id.begin(), my_id.end(), buff.begin()+36);
	for(int i=0;i+my_id.size()<20;i++){
		buff[36+my_id.size()+i] = rand() % 256;
	}

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
	
	udp client(t.url.host, t.url.port);
	client.send(build_conn_req());
	buffer b = client.receive();

	client.send(build_ann_req(b, t));
	buffer c = client.receive();

	return c;

}