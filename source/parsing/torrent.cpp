#include "parsing/torrent.h"
#include <iterator>
#include <fstream>
#include "parsing/bencode.h"
#include <openssl/sha.h>

using namespace std;

torrent::torrent(const string& filename) {

	buffer buff = get_bytes(filename);
	bencode::item item = bencode::parse(buff);

	this->url = item.get_string("announce");
	this->info_hash = get_hash_info(item);
	this->length = get_length(item);

	bencode::item info = item.get_item("info");

	this->piece_length = info.get_int("piece length");

	buffer hashes = info.get_buffer("pieces");

	const int SHA1_SIZE = 20;
	this->pieces = hashes.size() / SHA1_SIZE;

	buffer name_bytes = info.get_buffer("name");
	this->name = string(name_bytes.begin(), name_bytes.end());
}

buffer torrent::get_bytes(const string& filename) {

	ifstream is;
	buffer bytes;

	is.open(filename, ios::binary);
	if(is.fail()) {
		throw runtime_error("Opening file failed");
	}

	is.seekg(0, ios::end);
	size_t filesize=is.tellg();
	is.seekg(0, ios::beg);

	bytes.reserve(filesize);
	bytes.assign(istreambuf_iterator<char>(is),
	                     istreambuf_iterator<char>());

	return bytes;
}

buffer torrent::get_hash_info(const bencode::item& item) {

	bencode::item info = item.get_item("info");
	buffer encoded = bencode::encode(info);

	const int SIZE_SHA1 = 20;

	unsigned char buff[SIZE_SHA1];
	SHA1(encoded.data(), encoded.size(), buff);

	return buffer(buff,buff+SIZE_SHA1);
}

long long torrent::get_length(const bencode::item& item) {

	long long length = 0;

	bencode::item info = item.get_item("info");
	if(info.key_present("files")) {

		vector<bencode::item> files = info.get_list("files");
		for (bencode::item& file:files) {
			length += file.get_int("length");
		}

	} else {

		length = info.get_int("length");
	}

	return length;
}

unsigned int torrent::get_piece_length(unsigned int piece) {

	return piece == pieces - 1 ? 
		(length % piece_length == 0 ? piece_length : length % piece_length) : 
		piece_length;
}

unsigned int torrent::get_n_blocks(unsigned int piece) {

	return (get_piece_length(piece) + BLOCK_SIZE - 1) / BLOCK_SIZE;
}

unsigned int torrent::get_block_length(unsigned int piece, unsigned int block_index) {

	return block_index == get_n_blocks(piece) - 1 ? 
			(get_piece_length(piece) % BLOCK_SIZE == 0 ? 
					BLOCK_SIZE : 
					get_piece_length(piece) % BLOCK_SIZE) : 
			BLOCK_SIZE;
}


