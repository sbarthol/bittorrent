#include "buffer.h"
#include <stdexcept>
#include <iostream>

using namespace std;

unsigned int getBE16(const buffer& b, buffer::size_type idx) {

	if(idx+1 >= b.size()) throw runtime_error("index out of bounds");

	return b[idx] * 256 + b[idx+1];
}

unsigned int getBE32(const buffer& b, buffer::size_type idx) {

	if(idx+3 >= b.size()) throw runtime_error("index out of bounds");

	unsigned int ans = 0;
	for(int i=0;i<4;i++) {
		ans *= 256;
		ans += b[idx+i];
	}
	return ans;
}

buffer setBE32(unsigned int n, buffer& b, const buffer::size_type idx) {

	if(idx+3 >= b.size()) throw runtime_error("index out of bounds");

	for(int i=0;i<4;i++) {

		b[idx+3-i] = n % 256;
		n /= 256;
	}
}

buffer setBE16(unsigned int n, buffer& b, const buffer::size_type idx) {

	if(idx+1 >= b.size()) throw runtime_error("index out of bounds");

	b[idx+1] = n % 256;
	b[idx] = (n / 256) % 256;
}

void print(const buffer& b) {

	for(unsigned char c:b){
		if(32<=c&&c<128){
			cout<<c;
		}else{
			cout<<"0x"<<hex<<(unsigned int)c<<" "<<dec;
		}
	}
	cout<<endl;
}
