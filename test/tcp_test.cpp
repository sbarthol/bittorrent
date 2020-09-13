#include "gtest/gtest.h"
#include "tcp.h"
#include "buffer.h"
#include <iostream>
#include <future>
#include <chrono>
#include <functional>

using namespace std;

TEST(tcp, dns_lookup) {

    tcp client("tower.sachapro.com", 4444);

    string message = "hello";
    buffer buff(message.begin(), message.end());

    client.send(buff);
    buffer ret = client.receive();

    EXPECT_EQ (buff, ret);
}


