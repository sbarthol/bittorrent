#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include "bencode.h"
#include <stdexcept>
#include "udp.h"
#include "peers.h"
#include "url.h"

buffer get_bytes(std::string filename) {

	std::ifstream is;
	buffer bytes;

	is.open(filename, std::ios::binary);
	is.seekg(0, std::ios::end);
	size_t filesize=is.tellg();
	is.seekg(0, std::ios::beg);

	bytes.reserve(filesize);
	bytes.assign(std::istreambuf_iterator<char>(is),
	                     std::istreambuf_iterator<char>());

	return bytes;
}

int main() {

	// bencode::item e = bencode::parse(get_bytes("../puppy.torrent"));

	// using namespace std;

	// url url(e);
	
	// udp client(url.host, url.port);
	// client.send(peers::build_conn_req());
	// buffer b = client.receive();
	
	// for(unsigned char c:b)cout<<hex<<(int)c<<" ";
	// cout<<endl;

	// 0 0 0 0 - 36 c6 95 3d - 80 65 e1 4f c0 b2 aa 3e
}

/*

{
	"info":
	{
		"pieces":"T?k?/?_(?Sh%???+]q'B?٠:????p"?j???1-g"?s(??V??=?h?ma?nF?2????ǩ?_?"2???'?wO??-;ע?ؑ??L&????0?D_9??	\??O?h,n5g?(??仑,?\߰?%??U????C>??df??",
		"piece length":16384,
		"name":"puppy.jpg",
		"length":124234
	},
	"encoding":"UTF-8",
	"creation date":1462355939,
	"created by":"uTorrent/1870",
	"announce":"udp://tracker.coppersurfer.tk:6969/announce"
}

*/