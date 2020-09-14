#include "tracker.h"
#include "gtest/gtest.h"
#include <vector>
#include "torrent.h"
#include "buffer.h"
#include <stdio.h>
#include <iostream>
#include "bencode.h"
#include "peer_id.h"

using namespace std;

TEST(tracker, simple) {

    peer_id::generate();

    torrent t("../goosebumps.torrent");
    buffer b = tracker::get_peers(t);
    bencode::item item = bencode::parse(b);

    bencode::print(item);
}

/*


{
    "announce":"http://tracker.yggtracker.cc:8080/IEBVvI0dWAS5pBBKtKDpggBzr00fImwa/announce",
    "created by":"YggTorrent",
    "creation date":1599953075,
    "info":{
            "length":2202273313,
            "name":"Goosebumps.2.2018.MULTI.TRUEFRENCH.1080p.BluRay.HDLight.x264.AC3-TOXIC.mkv",
            "piece length":524288,
            "pieces":"...",
            "private":1
            }
}


*/