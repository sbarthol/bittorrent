#include "gtest/gtest.h"
#include "parsing/bencode.h"
#include <vector>
#include <string>
#include <iostream>
#include <map>

using namespace std;

TEST(byte_string, valid) {

    bencode::item tested = bencode::parse(buffer {'3',':','p','i','e'});

    bencode::item expected;
    expected.t = bencode::bs;
    expected.data = buffer {'p','i','e'};

    EXPECT_EQ (tested, expected);

    tested = bencode::parse(buffer {'0',':'});
    expected.data = buffer();

    EXPECT_EQ (tested, expected);
}


TEST(byte_string, invalid) {

    buffer tested1 = buffer {'2',':','p','i','e'};
    buffer tested2 = buffer {'4',':','p','i','e'};
    buffer tested3 = buffer {':','p','i','e'};
    buffer tested4 = buffer {'3',':'};

    vector<buffer> v {tested1, tested2, tested3, tested4}; 

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

TEST(integer, valid) {

    bencode::item tested = bencode::parse(buffer {'i','1','2','e'});

    bencode::item expected;
    expected.t = bencode::i;
    expected.data = 12ll;

    EXPECT_EQ (tested, expected);

    tested = bencode::parse(buffer {'i','-','1','2','e'});
    expected.data = -12ll;

    EXPECT_EQ (tested, expected);

    tested = bencode::parse(buffer {'i','-','0','e'});
    expected.data = 0ll;

    EXPECT_EQ (tested, expected);

    tested = bencode::parse(buffer {'i','0','e'});
    expected.data = 0ll;

    EXPECT_EQ (tested, expected);
}


TEST(integer, invalid) {

    buffer tested1 = buffer {'i','-','-','e'};
    buffer tested2 = buffer {'i','1','2','3','4'};
    buffer tested3 = buffer {'i','1','e','e'};
    buffer tested4 = buffer {'i','e'};
    buffer tested5 = buffer {'i'};

    vector<buffer> v {tested1, tested2, tested3, tested4, tested5}; 

    for(auto t: v) {
    	try {
    		bencode::parse(t);
    		FAIL() << "Expected bencode::invalid_bencode exception";
    	} catch (bencode::invalid_bencode e) {

    	} catch (...) {
    		FAIL() << "Wrong exception";
    	}
    }
}

TEST(list, valid1) {

	string s = "li35e2:abe";
    bencode::item tested = bencode::parse(buffer(s.begin(), s.end()));

    bencode::item a;
    a.t=bencode::i;
    a.data=35ll;

    bencode::item b;
    b.t=bencode::bs;
    b.data=buffer{'a','b'};

    bencode::item expected;
    expected.t = bencode::l;
    expected.data = vector<bencode::item>{a,b};

    EXPECT_EQ (expected, tested);
}

TEST(list, valid2) {

	string s = "lleli1eee";
    bencode::item tested = bencode::parse(buffer(s.begin(), s.end()));

    bencode::item a;
    a.t=bencode::l;
    a.data=vector<bencode::item>();

    bencode::item b;
    b.t=bencode::i;
    b.data=1ll;

    bencode::item c;
    c.t=bencode::l;
    c.data=vector<bencode::item>(1,b);

    bencode::item expected;
    expected.t = bencode::l;
    expected.data = vector<bencode::item>{a,c};

    EXPECT_EQ (expected, tested);
}


TEST(list, invalid) {

    buffer tested1 = buffer {'l','i','2','e'};
    buffer tested2 = buffer {'l','l'};
    buffer tested3 = buffer {'l','x','1','e'};
    buffer tested4 = buffer {'l','e','e'};
    buffer tested5 = buffer {'l'};

    vector<buffer> v {tested1, tested2, tested3, tested4, tested5}; 

    for(auto t: v) {
    	try {
    		bencode::parse(t);
    		FAIL() << "Expected bencode::invalid_bencode exception";
    	} catch (bencode::invalid_bencode e) {

    	} catch (...) {
    		FAIL() << "Wrong exception";
    	}
    }
}

TEST(dictionary, valid) {

	string s = "d3:bar4:spam3:fooi42ee";
    bencode::item tested = bencode::parse(buffer(s.begin(), s.end()));

    bencode::item a;
    a.t=bencode::bs;
    a.data=buffer{'b','a','r'};

    bencode::item b;
    b.t=bencode::bs;
    b.data=buffer{'s','p','a','m'};

    bencode::item c;
    c.t=bencode::bs;
    c.data=buffer{'f','o','o'};

    bencode::item d;
    d.t=bencode::i;
    d.data=42ll;

    map<bencode::item,bencode::item> data;
    data[a]=b;
    data[c]=d;

    bencode::item expected;
    expected.t = bencode::d;
    expected.data = data;

    EXPECT_EQ (expected, tested);
}


TEST(dictionary, invalid) {

    string s1="dee";
    string s2="d";
    string s3="dd";
    string s4="di1ei2ei3ee";
    string s5="d3:aaae";

    vector<string> v {s1,s2,s3,s4,s5}; 

    for(auto s: v) {
    	try {
    		buffer t(s.begin(), s.end());
    		bencode::parse(t);
    		FAIL() << "Expected bencode::invalid_bencode exception";
    	} catch (bencode::invalid_bencode e) {

    	} catch (...) {
    		FAIL() << "Wrong exception";
    	}
    }
}

TEST(encode, valid) {

    string s = "d3:barli-1ei34e3:samee";
    buffer t(s.begin(), s.end());
    bencode::item e = bencode::parse(t);

    buffer r = bencode::encode(e);

    EXPECT_EQ (t, r);
}

TEST(subscript, integer) {

    string s="d3:sami34ee";
    buffer t(s.begin(), s.end());
    bencode::item e = bencode::parse(t);

    long long val = e.get_int("sam");
    EXPECT_EQ (val, 34ll);
}

TEST(subscript, buffer) {

    string s="d3:sam3:t\x88me";
    buffer t(s.begin(), s.end());
    bencode::item e = bencode::parse(t);

    buffer val = e.get_buffer("sam");
    buffer expected {'t',0x88,'m'};
    EXPECT_EQ (val, expected);
}

TEST(subscript, string_invalid) {

    string s="d3:sam3:t\x88me";
    buffer t(s.begin(), s.end());
    bencode::item e = bencode::parse(t);

    try {
        e.get_string("sam");
        FAIL() << "Expected to fail";
    } catch (runtime_error e) {

    } catch (...) {
        FAIL() << "Wrong exception";
    }
}

TEST(subscript, string_valid) {

    string s="d3:sam3:tome";
    buffer t(s.begin(), s.end());
    bencode::item e = bencode::parse(t);

    string val = e.get_string("sam");
    string expected = "tom";
    EXPECT_EQ (val, expected);

}
