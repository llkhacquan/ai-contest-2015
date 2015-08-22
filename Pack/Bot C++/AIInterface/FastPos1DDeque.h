#pragma once
#include "mydefine.h"
class CFastPos1DDeque
{
public:
	static const int CAPACITY = 1024;
	Pos1D data[CAPACITY];
	int start;
	int end;

	inline void reOrder()
	{
		static Pos1D data_[CAPACITY];
		memcpy(data_, data + start, sizeof(Pos1D)*(end - start));
		memcpy(data, data_, sizeof(Pos1D)*(end - start));
	}

	inline size_t size() const
	{
		assert(start >= 0 && end >= start && end <= CAPACITY);
		return end - start;
	}

	inline void clear()
	{
		assert(start >= 0 && end >= start && end <= CAPACITY);
		start = end = 0;
	}

	inline void push_back(const Pos1D u)
	{
		assert(start >= 0 && end >= start && end <= CAPACITY);
		data[end] = u;
		end++;
	}

	inline Pos1D front() const
	{
		assert(start >= 0 && end > start && end <= CAPACITY);
		return data[start];
	}

	inline Pos1D back() const {
		assert(start >= 0 && end > start && end <= CAPACITY);
		return data[end - 1];
	}

	inline Pos1D pop_front()
	{
		assert(start >= 0 && end > start && end <= CAPACITY);
		Pos1D resutl = data[start];
		start++;
		if (start == end)
			start = end = 0;
		return resutl;
	}

	inline Pos1D pop_back(){
		assert(start >= 0 && end > start && end <= CAPACITY);
		Pos1D result = data[end - 1];
		end--;
		if (start == end)
			start = end = 0;
		return result;
	}

	~CFastPos1DDeque()
	{

	}

	CFastPos1DDeque()
	{
		start = end = 0;
	}

	inline const Pos1D &operator[](const int i) const
	{
		assert(start >= 0 && end >= start && end <= CAPACITY);
		assert(i >= 0 && i + start < end);
		return *(data + i + start);
	}

	inline bool empty()
	{
		return end == start;
	}

	CFastPos1DDeque& operator=(const CFastPos1DDeque &deque){
		start = deque.start;
		end = deque.end;
		memcpy(data, deque.data, sizeof(data[0])*CAPACITY);
		return *this;
	}
};

