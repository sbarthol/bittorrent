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

using namespace std;

TEST(tracker, puppy) {

    peer_id::generate();

    torrent t("../goosebumps.torrent");
    vector<peer> v = tracker::get_peers(t);

    cout<<"received "<<v.size()<<" peers"<<endl;
    
    connection c(v[0],t);
    c.download();
}
