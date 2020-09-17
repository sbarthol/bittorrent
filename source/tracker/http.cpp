#include "tracker/http.h"
#include <algorithm>
#include <string>
#include <stdexcept>
#include <ctype.h>

using namespace std;

http::http(const url_t& url): 
	socket(url.host, url.port), url(url), n_args(0) {
		
		if (url.protocol != url_t::HTTP) {
			throw runtime_error("url is not a http url");
		}

		this->url = url;
		this->path = buffer(url.path.begin(), url.path.end());
}

void http::add_argument(const string& key, const buffer& value) {

	path.push_back(n_args == 0 ? '?' : '&');
	append(key, path);
	path.push_back('=');
	append(urlencode(value), path);

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

string http::urlencode(const buffer &b) {

    static const char lookup[]= "0123456789abcdef";
    string ans;

    for(int i=0; i<b.size(); i++) {

        char c = b[i];
        if (isalnum(c) || c=='-' || c=='_' || c=='.' || c=='~') {
        	ans.push_back(c);

        }else{

        	ans.push_back('%');
        	ans.push_back(lookup[(c&0xF0)>>4]);
        	ans.push_back(lookup[(c&0x0F)]);
        }
    }

    return ans;
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
	append(CRLF, payload);

	socket.send(payload);
	buffer response = socket.receive();

	buffer::size_type pos_body = -1;
	for(buffer::size_type i = 0; i + 3 < response.size(); i++) {

		if(response[i] == '\r' 
			&& response[i+1] == '\n' 
			&& response[i+2] == '\r' 
			&& response[i+3] == '\n') {

			pos_body = i + 4;
		}
	}

	if(pos_body == -1) return buffer();
	return buffer(response.begin() + pos_body, response.end());
}

// GET /search?q=test HTTP/1.1\r\n
// Host: www.bing.com\r\n
// User-Agent: sacha_torrent/1.0\r\n
// Accept: */*\r\n\r\n
