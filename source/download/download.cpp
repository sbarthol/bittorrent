#include "download/download.h"
#include <thread>
#include <iostream>
#include "download/connection.h"
#include <algorithm>
#include <cassert>
#include <stdlib.h>

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

	cout<<"Wait for the download to complete ..."<<endl;
	show_progress_bar(0.0);

	int n_threads = min(vector<peer>::size_type(10), peers.size());
	vector<thread> threads(n_threads);
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

	if(requested[piece][block]) return;

	requested_count++;
	requested[piece][block] = true;
}

void download::add_received(int piece, int block, buffer piece_data) {

	assert(piece >= 0 && piece < t.pieces);
	assert(block >= 0 && block < t.get_n_blocks(piece));

	if(received[piece][block]) return;

	int offset = block * BLOCK_SIZE;
	out.seekp(piece * t.piece_length + offset, ios::beg);
	out.write(reinterpret_cast<char*>(piece_data.data()), piece_data.size());

	received_count++;
	received[piece][block] = true;

	double progress = (double)received_count / total_blocks;
	show_progress_bar(progress);

	if(is_done()) {
		cout<<endl<<"Download completed successfully!"<<endl;
		exit(0);
	}
}

void download::show_progress_bar(double progress) {

	int bar_width = 70;

	cout << "[";
	int pos = bar_width * progress;
	for (int i = 0; i < bar_width; ++i) {
		if (i < pos) cout << "=";
		else if (i == pos) cout << ">";
		else cout << " ";
	}
	cout << "] " << int(progress * 100.0) << " %\r";
	cout.flush();
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

