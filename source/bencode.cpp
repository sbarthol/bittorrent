#include "bencode.h"

using namespace std;



// void rec(vector<char> const& s, size idx, Element& parent) {


// }

bencode::Element bencode::parse_byte_string(vector<char> const& s, size& idx) {

	// Todo, unallow leading 0s

	size len = 0;

	while(idx < s.size() && '0' <= s[idx] && s[idx] <= '9') {
		len *= 10;
		len += s[idx] - '0';
		idx++;
	}

	if(idx == s.size()) {
		throw invalid_bencode();
	}

	if(s[idx] != ':') {
		throw invalid_bencode();
	}

	byte_string result(len);

	idx++;
	if(idx + len > s.size()) {
		throw invalid_bencode();
	}

	for(size i = 0; i < len; i++) {
		result[i] = s[idx+i];
	}

	idx += len;
	return make_pair(move(result), bs);
}

bencode::Element bencode::parse(vector<char> const& s) {

	if(s.size() == 0) throw invalid_bencode("empty string");



	if(s[0] == 'i') {

	}else if('0' <= s[0] && s[0] <= '9') {

		size idx=0;
		Element e = bencode::parse_byte_string(s, idx);

		if(idx < s.size()) {
			throw invalid_bencode();
		}

		return e;

	}else if(s[0] == 'l') {

	}else if(s[0] == 'd') {

	}else {
		throw invalid_bencode("wrong first character");
	}
}