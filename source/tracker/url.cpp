#include "tracker/url.h"
#include <stdexcept>
#include <algorithm>

using namespace std;

url_t::url_t(const string& url) {

	string s = url;

	string pref_udp = "udp://";
	string pref_http = "http://";

	// udp://tracker.coppersurfer.tk:6969/announce

	if(s.substr(0,pref_udp.size()) == pref_udp) {
		s.erase(0,pref_udp.size());
		this->protocol = UDP;

	} else if(s.substr(0,pref_http.size()) == pref_http) {
		s.erase(0,pref_http.size());
		this->protocol = HTTP;

	} else {
		throw runtime_error("undefined protocol in url");
	}
	

	auto it = find(s.begin(), s.end(), ':');
	string host;
	copy(s.begin(), it, back_inserter(host));

	it++;
	s.erase(s.begin(), it);

	this->host = move(host);
	this->port = stoi(s);

	it = find(s.begin(), s.end(), '/');
	s.erase(s.begin(), it);

	this->path = move(s);
	
}