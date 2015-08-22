#pragma once
#include "mydefine.h"

class CBigDeque
{
public:
	static const int CAPACITY = 1024;
	TPos data[CAPACITY];
	int start;
	int end;

	inline void reOrder()
	{
		static TPos data_[CAPACITY];
		memcpy(data_, data + start, sizeof(TPos)*(end - start));
		memcpy(data, data_, sizeof(TPos)*(end - start));
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

	inline void push_back(const TPos u)
	{
		assert(start >= 0 && end >= start && end <= CAPACITY);
		data[end] = u;
		end++;
	}

	inline TPos front() const
	{
		assert(start >= 0 && end > start && end <= CAPACITY);
		return data[start];
	}

	inline TPos back() const {
		assert(start >= 0 && end > start && end <= CAPACITY);
		return data[end - 1];
	}

	inline TPos pop_front()
	{
		assert(start >= 0 && end > start && end <= CAPACITY);
		TPos resutl = data[start];
		start++;
		if (start == end)
			start = end = 0;
		return resutl;
	}

	inline TPos pop_back(){
		assert(start >= 0 && end > start && end <= CAPACITY);
		TPos result = data[end - 1];
		end--;
		if (start == end)
			start = end = 0;
		return result;
	}

	~CBigDeque()
	{

	}

	CBigDeque()
	{
		start = end = 0;
	}

	inline const TPos &operator[](const int i) const
	{
		assert(start >= 0 && end >= start && end <= CAPACITY);
		assert(i >= 0 && i + start < end);
		return *(data + i + start);
	}

	inline bool empty()
	{
		return end == start;
	}

	CBigDeque& operator=(const CBigDeque &deque){
		start = deque.start;
		end = deque.end;
		memcpy(data, deque.data, sizeof(data[0])*CAPACITY);
		return *this;
	}
};

