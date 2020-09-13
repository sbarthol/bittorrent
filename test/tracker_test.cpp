#include "tracker.h"
#include "gtest/gtest.h"
#include <vector>
#include "torrent.h"
#include "buffer.h"
#include <stdio.h>

using namespace std;

TEST(tracker, simple) {

    torrent t("../goosebumps.torrent");
    buffer b = tracker::get_peers(t);

    for(int i=0;i<b.size();i+=4){
    	for(int j=0;j<4&&i+j<b.size();j++){
    		printf("%02X ",b[i+j]);
    	}
    	puts("");
    }

    EXPECT_EQ(b[3],1);
    EXPECT_TRUE(b.size()>5*4);
}