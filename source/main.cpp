#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include "bencode.h"
#include <stdexcept>
#include "udp_client.h"
#include "peers.h"

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

	bencode::item e = bencode::parse(get_bytes("../puppy.torrent"));
	bencode::item url_bencode = e["announce"];

	bencode::print(url_bencode);

	if(url_bencode.t != bencode::bs) throw std::runtime_error("announce is not a byte string");
	std::cout<<"ici"<<std::endl;
	buffer url_buffer = std::any_cast<buffer>(url_bencode.data);
	std::string s(url_buffer.begin(), url_buffer.end());
	std::cout<<s<<std::endl;

	udp_client client("tracker.coppersurfer.tk", 6969);
	client.send(peers::build_conn_req());
	buffer b = client.receive();
	for(unsigned char c:b)std::cout<<std::hex<<(int)c<<" ";
	std::cout<<std::endl;

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