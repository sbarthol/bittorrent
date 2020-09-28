#ifndef WORKER_H
#define WORKER_H

#include <string>
#include <fstream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "parsing/buffer.h"
#include <string>

class speed {

private:
	std::atomic<unsigned int> bytes, received;
	std::atomic<bool> finito;
	std::thread t;
	
	unsigned int prev;
	long long total;
	const double e_factor = 0.6321205588; // 1 - 1/e
	const int delay = 500;
	int mod;

	void human_readable(unsigned int b); 

public:
	speed(): total(0), bytes(0), finito(false), prev(0), mod(0) {}
	void set_total(long long t);
	void start();
	void stop();
	void add(unsigned int b);
	void draw(double progress, unsigned int speed);
};

class writer {

private:
	std::ofstream out;
	std::atomic<bool> finito;
	std::mutex mx;
	std::condition_variable cv;
	std::thread t;

	struct job {
		buffer b;
		int offset;
		job(buffer b, int offset): b(b), offset(offset) {};
	};

	std::queue<job> q;

public:
	writer(std::string filename): out(filename, std::ios::binary), finito(false) {}
	void start();
	void stop();
	void add(buffer& b, int offset);
};

#endif // WORKER_H
