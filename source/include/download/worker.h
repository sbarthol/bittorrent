#ifndef WORKER_H
#define WORKER_H

#include <string>
#include <fstream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "parsing/buffer.h"

class writer {

private:
	std::ofstream out;
	bool finito;
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
