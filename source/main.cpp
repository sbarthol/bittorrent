#include "parsing/torrent.h"
#include <iostream>
#include "download/peer_id.h"

using namespace std;

int main() {



	torrent t("../goosebumps.torrent");
	cout<<t.url.host<<endl;
	cout<<t.url.port<<endl;
	cout<<t.length<<endl;

	for(unsigned char c:t.info_hash){
		cout<<hex<<(int)c<<" ";
	}
	cout<<endl;
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