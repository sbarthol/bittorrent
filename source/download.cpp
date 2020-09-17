#include "download.h"
#include <thread>
#include <iostream>
#include "connection.h"
#include <algorithm>

using namespace std;

download::download(const vector<peer>& peers, torrent& t): 
						t(t), peers(peers), received_count(0), requested_count(0) {

	requested = vector<vector<bool>>(t.pieces);
	received = vector<vector<bool>>(t.pieces);

	total_blocks = 0;

	for(int i=0;i<t.pieces;i++) {

		int n = t.get_n_blocks(i);
		total_blocks += n;
		requested[i] = vector<bool>(n);
		received[i] = vector<bool>(n);
	}
}

void download::start() {

	if(peers.size() == 0) throw runtime_error("no peers");

	vector<thread> threads(10);
	for(int i=0;i<threads.size();i++) {

		threads[i] = thread([this,i](){

			try {
				connection conn(peers[i], t, *this);
				conn.start_download();
			} catch (exception& e) {
				cout<<"thread threw: "<<e.what()<<endl;
			}
		});
	}

	for(thread& t:threads) {

		t.join();
	}
}

void download::add_requested(int piece, int block) {

	requested_count++;
	requested[piece][block] = true;
}

void download::add_received(int piece, int block) {

	received_count++;
	received[piece][block] = true;
}

bool download::is_needed(int piece, int block) {

	if(requested_count == total_blocks) {
		copy(received.begin(), received.end(), requested.begin());
		requested_count = received_count;
	}

	return !requested[piece][block];

}

bool download::is_done() {

	return received_count == total_blocks;
}

double download::completed() {

	return (double)received_count / total_blocks;
}

void download::write_to_file(int index, int begin, buffer& piece) {



}
