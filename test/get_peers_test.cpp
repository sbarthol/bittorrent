#include "get_peers.h"
#include "gtest/gtest.h"
#include <vector>

using namespace std;

TEST(get_peers, simple) {

    buffer s = get_peers::build_conn_req();
    for(unsigned char c:s){
    	cout << hex << setfill('0') << setw(2) << (int)c << " ";
    }

    //EXPECT_EQ (tested, expected);
}