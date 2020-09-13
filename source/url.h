#ifndef URL_H
#define URL_H

#include <string>

struct url_t {

	std::string host;
	std::string path;
	int port;

	enum {UDP, TCP} protocol;

	url_t(const std::string& url);
	url_t() {};
};

#endif // URL_H
