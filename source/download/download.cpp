#include "download/download.h"
#include <thread>
#include "download/connection.h"
#include <algorithm>
#include <cassert>
#include <stdlib.h>
#include "download/farm.h"
#include <iostream>

using namespace std;

download::download(const vector<peer>& peers, torrent& t): 
						t(t), peers(peers), received_count(0), w(t.name) {

	received = vector<vector<bool>>(t.pieces);
	is_in_queue = vector<vector<bool>>(t.pieces);

	total_blocks = 0;

	for(int i=0;i<t.pieces;i++) {

		int n = t.get_n_blocks(i);
		total_blocks += n;
		is_in_queue[i] = vector<bool>(n);
		received[i] = vector<bool>(n);
	}

	s.set_total(total_blocks * BLOCK_SIZE);
}

void download::start() {

	if(peers.size() == 0) throw runtime_error("no peers");

	w.start();
	s.start();

	vector<connection> conns;
	for(const peer& p: peers) {
		
		try {
			conns.push_back(connection(p, t, *this));
		} catch (exception& e) {
			cout<<"connection constructor threw: "<<e.what()<<endl;
		}
	}

	farm f(conns, *this);
	f.hatch();

	w.stop();
	s.stop();
}

void download::add_received(int piece, int block, buffer piece_data) {

	assert(piece >= 0 && piece < t.pieces);
	assert(block >= 0 && block < t.get_n_blocks(piece));

	if(received[piece][block]) return;

	int offset = block * BLOCK_SIZE;
	s.add(piece_data.size());
	w.add(piece_data, piece * t.piece_length + offset);

	received_count++;
	received[piece][block] = true;
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

void download::push_job(job j) {

	assert(j.index >= 0 && j.index < t.pieces);
	assert(j.begin % BLOCK_SIZE == 0);
	assert(j.begin / BLOCK_SIZE >= 0 && j.begin / BLOCK_SIZE < t.get_n_blocks(j.index));
	assert(0 < j.length && j.length <= BLOCK_SIZE);

	if (is_in_queue[j.index][j.begin / BLOCK_SIZE]) {
		return;
	}

	is_in_queue[j.index][j.begin / BLOCK_SIZE] = true;
	jobs.push(j);
}

download::job download::pop_job() {

	while(!jobs.empty()) {
		
		job j = jobs.top();
		jobs.pop();
		if(!received[j.index][j.begin / BLOCK_SIZE]) {

			j.requested++;
			jobs.push(j);
			return j;
		}
	}

	throw exception();
}

