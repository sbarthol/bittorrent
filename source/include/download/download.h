#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <vector>
#include "tracker/tracker.h"
#include <fstream>
#include <mutex>

class download {

private:
	std::vector<std::vector<bool>> requested;
	std::vector<std::vector<bool>> received;
	const std::vector<peer>& peers;
	torrent& t;

	int received_count;
	int requested_count;
	int total_blocks;

	std::ofstream out;

	bool is_done();
	void show_progress_bar(double progress);

public:
	download(const std::vector<peer>& peers, torrent& t);
	void add_requested(int piece, int block);
	void add_received(int piece, int block, buffer b);
	bool is_needed(int piece, int block);
	
	void start();
	double completed();

	std::mutex m;
	static const int BLOCK_SIZE = (1<<14);
};

#endif // DOWNLOAD_H