#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <vector>
#include "tracker/tracker.h"
#include <fstream>

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

	void show_progress_bar(double progress);

public:
	download(const std::vector<peer>& peers, torrent& t);
	void add_requested(int piece, int block);
	void add_received(int piece, int block, buffer b);
	bool is_needed(int piece, int block);
	
	void start();
	double completed();
	bool is_done();

	static const int BLOCK_SIZE = (1<<14);
};

#endif // DOWNLOAD_H