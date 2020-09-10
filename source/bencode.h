#ifndef BENCODE_H
#define BENCODE_H

#include <vector>
#include <stdexcept>
#include <map>

class bencode {

public:
	enum Type { bs, i, l, d };
	typedef std::pair<std::vector<char>, Type> Element;

private:
	typedef std::vector<char>::size_type size;
	static Element parse_byte_string(std::vector<char> const& s, size& idx);

public:

	typedef int integer;
	typedef std::vector<char> byte_string;
	typedef std::vector<Element> list;
	typedef std::map<Element, Element> dictionary;

	struct invalid_bencode: std::invalid_argument {

		invalid_bencode(): std::invalid_argument("") {}
		invalid_bencode(const char* what): std::invalid_argument(what) {}
	};

	static Element parse(std::vector<char> const& s);
};

#endif // BENCODE_H