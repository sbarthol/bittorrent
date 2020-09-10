#include "bencode.h"
#include <algorithm>
#include <iostream>

using namespace std;

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

	vector<char> result(len);

	idx++;
	if(idx + len > s.size()) {
		throw invalid_bencode();
	}

	for(size i = 0; i < len; i++) {
		result[i] = s[idx+i];
	}

	idx += len;

	Element e;
	e.t = bs;
	e.data = move(result);

	return e;
}

bencode::Element bencode::parse_integer(vector<char> const& s, size& idx) {

	if(s[idx] != 'i') throw invalid_bencode("does not start with i");

	idx++;
	if(idx >= s.size()) throw invalid_bencode("to small");

	int sign = 1;

	if(s[idx] == '-') {
		sign = -1;
		idx++;
	}

	if(idx >= s.size() || s[idx] < '0' || s[idx] > '9') {
		throw invalid_bencode("wrong number");
	}

	int result = 0;
	while(idx < s.size() && '0' <= s[idx] && s[idx] <= '9') {
		result*=10;
		result+=s[idx]-'0';
		idx++;
	}

	if(idx == s.size()) throw invalid_bencode();
	if(s[idx] != 'e') throw invalid_bencode();
	idx++;

	Element e;
	e.t = i;
	e.data = result * sign;

	return e;
}

void bencode::next(bencode::Element& e, const vector<char>& s, size& idx) {

	if(s[idx] == 'i') {
		e = bencode::parse_integer(s, idx);
	}else if('0' <= s[idx] && s[idx] <= '9') {
		e = bencode::parse_byte_string(s, idx);
	}else if(s[idx] == 'l') {
		e = bencode::parse_list(s, idx);
	}else if(s[idx] == 'd') {
		e = bencode::parse_dictionary(s, idx);
	}else {
		throw invalid_bencode("wrong first character");
	}
}

bencode::Element bencode::parse_list(vector<char> const& s, size& idx) {

	if(s[idx] != 'l') throw invalid_bencode("does not start with l");
	idx++;

	if(idx >= s.size()) throw invalid_bencode("to small");

	vector<Element> result;
	while(idx < s.size() && s[idx] != 'e') {

		Element f;
		next(f, s, idx);
		result.push_back(f);
	}

	if (idx >= s.size()) throw invalid_bencode("no e found at the end");
	idx++;

	Element e;
	e.t = l;
	e.data = move(result);

	return e;
}

bencode::Element bencode::parse_dictionary(vector<char> const& s, size& idx) {

	if(s[idx] != 'd') throw invalid_bencode("does not start with d");
	idx++;

	if(idx >= s.size()) throw invalid_bencode("to small");

	map<Element,Element> result;
	while(idx < s.size() && s[idx] != 'e') {

		Element key;
		next(key, s, idx);

		if(result.count(key) > 0) throw invalid_bencode("duplicate key");
		if(idx >= s.size()) throw invalid_bencode("to small");

		Element value;
		next(value, s, idx);
		
		result[key] = value;
	}

	if (idx >= s.size()) throw invalid_bencode("no e found at the end");
	idx++;

	Element e;
	e.t = d;
	e.data = move(result);

	return e;
}

bencode::Element bencode::parse(vector<char> const& s) {

	if(s.size() == 0) throw invalid_bencode("empty string");

	size idx=0;
	Element e;
	next(e, s, idx);

	if(idx < s.size()) {
		throw invalid_bencode();
	}

	return e;
}

static void print_rec(const bencode::Element& e) {

	if(e.t==bencode::i){
		cout<<any_cast<int>(e.data);
	}else if(e.t==bencode::l){
		auto v=any_cast<vector<bencode::Element>>(e.data);
		cout<<"[";
		for(auto& x:v){
			print_rec(x);
			cout<<",";
		}
		cout<<"]";
	}else if(e.t==bencode::bs){
		auto v=any_cast<vector<char>>(e.data);
		cout<<"\"";
		for(char c:v){
			cout<<c;
		}
		cout<<"\"";
	}else if(e.t==bencode::d){
		auto v=any_cast<map<bencode::Element,bencode::Element>>(e.data);
		cout<<"{";
		for(auto kv:v){
			print_rec(kv.first);
			cout<<":";
			print_rec(kv.second);
			cout<<",";
		}
		cout<<"}";
	}else{
		throw bencode::invalid_bencode("not a valid type");
	}
}

void bencode::print(const bencode::Element& e) {

	print_rec(e);
	cout<<endl;
}

bool bencode::Element::operator==(const bencode::Element& other) const {

	if(this->t != other.t) return false;

	switch (other.t) {

		case bs: 
			return std::any_cast<std::vector<char>>(other.data)
				== std::any_cast<std::vector<char>>(this->data);
		case i: 
			return std::any_cast<int>(other.data)
				== std::any_cast<int>(this->data);
		case l: 
			return std::any_cast<std::vector<bencode::Element>>(other.data)
				== std::any_cast<std::vector<bencode::Element>>(this->data);
		case d:
			return std::any_cast<std::map<bencode::Element,bencode::Element>>(other.data)
				== std::any_cast<std::map<bencode::Element,bencode::Element>>(this->data);
		default:
			return false;
	}
}

bool bencode::Element::operator<(const bencode::Element& other) const {

	if(this->t != other.t) return this->t < other.t;

	switch (other.t) {

		case bs: 
			return std::any_cast<std::vector<char>>(other.data)
				< std::any_cast<std::vector<char>>(this->data);
		case i: 
			return std::any_cast<int>(other.data)
				< std::any_cast<int>(this->data);
		case l: 
			return std::any_cast<std::vector<bencode::Element>>(other.data)
				< std::any_cast<std::vector<bencode::Element>>(this->data);
		case d:
			return std::any_cast<std::map<bencode::Element,bencode::Element>>(other.data)
				< std::any_cast<std::map<bencode::Element,bencode::Element>>(this->data);
		default:
			return false;
	}
}