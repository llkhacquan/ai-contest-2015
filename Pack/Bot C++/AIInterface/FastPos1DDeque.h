#pragma once
#include "mydefine.h"
class CFastPos1DDeque
{
public:
	static const int CAPACITY = 1024;
private:
	Pos1D data[CAPACITY];
	int start;
	int end;
public:

	CFastPos1DDeque();
	~CFastPos1DDeque();

	Pos1D pop_front();

	Pos1D front()const;

	void push_back(const Pos1D u);

	void clear();

	size_t size();

	void reOrder();

	bool empty(){
		return end == start;
	}

	const Pos1D operator[](const int i) const;

	Pos1D back() const;
	Pos1D pop_back();
};

