#ifndef HTTP_H
#define HTTP_H

#include "parsing/buffer.h"
#include "tracker/tcp.h"
#include "tracker/url.h"
#include <string>

class http {

private:
	tcp socket;
	url_t url;
	buffer path;
	int n_args;

	void append(const std::string& s, buffer& b);
	void append(const buffer& s, buffer& b);

public:
	http(const url_t& url);
	std::string urlencode(const buffer& b);
	void add_argument(const std::string& key, const std::string& value);
	void add_argument(const std::string& key, const buffer& value);
	buffer get();
};

#endif // HTTP_H
