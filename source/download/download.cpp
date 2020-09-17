#include "download/download.h"
#include <thread>
#include <iostream>
#include "download/connection.h"
#include <algorithm>
#include <cassert>

using namespace std;

download::download(const vector<peer>& peers, torrent& t): 
						t(t), peers(peers), received_count(0), 
						requested_count(0), out(t.name, ios::binary) {

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

	vector<thread> threads(1);
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

	assert(piece >= 0 && piece < t.pieces);
	assert(block >= 0 && block < t.get_n_blocks(piece));

	requested_count++;
	requested[piece][block] = true;
}

void download::add_received(int piece, int block) {

	assert(piece >= 0 && piece < t.pieces);
	assert(block >= 0 && block < t.get_n_blocks(piece));

	received_count++;
	received[piece][block] = true;
}

bool download::is_needed(int piece, int block) {

	assert(piece >= 0 && piece < t.pieces);
	assert(block >= 0 && block < t.get_n_blocks(piece));

	if(requested_count == total_blocks) {
		copy(received.begin(), received.end(), requested.begin());
		requested_count = received_count;
	}

	return !requested[piece][block];

}

bool download::is_done() {

	bool is_done = received_count == total_blocks;

	auto lambda = [this](){

		bool all_true = true;
		for( auto v:received) {
			for(bool x:v){
				all_true = all_true && x;
			}
		}

		return all_true;
	};	

	if (is_done) {
		assert(lambda());
	}

	return is_done;
}

double download::completed() {

	return (double)received_count / total_blocks;
}

void download::write_to_file(int piece, int offset, buffer& piece_data) {

	assert(piece >= 0 && piece < t.pieces);
	assert(offset >= 0 && offset < t.piece_length);

	out.seekp(piece * t.piece_length + offset, ios::beg);
	out.write(reinterpret_cast<char*>(piece_data.data()), piece_data.size());
}
