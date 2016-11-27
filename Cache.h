#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <iostream>
#include <random>
#include <memory>
#include <algorithm>

class Cache {
public:
	Cache(int size) : size(size) {}
	virtual int run(std::vector<int>) = 0;
	virtual std::string name() = 0;

protected:
	int size;
};

class Fifo : public Cache {
public:
	Fifo(int size) : Cache(size) {}
	int run(std::vector<int> data) override;
	std::string name() override { return std::string("FIFO"); }
};

class SecondChance : public Cache {
public:
	SecondChance(int size) : Cache(size) {}
	int run(std::vector<int>) override;
	std::string name() override { return std::string("SC"); }
};
class LRU : public Cache {
public:
	LRU(int size) : Cache(size) {}
	int run(std::vector<int>) override;
	std::string name() override { return std::string("LRU"); }
	static unsigned long timeStamp;
};

class MRU : public Cache {
public:
	MRU(int size) : Cache(size) {}
	int run(std::vector<int> data) override;
	std::string name() override { return std::string("MRU"); }
};

std::vector<int> make_list(int n, int numOfPages);

#endif