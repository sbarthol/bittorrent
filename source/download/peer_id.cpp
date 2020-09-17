#include "download/peer_id.h"
#include <cstdlib>
#include <algorithm>

using namespace std;

buffer peer_id::id;

buffer peer_id::get() {

	return id;
}

void peer_id::generate(const std::string& pref) {

	static const char *alnum = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

	id = buffer();

	copy(pref.begin(),pref.end(),back_inserter(id));

	for(int i=0; i+pref.size() < PEER_ID_SIZE; i++) {
		id.push_back(alnum[rand() % 62]);
	}
}