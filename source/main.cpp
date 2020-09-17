#include "parsing/torrent.h"
#include <iostream>
#include "download/peer_id.h"
#include "tracker/tracker.h"
#include "download/download.h"

using namespace std;

int main(int argc, const char** argv) {

	if(argc < 2) {
		cout<<"usage: BitTorrent <torrent_file>"<<endl;
		return 0;
	}

	srand(time(NULL));
	peer_id::generate();

	torrent t(argv[1]);

	cout<<"Fetching peers from tracker... (This could take a while)"<<endl;
	vector<peer> v = tracker::get_peers(t);
	cout<<"Received "<<v.size()<<" peers"<<endl;

	download d(v,t);
	d.start();

	return 0;
}
