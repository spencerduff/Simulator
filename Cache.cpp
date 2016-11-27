#include "Cache.h"

unsigned long LRU::timeStamp = 0;

int Fifo::run(std::vector<int> data){
	std::vector<int> c(size);
	std::generate(c.begin(), c.end(), [] { return -1; });
	int miss = 0;
	int cur = 0;
	for (auto request : data) {
		if (std::find(c.begin(), c.end(), request) == c.end()) {
			++miss;
			c[cur] = request;
			cur = (cur + 1) % size;
		}
	}
	return miss;
}

int MRU::run(std::vector<int> data){
	std::vector<int> c(size);
	std::generate(c.begin(), c.end(), [] { return -1; });
	int miss = 0;
	std::vector<int>::iterator last;
	for (auto request : data) {
		auto cur = std::find(c.begin(), c.end(), request);
		if (cur == c.end()) {
			++miss;
			auto pos = std::find(c.begin(), c.end(), -1);
			if (pos != c.end()) {
				*pos = request;
				last = pos;
			}
			else {
				*last = request;
			}
		}
		else {
			last = cur;
		}
	}
	return miss;
}

int LRU::run(std::vector<int> data){
	std::vector<int> c(size);
	std::vector<int> timeStamps(size);
	std::generate(c.begin(), c.end(), [] { return -1; });
	std::generate(timeStamps.begin(), timeStamps.end(), [] { return 0; });
	int miss = 0;
	for (auto request : data) {
		auto cur = std::find(c.begin(), c.end(), request);
		if (cur == c.end()) {
			++miss;
			auto pos = std::find(c.begin(), c.end(), -1);
			if (pos != c.end()) {
				*pos = request;
				timeStamps[pos - c.begin()] = ++timeStamp;
			}
			else {
				auto toReplace = std::min_element(timeStamps.begin(), timeStamps.end());
				c[toReplace - timeStamps.begin()] = request;
				timeStamps[toReplace - timeStamps.begin()] = ++timeStamp;
			}
		}
		else {
			timeStamps[cur - c.begin()] = ++timeStamp;
		}
	}
	return miss;
}

int SecondChance::run(std::vector<int> data){
	std::vector<int> c(size);
	std::vector<bool> b(size);
	std::generate(c.begin(), c.end(), [] { return -1; });
	std::generate(b.begin(), b.end(), [] { return false; });
	int miss = 0;
	int cur = 0;
	for (auto request : data) {
		auto result = std::find(c.begin(), c.end(), request);
		if (result == c.end()) {
			++miss;
			bool added = false;
			while (!added){
				if (b[cur] == 1){
					b[cur] = 0;
					cur = (cur + 1) % size;
				}
				else{
					c[cur] = request;
					added = true;
				}
			}
			cur = (cur + 1) % size;
		}
		else{
			b[result - c.begin()] = true;
		}
	}
	return miss;
}

std::vector<int> make_list(int n, int numOfPages) {
	std::vector<int> result(n);
	static std::random_device rd;
	static std::mt19937 mt(rd());
	std::uniform_int_distribution<> dist(0, numOfPages);
	std::generate(result.begin(), result.end(), [&]() { return dist(mt); });
	return result;
}