#include "connection.h"

class download {

private:
	std::vector<bool> requested;
	const std::vector<peer>& peers;
	const torrent t;

public:
	download(const std::vector<peer>& peers, const torrent& t);
	void start();
};