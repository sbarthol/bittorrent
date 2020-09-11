#include "gtest/gtest.h"
#include "udp_client.h"
#include <iostream>
#include <future>
#include <chrono>
#include <functional>

using namespace std;

TEST(udp_client, simple) {

    udp_client client("34.107.107.236", 41234);

    packaged_task<string()> task(bind(&udp_client::receive, &client));
    future<string> fut = task.get_future();

    client.send("hello");

    if(fut.wait_for(chrono::seconds(3)) == future_status::timeout) {
        FAIL() << "receive timed out";
    }

    string ret = fut.get();
    cout<<ret<<endl;

    //EXPECT_EQ (tested, expected);
}


