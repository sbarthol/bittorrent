#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <vector>
#include "tracker.h"

class download {

private:
	std::vector<std::vector<bool>> requested;
	std::vector<std::vector<bool>> received;
	const std::vector<peer>& peers;
	torrent& t;

	int received_count;
	int requested_count;
	int total_blocks;

public:
	download(const std::vector<peer>& peers, torrent& t);
	void add_requested(int piece, int block);
	void add_received(int piece, int block);
	bool is_needed(int piece, int block);
	bool is_done();
	void start();
	void write_to_file(int index, int begin, buffer& piece);
	double completed();
};

#endif // DOWNLOAD_H