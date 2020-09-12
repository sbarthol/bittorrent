#include "peers.h"
#include <stdlib.h> 
#include <time.h>
#include <vector>
#include "udp.h"
#include "url.h"
#include <algorithm>

using namespace std;

buffer peers::build_conn_req() {

	#define SIZE_CONN 16
	#define RANDOM_SIZE 4

	unsigned char msg[SIZE_CONN] = {0x00, 0x00, 0x04, 0x17,
							0x27, 0x10, 0x19, 0x80,
							0x00, 0x00, 0x00, 0x00};	

	for(int i=0;i<RANDOM_SIZE;i++){
		msg[SIZE_CONN-RANDOM_SIZE+i]=rand();
	}

	return buffer(msg,msg+SIZE_CONN);
}

buffer peers::build_ann_req(const buffer& b) {

	#define SIZE_ANN 98

	buffer buff(SIZE_ANN, 0x00);

	// connection id
	copy(b.begin()+8, b.begin()+16, buff.begin());

	// action
	buff[11]=1;

	// transaction id
	copy(b.begin()+4,b.begin()+8, buff.begin()+12);

	// info hash

	return buff;
}

vector<string> peers::get(bencode::item e) {

	srand(time(NULL));

	url url(e);
	
	udp client(url.host, url.port);
	client.send(build_conn_req());
	buffer b = client.receive();

	return vector<string>();

}