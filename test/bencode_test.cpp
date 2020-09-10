#include "gtest/gtest.h"
#include "bencode.h"
#include <vector>
#include <string>

using namespace std;

TEST(simple_valid, test1) {

    bencode::Element tested = bencode::parse(vector<char> {'3',':','p','i','e'});
    bencode::Element expected = make_pair(vector<char> {'p','i','e'},bencode::bs);

    EXPECT_EQ (tested, expected);

    tested = bencode::parse(vector<char> {'0',':'});
    expected = make_pair(vector<char>(),bencode::bs);

    EXPECT_EQ (tested, expected);
}


TEST(simple_invalid, test1) {

    vector<char> tested1 = vector<char> {'2',':','p','i','e'};
    vector<char> tested2 = vector<char> {'4',':','p','i','e'};
    vector<char> tested3 = vector<char> {':','p','i','e'};
    vector<char> tested4 = vector<char> {'3',':'};

    vector<vector<char>> v {tested1, tested2, tested3, tested4}; 

    for(auto t: v) {
    	try {
    		bencode::parse(t);
    		FAIL() << "Expected bencode::invalid_exception";
    	} catch (bencode::invalid_bencode e) {

    	} catch (...) {
    		FAIL() << "Wrong exception";
    	}
    }
}