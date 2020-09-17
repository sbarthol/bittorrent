#ifndef BENCODE_H
#define BENCODE_H

#include <vector>
#include <stdexcept>
#include <map>
#include <any>
#include "parsing/buffer.h"
#include <string>

class bencode {

public:
	enum type {bs, i, l, d};

	struct item {

		std::any data;
		type t;

		bool operator==(const item& other) const;
		bool operator<(const item& other) const;

		buffer get_buffer(const char* key) const;
		long long get_int(const char* key) const;
		std::string get_string(const char* key) const;
		item get_item(const char* key) const;
		std::vector<bencode::item> get_list(const char* key) const;
		bool key_present(const char* key) const;
	private:
		item get(const item& key) const;
	};

private:
	typedef buffer::size_type size;
	static void next(bencode::item& e, const buffer& s, size& idx);
	static item parse_byte_string(buffer const& s, size& idx);
	static item parse_integer(buffer const& s, size& idx);
	static item parse_list(buffer const& s, size& idx);
	static item parse_dictionary(buffer const& s, size& idx);

public:

	bencode() = delete;

	struct invalid_bencode: std::invalid_argument {

		invalid_bencode(): std::invalid_argument("") {}
		invalid_bencode(const char* what): std::invalid_argument(what) {}
	};

	static item parse(buffer const& s);
	static buffer encode(item const& e);
	static void print(const item& e);
};

#endif // BENCODE_H
