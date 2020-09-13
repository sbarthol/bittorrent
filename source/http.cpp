#include "http.h"
#include <algorithm>
#include <string>

using namespace std;

http::http(const url_t& url): 
	socket(url.host, url.port), url(url), n_args(0) {
		
		path = buffer(url.path.begin(), url.path.end());
}

void http::add_argument(const string& key, const buffer& value) {

	path.push_back(n_args == 0 ? '?' : '&');
	append(key, path);
	path.push_back('=');
	append(value, path);

	n_args++;
}

void http::add_argument(const string& key, const string& value) {

	path.push_back(n_args == 0 ? '?' : '&');
	append(key, path);
	path.push_back('=');
	append(value, path);

	n_args++;
}

void http::append(const string& s, buffer& b) {
	copy(s.begin(), s.end(), back_inserter(b));
}

void http::append(const buffer& s, buffer& b) {
	copy(s.begin(), s.end(), back_inserter(b));
}

buffer http::get() {

	buffer payload;
	const string CRLF = "\r\n";

	append("GET ", payload);
	append(path, payload);
	append(" HTTP/1.1", payload);

	append(CRLF, payload);

	append("Host: ", payload);
	append(url.host, payload);
	append(":", payload);
	append(to_string(url.port), payload);

	append(CRLF, payload);

	append("User-Agent: sacha_torrent/1.0", payload);

	append(CRLF, payload);

	append("Accept: */*", payload);

	append(CRLF, payload);

	socket.send(payload);
	return socket.receive();
}

// GET /search?q=test HTTP/1.1\r\n
// Host: www.bing.com\r\n
// User-Agent: sacha_torrent/1.0\r\n
// Accept: */*\r\n
