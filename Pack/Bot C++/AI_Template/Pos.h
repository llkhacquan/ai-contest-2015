#pragma once
#include "mydefine.h"
#include "ai/AI.h"
class CPos
{
public:
	int x;
	int y;

	CPos();
	CPos(const int pos);
	CPos(const int _x, const int _y);
	CPos operator =(const CPos &pos);
	CPos operator =(const Position &pos);
	bool operator==(const CPos &pos) const;
	const CPos move(const int direction) const;
	int to1D() const;
};

