#ifndef BUFFER_H
#define BUFFER_H

#include <vector>

typedef std::vector<unsigned char> buffer;
unsigned int getBE16(const buffer& b, buffer::size_type idx);
unsigned int getBE32(const buffer& b, buffer::size_type idx);
buffer setBE32(unsigned int n, buffer& b, const buffer::size_type idx);
buffer setBE16(unsigned int n, buffer& b, const buffer::size_type idx);
void print(const buffer& b);

#endif // BUFFER_H
