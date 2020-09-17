#ifndef PEER_ID_H
#define PEER_ID_H

#include "parsing/buffer.h"
#include <string>

class peer_id {

private:
	static const int PEER_ID_SIZE = 20;
	static buffer id;

public:
	peer_id() = delete;
	static buffer get();
	static void generate(const std::string& pref = "-SA0001-");
};

#endif // PEER_ID_H
