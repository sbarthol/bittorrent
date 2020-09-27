#include "download/worker.h"
#include <vector>

using namespace std;

void writer::start() {

	t = thread([this](){

		while(!finito) {

			unique_lock<mutex> lock(mx);
			cv.wait(lock, [this](){
				return !q.empty() || finito;
			});

			vector<job> vec;

			while(!q.empty()) {

				job j = q.front();
				q.pop();

				vec.push_back(move(j));
			}

			lock.unlock();

			for(job& j:vec) {

				out.seekp(j.offset, ios::beg);
				out.write(reinterpret_cast<char*>(j.b.data()), j.b.size());
			}
		}
	});
}

void writer::stop() {

	finito = true;
	cv.notify_one();
	t.join();
}

void writer::add(buffer& b, int offset) {

	unique_lock<mutex> lock(mx);
	q.push(job(move(b), offset));
	lock.unlock();
	cv.notify_one();
}


