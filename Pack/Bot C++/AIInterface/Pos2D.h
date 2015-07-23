#pragma once
#include "..\AI_Template\mydefine.h"
#include "..\AI_Template\include/ai/AI.h"

class Pos2D
{
public:
	int x;
	int y;

	Pos2D();
	Pos2D(const int pos);
	Pos2D(const int _x, const int _y);
	Pos2D operator =(const Pos2D &pos);
	Pos2D operator =(const Position &pos);
	bool operator==(const Pos2D &pos) const;
	const Pos2D move(const int direction) const;
	int to1D() const;

	operator int()const { return to1D(); }
};

