#include "gtest/gtest.h"
#include "udp_client.h"
#include <iostream>
#include <future>
#include <chrono>
#include <functional>

using namespace std;

TEST(udp_client, simple) {

    udp_client client("127.0.0.1", 3333);

    string message = "hello";
    client.send(message);
    string ret = client.receive();

    EXPECT_EQ (message, ret);
}

TEST(udp_client, dns_lookup) {

    udp_client client("tower.sachapro.com", 3333);

    string message = "hello";
    client.send(message);
    string ret = client.receive();

    EXPECT_EQ (message, ret);
}


