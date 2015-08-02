#pragma once
#include "mydefine.h"
#include "StaticFunctions.h"


class CGameState
{
private:
	bitset<218> history;
	signed char historyLength;
public:
	CGameState();
	CGameState(const vector<TMove> &_history);
	~CGameState();

	signed char depth;
	int lowerbound;
	int upperBound;

	void set(const vector<TMove> &_history);

	void clear();

	unsigned int hash() const;

	bool isSet() const;

	bool operator==(const CGameState &state) const;

	bool operator!=(const CGameState &state) const;
};

