#include "gtest/gtest.h"
#include "tracker/udp.h"
#include "parsing/buffer.h"
#include <iostream>
#include <future>
#include <chrono>
#include <functional>

using namespace std;

TEST(udp, dns_lookup) {

    udp client("tower.sachapro.com", 3333);

    string message = "hello";
    buffer buff(message.begin(), message.end());

    client.send(buff);
    buffer ret = client.receive();

    EXPECT_EQ (buff, ret);
}


