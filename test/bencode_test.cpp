#include "gtest/gtest.h"
#include "bencode.h"
#include <vector>
#include <string>
#include <iostream>
#include <map>

using namespace std;

TEST(byte_string, valid) {

    bencode::Element tested = bencode::parse(vector<char> {'3',':','p','i','e'});

    bencode::Element expected;
    expected.t = bencode::bs;
    expected.data = vector<char> {'p','i','e'};

    EXPECT_EQ (tested, expected);

    tested = bencode::parse(vector<char> {'0',':'});
    expected.data = vector<char>();

    EXPECT_EQ (tested, expected);
}


TEST(byte_string, invalid) {

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

TEST(integer, valid) {

    bencode::Element tested = bencode::parse(vector<char> {'i','1','2','e'});

    bencode::Element expected;
    expected.t = bencode::i;
    expected.data = 12;

    EXPECT_EQ (tested, expected);

    tested = bencode::parse(vector<char> {'i','-','1','2','e'});
    expected.data = -12;

    EXPECT_EQ (tested, expected);

    tested = bencode::parse(vector<char> {'i','-','0','e'});
    expected.data = 0;

    EXPECT_EQ (tested, expected);

    tested = bencode::parse(vector<char> {'i','0','e'});
    expected.data = 0;

    EXPECT_EQ (tested, expected);
}


TEST(integer, invalid) {

    vector<char> tested1 = vector<char> {'i','-','-','e'};
    vector<char> tested2 = vector<char> {'i','1','2','3','4'};
    vector<char> tested3 = vector<char> {'i','1','e','e'};
    vector<char> tested4 = vector<char> {'i','e'};
    vector<char> tested5 = vector<char> {'i'};

    vector<vector<char>> v {tested1, tested2, tested3, tested4, tested5}; 

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
    bencode::Element tested = bencode::parse(vector<char>(s.begin(), s.end()));

    bencode::Element a;
    a.t=bencode::i;
    a.data=35;

    bencode::Element b;
    b.t=bencode::bs;
    b.data=vector<char>{'a','b'};

    bencode::Element expected;
    expected.t = bencode::l;
    expected.data = vector<bencode::Element>{a,b};

    EXPECT_EQ (expected, tested);
}

TEST(list, valid2) {

	string s = "lleli1eee";
    bencode::Element tested = bencode::parse(vector<char>(s.begin(), s.end()));

    bencode::Element a;
    a.t=bencode::l;
    a.data=vector<bencode::Element>();

    bencode::Element b;
    b.t=bencode::i;
    b.data=1;

    bencode::Element c;
    c.t=bencode::l;
    c.data=vector<bencode::Element>(1,b);

    bencode::Element expected;
    expected.t = bencode::l;
    expected.data = vector<bencode::Element>{a,c};

    EXPECT_EQ (expected, tested);
}


TEST(list, invalid) {

    vector<char> tested1 = vector<char> {'l','i','2','e'};
    vector<char> tested2 = vector<char> {'l','l'};
    vector<char> tested3 = vector<char> {'l','x','1','e'};
    vector<char> tested4 = vector<char> {'l','e','e'};
    vector<char> tested5 = vector<char> {'l'};

    vector<vector<char>> v {tested1, tested2, tested3, tested4, tested5}; 

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
    bencode::Element tested = bencode::parse(vector<char>(s.begin(), s.end()));

    bencode::Element a;
    a.t=bencode::bs;
    a.data=vector<char>{'b','a','r'};

    bencode::Element b;
    b.t=bencode::bs;
    b.data=vector<char>{'s','p','a','m'};

    bencode::Element c;
    c.t=bencode::bs;
    c.data=vector<char>{'f','o','o'};

    bencode::Element d;
    d.t=bencode::i;
    d.data=42;

    map<bencode::Element,bencode::Element> data;
    data[a]=b;
    data[c]=d;

    bencode::Element expected;
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
    		vector<char> t(s.begin(), s.end());
    		bencode::parse(t);
    		FAIL() << "Expected bencode::invalid_bencode exception";
    	} catch (bencode::invalid_bencode e) {

    	} catch (...) {
    		FAIL() << "Wrong exception";
    	}
    }
}
