#include "gtest/gtest.h"
#include "udp_client.h"
#include "buffer.h"
#include <iostream>
#include <future>
#include <chrono>
#include <functional>

using namespace std;

TEST(udp_client, dns_lookup) {

    udp_client client("tower.sachapro.com", 3333);

    string message = "hello";
    buffer buff(message.begin(), message.end());

    client.send(buff);
    buffer ret = client.receive();

    EXPECT_EQ (buff, ret);
}


