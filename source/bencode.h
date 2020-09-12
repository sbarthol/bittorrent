#ifndef BENCODE_H
#define BENCODE_H

#include <vector>
#include <stdexcept>
#include <map>
#include <any>
#include "buffer.h"

class bencode {

public:
	enum type {bs, i, l, d};

	struct item {

		std::any data;
		type t;

		bool operator==(const item& other) const;
		bool operator<(const item& other) const;
		item operator[](const std::string& key) const;
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
	static void print(const item& e);
};

#endif // BENCODE_H
