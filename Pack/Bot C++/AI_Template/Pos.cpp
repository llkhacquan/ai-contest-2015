#include "Pos.h"

CPos::CPos() : x(0), y(0)
{

}

CPos::CPos(const int _x, const int _y) : x(_x), y(_y)
{

}

CPos::CPos(const int pos)
{
	assert(pos >= 0 && pos < BOARD_SIZE);
	x = pos % MAP_SIZE;
	y = pos / MAP_SIZE;
}

const CPos CPos::move(const int direction) const
{
	switch (direction){
	case DIRECTION_DOWN:
		return CPos(x, y + 1);
	case DIRECTION_UP:
		return CPos(x, y - 1);
	case DIRECTION_LEFT:
		return CPos(x - 1, y);
	case DIRECTION_RIGHT:
		return CPos(x + 1, y);
	default:
		assert(false);
		return *this;
	}
}

bool CPos::operator==(const CPos &pos) const
{
	return x == pos.x && y == pos.y;
}

CPos CPos::operator=(const CPos &pos)
{
	x = pos.x; y = pos.y; return *this;
}

CPos CPos::operator=(const Position &pos)
{
	x = pos.x; y = pos.y; return *this;
}

int CPos::to1D() const
{
	return CC(x, y);
}
