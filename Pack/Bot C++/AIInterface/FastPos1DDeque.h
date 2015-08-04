#pragma once
#include "mydefine.h"
class CFastPos1DDeque
{
public:
	static const int CAPACITY = 1024;
	Pos1D data[CAPACITY];
	int start;
	int end;

	inline void CFastPos1DDeque::reOrder()
	{
		static Pos1D data_[CAPACITY];
		memcpy(data_, data + start, sizeof(Pos1D)*(end - start));
		memcpy(data, data_, sizeof(Pos1D)*(end - start));
	}

	inline size_t CFastPos1DDeque::size() const
	{
		assert(start >= 0 && end >= start && end <= CAPACITY);
		return end - start;
	}

	inline void CFastPos1DDeque::clear()
	{
		assert(start >= 0 && end >= start && end <= CAPACITY);
		start = end = 0;
	}

	inline void CFastPos1DDeque::push_back(const Pos1D u)
	{
		assert(start >= 0 && end >= start && end <= CAPACITY);
		data[end] = u;
		end++;
	}

	inline Pos1D CFastPos1DDeque::front() const
	{
		assert(start >= 0 && end > start && end <= CAPACITY);
		return data[start];
	}

	inline Pos1D CFastPos1DDeque::back() const {
		assert(start >= 0 && end > start && end <= CAPACITY);
		return data[end - 1];
	}

	inline Pos1D CFastPos1DDeque::pop_front()
	{
		assert(start >= 0 && end > start && end <= CAPACITY);
		Pos1D resutl = data[start];
		start++;
		if (start == end)
			start = end = 0;
		return resutl;
	}

	inline Pos1D CFastPos1DDeque::pop_back(){
		assert(start >= 0 && end > start && end <= CAPACITY);
		Pos1D result = data[end - 1];
		end--;
		if (start == end)
			start = end = 0;
		return result;
	}

	CFastPos1DDeque::~CFastPos1DDeque()
	{

	}

	CFastPos1DDeque::CFastPos1DDeque()
	{
		start = end = 0;
	}

	inline const Pos1D &CFastPos1DDeque::operator[](const int i) const
	{
		assert(start >= 0 && end >= start && end <= CAPACITY);
		assert(i >= 0 && i + start < end);
		return *(data + i + start);
	}

	inline bool CFastPos1DDeque::empty()
	{
		return end == start;
	}

	CFastPos1DDeque& CFastPos1DDeque::operator=(const CFastPos1DDeque &deque){
		start = deque.start;
		end = deque.end;
		memcpy(data, deque.data, sizeof(data[0])*CAPACITY);
		return *this;
	}
};

