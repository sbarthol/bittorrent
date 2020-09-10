#ifndef BENCODE_H
#define BENCODE_H

#include <vector>
#include <stdexcept>
#include <map>
#include <any>

class bencode {

public:
	enum type {bs, i, l, d};

	struct Element {

		std::any data;
		type t;

		bool operator==(const Element& other) const;
		bool operator<(const Element& other) const;
	};

private:
	typedef std::vector<char>::size_type size;
	static void next(bencode::Element& e, const std::vector<char>& s, size& idx);
	static Element parse_byte_string(std::vector<char> const& s, size& idx);
	static Element parse_integer(std::vector<char> const& s, size& idx);
	static Element parse_list(std::vector<char> const& s, size& idx);
	static Element parse_dictionary(std::vector<char> const& s, size& idx);

public:

	bencode() = delete;

	struct invalid_bencode: std::invalid_argument {

		invalid_bencode(): std::invalid_argument("") {}
		invalid_bencode(const char* what): std::invalid_argument(what) {}
	};

	static Element parse(std::vector<char> const& s);
	static void print(const Element& e);
};

#endif // BENCODE_H
