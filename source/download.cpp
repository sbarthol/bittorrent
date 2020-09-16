#include "download.h"
#include <thread>
#include <iostream>

using namespace std;

download::download(const vector<peer>& peers, const torrent& t): 
														t(t), peers(peers) {

	requested = vector<bool>(t.pieces);
}

void download::start() {

	if(peers.size() == 0) throw runtime_error("no peers");

	vector<thread> threads(5);
	for(int i=0;i<threads.size();i++) {

		threads[i] = thread([this,i](){

			connection conn(peers[i], t, requested);
			try {
				conn.download();
			} catch (exception& e) {
				cout<<"thread threw: "<<e.what()<<endl;
			}
		});
	}

	for(thread& t:threads) {

		t.join();
	}

	
}