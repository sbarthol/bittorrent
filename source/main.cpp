#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include "bencode.h"

using namespace std;

vector<char> get_bytes(string filename) {

	ifstream is;
	vector<char> bytes;

	is.open(filename, ios::binary);
	is.seekg(0, ios::end);
	size_t filesize=is.tellg();
	is.seekg(0, ios::beg);

	bytes.reserve(filesize);
	bytes.assign(istreambuf_iterator<char>(is),
	                     istreambuf_iterator<char>());

	return bytes;
}

int main() {

	bencode::Element e = ::bencode::parse(get_bytes("../puppy.torrent"));
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