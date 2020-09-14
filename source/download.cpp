#include "download.h"

#include <iostream>

using namespace std;

download::download(const std::vector<peer>& peers, const torrent& t): t(t), peers(peers) {

	bencode::item info = t.dic.get_item("info");
	long long length = info.get_int("length");
	long long piece_length = info.get_int("piece length");

	long long pieces = (length + piece_length - 1) / piece_length;
	cout<<"there are "<<pieces<<" pieces"<<endl;
	requested = vector<bool>(pieces);
}

void download::start() {

	connection conn(peers[0], t, requested);
	conn.download();
}