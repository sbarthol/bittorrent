#include "tracker.h"
#include "gtest/gtest.h"
#include <vector>
#include "torrent.h"
#include "buffer.h"
#include <stdio.h>
#include <iostream>
#include "bencode.h"
#include "peer_id.h"
#include "connection.h"
#include "download.h"

using namespace std;

TEST(connection, goosebumps) {

    peer_id::generate();

    torrent t("../goosebumps.torrent");
    vector<peer> v = tracker::get_peers(t);

    cout<<"received "<<v.size()<<" peers"<<endl;

    download d(v,t);
    d.start();
}
