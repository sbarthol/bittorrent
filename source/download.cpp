#include "download.h"
#include <thread>
#include <iostream>
#include "connection.h"
#include <algorithm>

using namespace std;

download::download(const vector<peer>& peers, torrent& t): 
														t(t), peers(peers) {

	requested = vector<vector<bool>>(t.pieces);
	received = vector<vector<bool>>(t.pieces);

	for(int i=0;i<t.pieces;i++) {
		requested[i] = vector<bool>(t.get_n_blocks(i));
		received[i] = vector<bool>(t.get_n_blocks(i));
	}
}

void download::start() {

	if(peers.size() == 0) throw runtime_error("no peers");

	vector<thread> threads(5);
	for(int i=0;i<threads.size();i++) {

		threads[i] = thread([this,i](){

			connection conn(peers[i], t, *this);
			try {
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

	lock_guard<mutex> l(requested_mutex);
	requested[piece][block] = true;
}

void download::add_received(int piece, int block) {

	lock_guard<mutex> l(received_mutex);
	received[piece][block] = true;
}

bool download::is_needed(int piece, int block) {

	if(is_done()) return false;

	scoped_lock sl(received_mutex, requested_mutex);

	bool all_flag=true;
	// todo: use a counter
	for_each(requested.begin(), requested.end(), [&all_flag](vector<bool> blocks){
		for_each(blocks.begin(), blocks.end(), [&all_flag](bool x){
			all_flag = all_flag && x;
		});
	});

	if(all_flag) {
		copy(received.begin(), received.end(), requested.begin());
	}

	return !requested[piece][block];

}

bool download::is_done() {

	bool all_flag=true;
	// todo: use a counter
	for_each(received.begin(), received.end(), [&all_flag](vector<bool> blocks){
		for_each(blocks.begin(), blocks.end(), [&all_flag](bool x){
			all_flag = all_flag && x;
		});
	});

	return all_flag;
}
