#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include "bencode.h"

using namespace std;

int main() {

	ifstream is;
	vector<char> bytes;

	is.open("../puppy.torrent", ios::binary);
	is.seekg(0, ios::end);
	size_t filesize=is.tellg();
	is.seekg(0, ios::beg);

	bytes.reserve(filesize);
	bytes.assign(istreambuf_iterator<char>(is),
	                     istreambuf_iterator<char>());


	bencode::Element e = ::bencode::parse(bytes);
}