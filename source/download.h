#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <mutex>
#include <vector>
#include "tracker.h"

class download {

private:
	std::vector<std::vector<bool>> requested;
	std::vector<std::vector<bool>> received;
	const std::vector<peer>& peers;
	torrent& t;

	std::mutex requested_mutex;
	std::mutex received_mutex;

public:
	download(const std::vector<peer>& peers, torrent& t);
	void add_requested(int piece, int block);
	void add_received(int piece, int block);
	bool is_needed(int piece, int block);
	bool is_done();
	void start();
};

#endif // DOWNLOAD_H