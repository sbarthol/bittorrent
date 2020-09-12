#ifndef URL_H
#define URL_H

#include "bencode.h"
#include <string>

struct url {

	std::string host;
	int port;

	url(const bencode::item& file);
};

#endif // URL_H
