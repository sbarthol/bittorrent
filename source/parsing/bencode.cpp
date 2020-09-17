#include "parsing/bencode.h"
#include <algorithm>
#include <iostream>
#include <string.h>

using namespace std;

bencode::item bencode::parse_byte_string(buffer const& s, size& idx) {

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

	buffer result(len);

	idx++;
	if(idx + len > s.size()) {
		throw invalid_bencode();
	}

	for(size i = 0; i < len; i++) {
		result[i] = s[idx+i];
	}

	idx += len;

	item e;
	e.t = bs;
	e.data = move(result);

	return e;
}

bencode::item bencode::parse_integer(buffer const& s, size& idx) {

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

	long long result = 0;
	while(idx < s.size() && '0' <= s[idx] && s[idx] <= '9') {
		result*=10;
		result+=s[idx]-'0';
		idx++;
	}

	if(idx == s.size()) throw invalid_bencode();
	if(s[idx] != 'e') throw invalid_bencode();
	idx++;

	item e;
	e.t = i;
	e.data = result * sign;

	return e;
}

void bencode::next(bencode::item& e, const buffer& s, size& idx) {

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

bencode::item bencode::parse_list(buffer const& s, size& idx) {

	if(s[idx] != 'l') throw invalid_bencode("does not start with l");
	idx++;

	if(idx >= s.size()) throw invalid_bencode("to small");

	vector<item> result;
	while(idx < s.size() && s[idx] != 'e') {

		item f;
		next(f, s, idx);
		result.push_back(f);
	}

	if (idx >= s.size()) throw invalid_bencode("no e found at the end");
	idx++;

	item e;
	e.t = l;
	e.data = move(result);

	return e;
}

bencode::item bencode::parse_dictionary(buffer const& s, size& idx) {

	if(s[idx] != 'd') throw invalid_bencode("does not start with d");
	idx++;

	if(idx >= s.size()) throw invalid_bencode("to small");

	map<item,item> result;
	while(idx < s.size() && s[idx] != 'e') {

		item key;
		next(key, s, idx);

		if(result.count(key) > 0) throw invalid_bencode("duplicate key");
		if(idx >= s.size()) throw invalid_bencode("to small");

		item value;
		next(value, s, idx);
		
		result[key] = value;
	}

	if (idx >= s.size()) throw invalid_bencode("no e found at the end");
	idx++;

	item e;
	e.t = d;
	e.data = move(result);

	return e;
}

bencode::item bencode::parse(buffer const& s) {

	if(s.size() == 0) throw invalid_bencode("empty string");

	size idx=0;
	item e;
	next(e, s, idx);

	if(idx < s.size()) {
		throw invalid_bencode();
	}

	return e;
}

static void print_rec(const bencode::item& e) {

	if(e.t==bencode::i){
		cout<<any_cast<long long>(e.data);
	}else if(e.t==bencode::l){
		auto v=any_cast<vector<bencode::item>>(e.data);
		cout<<"[";
		for(auto& x:v){
			print_rec(x);
			cout<<",";
		}
		cout<<"]";
	}else if(e.t==bencode::bs){
		auto v=any_cast<buffer>(e.data);
		cout<<"\"";
		for(char c:v){
			cout<<c;
		}
		cout<<"\"";
	}else if(e.t==bencode::d){
		auto v=any_cast<map<bencode::item,bencode::item>>(e.data);
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

void bencode::print(const bencode::item& e) {

	print_rec(e);
	cout<<endl;
}

bool bencode::item::operator==(const bencode::item& other) const {

	if(this->t != other.t) return false;

	switch (other.t) {

		case bs: 
			return any_cast<buffer>(other.data)
				== any_cast<buffer>(this->data);
		case i: 
			return any_cast<long long>(other.data)
				== any_cast<long long>(this->data);
		case l: 
			return any_cast<vector<bencode::item>>(other.data)
				== any_cast<vector<bencode::item>>(this->data);
		case d:
			return any_cast<map<bencode::item,bencode::item>>(other.data)
				== any_cast<map<bencode::item,bencode::item>>(this->data);
		default:
			return false;
	}
}

bool bencode::item::operator<(const bencode::item& other) const {

	if(this->t != other.t) return this->t < other.t;

	switch (other.t) {

		case bs: 
			return any_cast<buffer>(other.data)
				> any_cast<buffer>(this->data);
		case i: 
			return any_cast<long long>(other.data)
				> any_cast<long long>(this->data);
		case l: 
			return any_cast<vector<bencode::item>>(other.data)
				> any_cast<vector<bencode::item>>(this->data);
		case d:
			return any_cast<map<bencode::item,bencode::item>>(other.data)
				> any_cast<map<bencode::item,bencode::item>>(this->data);
		default:
			return false;
	}
}

bencode::item bencode::item::get(const item& key) const {

	if(this->t != d) throw runtime_error("not a dictionary");
	map<bencode::item,bencode::item> dic = 
		any_cast<map<bencode::item,bencode::item>>(this->data);

	if(dic.count(key) == 0) throw runtime_error("no key in dictionary");
	return dic[key];
}

buffer bencode::item::get_buffer(const char* key) const {

	item key_item;
	key_item.t = bs;
	key_item.data = buffer(key, key+strlen(key));

	item val = get(key_item);
	if(val.t != bs) throw runtime_error("value not of type bs");

	return any_cast<buffer>(val.data);
}

bencode::item bencode::item::get_item(const char* key) const {

	item key_item;
	key_item.t = bs;
	key_item.data = buffer(key, key+strlen(key));

	return get(key_item);
}

long long bencode::item::get_int(const char* key) const {

	item key_item;
	key_item.t = bs;
	key_item.data = buffer(key, key+strlen(key));

	item val = get(key_item);
	if(val.t != i) throw runtime_error("value not of type i");

	return any_cast<long long>(val.data);
}

string bencode::item::get_string(const char* key) const {

	buffer buff = get_buffer(key);
	for(char c:buff){
		if(c<0) throw runtime_error("not a valid ASCII buffer");
	}

	return string(buff.begin(), buff.end());
}

vector<bencode::item> bencode::item::get_list(const char* key) const {

	item key_item;
	key_item.t = bs;
	key_item.data = buffer(key, key+strlen(key));

	item val = get(key_item);
	if(val.t != l) throw runtime_error("value not of type l");

	return any_cast<vector<item>>(val.data);
}

bool bencode::item::key_present(const char* key) const {

	item key_item;
	key_item.t = bs;
	key_item.data = buffer(key, key+strlen(key));

	if(this->t != d) throw runtime_error("not a dictionary");
	map<bencode::item,bencode::item> dic = 
		any_cast<map<bencode::item,bencode::item>>(this->data);

	return dic.count(key_item) != 0;
}

buffer bencode::encode(const bencode::item& e) {

	buffer ans;

	if (e.t == i) {

		ans.push_back('i');
		long long x=any_cast<long long>(e.data);
		string number = to_string(x);
		copy(number.begin(), number.end(), back_inserter(ans));
		ans.push_back('e');

	} else if (e.t == bs) {

		buffer x = any_cast<buffer>(e.data);
		int len = x.size();
		string number = to_string(len);
		copy(number.begin(), number.end(), back_inserter(ans));
		ans.push_back(':');
		copy(x.begin(), x.end(), back_inserter(ans));

	} else if (e.t == l) {

		vector<item> v = any_cast<vector<item>>(e.data);
		ans.push_back('l');
		for(item i:v){
			buffer tmp=encode(i);
			copy(tmp.begin(), tmp.end(), back_inserter(ans));
		}
		ans.push_back('e');

	} else if (e.t == d) {

		map<item,item> m = any_cast<map<item,item>>(e.data);
		ans.push_back('d');
		for(pair<item,item> i:m){
			buffer tmp=encode(i.first);
			copy(tmp.begin(), tmp.end(), back_inserter(ans));
			tmp=encode(i.second);
			copy(tmp.begin(), tmp.end(), back_inserter(ans));
		}
		ans.push_back('e');

	} else {

		throw invalid_bencode("invalid element found");
	}

	return ans;
}
