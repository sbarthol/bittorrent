#include "peers.h"
#include <stdlib.h> 
#include <time.h>
#include <vector>

using namespace std;

buffer peers::build_conn_req() {

	#define SIZE 16
	#define RANDOM_SIZE 4

	unsigned char msg[SIZE] = {0x00, 0x00, 0x04, 0x17,
							0x27, 0x10, 0x19, 0x80,
							0x00, 0x00, 0x00, 0x00};

	// Todo don't put here
	srand(time(NULL));

	for(int i=0;i<RANDOM_SIZE;i++){
		msg[SIZE-RANDOM_SIZE+i]=rand();
	}

	return buffer(msg,msg+SIZE);
}