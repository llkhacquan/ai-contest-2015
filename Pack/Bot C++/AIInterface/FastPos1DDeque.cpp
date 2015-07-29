#include "FastPos1DDeque.h"


void CFastPos1DDeque::reOrder()
{
	static Pos1D data_[CAPACITY];
	memcpy(data_, data + start, sizeof(Pos1D)*(end - start));
	memcpy(data, data_, sizeof(Pos1D)*(end - start));
}

size_t CFastPos1DDeque::size()
{
	assert(start >= 0 && end >= start && end <= CAPACITY);
	return end - start;
}

void CFastPos1DDeque::clear()
{
	assert(start >= 0 && end >= start && end <= CAPACITY);
	start = end = 0;
}

void CFastPos1DDeque::push_back(const Pos1D u)
{
	assert(start >= 0 && end >= start && end <= CAPACITY);
	data[end] = u;
	end++;
}

Pos1D CFastPos1DDeque::front() const
{
	assert(start >= 0 && end > start && end <= CAPACITY);
	return data[start];
}

Pos1D CFastPos1DDeque::back() const {
	assert(start >= 0 && end > start && end <= CAPACITY);
	return data[end - 1];
}

Pos1D CFastPos1DDeque::pop_front()
{
	assert(start >= 0 && end > start && end <= CAPACITY);
	Pos1D resutl = data[start];
	start++;
	if (start == end)
		start = end = 0;
	return resutl;
}

Pos1D CFastPos1DDeque::pop_back(){
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

const Pos1D CFastPos1DDeque::operator[](const int i) const
{
	assert(start >= 0 && end >= start && end <= CAPACITY);
	assert(i >= 0 && i + start < end);
	return *(data + i + start);
}
