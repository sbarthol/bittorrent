#include "get_peers.h"
#include <stdlib.h> 
#include <time.h>
#include <vector>

using namespace std;

buffer get_peers::build_conn_req() {

	unsigned char msg[16] = {0x00, 0x00, 0x04, 0x17,
							0x27, 0x10, 0x19, 0x80,
							0x00, 0x00, 0x00, 0x00};

	srand(time(NULL));

	for(int i=0;i<4;i++){
		msg[12+i]=rand();
	}

	buffer ans(msg,msg+16);

	return ans;
}