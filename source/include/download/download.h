#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <vector>
#include "tracker/tracker.h"
#include <queue>
#include "download/worker.h"

class download {

private:
	std::vector<std::vector<bool>> received;
	std::vector<std::vector<bool>> is_in_queue;
	const std::vector<peer>& peers;
	torrent& t;

	writer w;
	speed s;

	int received_count;
	int total_blocks;

public:
	download(const std::vector<peer>& peers, torrent& t);
	void add_received(int piece, int block, buffer b);
	
	void start();
	double completed();
	bool is_done();

	struct job {
		int index;
		int begin;
		int length;

		int requested;

		job(int i, int b, int l): index(i), begin(b), length(l), requested(0) {}
		job() {}
		bool operator<(const job& other) const {return this->requested > other.requested;}
	};

	void push_job(job j);
	job pop_job();

	static const int BLOCK_SIZE = (1<<14);

private:
	std::priority_queue<job> jobs;
};

#endif // DOWNLOAD_H