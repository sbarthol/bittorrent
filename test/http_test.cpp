#include "gtest/gtest.h"
#include "parsing/buffer.h"
#include "tracker/url.h"
#include "tracker/http.h"
#include <iostream>

using namespace std;

TEST(http, simple) {

	url_t url("http://tower.sachapro.com:8888/oktamer");

	EXPECT_EQ(url.host, "tower.sachapro.com");
	EXPECT_EQ(url.port, 8888);
	EXPECT_EQ(url.path, "/oktamer");
	EXPECT_EQ(url.protocol, url_t::HTTP);

    http request(url);
    request.add_argument("abc", "123");
    request.add_argument("efg", "456");

    buffer body = request.get();
    buffer expected {'1','2','3','4','5','6'};
    EXPECT_EQ(body, expected);

}

TEST(url, simple) {

	url_t url("udp://tracker.coppersurfer.tk:6969/announce");

	EXPECT_EQ(url.host, "tracker.coppersurfer.tk");
	EXPECT_EQ(url.port, 6969);
	EXPECT_EQ(url.path, "/announce");
	EXPECT_EQ(url.protocol, url_t::UDP);
}


