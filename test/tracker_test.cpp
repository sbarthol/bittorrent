#include "tracker/tracker.h"
#include "gtest/gtest.h"
#include <vector>
#include "parsing/torrent.h"
#include "parsing/buffer.h"
#include <stdio.h>
#include <iostream>
#include "parsing/bencode.h"
#include "download/peer_id.h"

using namespace std;

TEST(tracker, goosebumps) {

    peer_id::generate();

    torrent t("../sample/goosebumps.torrent");
    vector<peer> v = tracker::get_peers(t);
    
    for(peer p:v){
        cout<<"host: "<<p.host<<endl;
        cout<<"port: "<<p.port<<endl;
        cout<<endl;
    }
}

TEST(tracker, puppy) {

    peer_id::generate();

    torrent t("../sample/puppy.torrent");
    vector<peer> v = tracker::get_peers(t);
    
    for(peer p:v){
        cout<<"host: "<<p.host<<endl;
        cout<<"port: "<<p.port<<endl;
        cout<<endl;
    }
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