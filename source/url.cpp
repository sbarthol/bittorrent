#include "url.h"
#include <stdexcept>
#include <algorithm>

using namespace std;

url_t::url_t(const string& url) {

	string s = url;

	string pref = "udp://";
	s.erase(0,pref.size());

	auto it = find(s.begin(), s.end(), ':');
	string host;
	copy(s.begin(), it, back_inserter(host));

	it++;
	s.erase(s.begin(), it);

	this->host = move(host);
	this->port = stoi(s);
	
}