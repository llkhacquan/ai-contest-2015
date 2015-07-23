#include "Pos2D.h"

Pos2D::Pos2D() : x(0), y(0)
{

}

Pos2D::Pos2D(const int _x, const int _y) : x(_x), y(_y)
{

}

Pos2D::Pos2D(const int pos)
{
	assert(pos >= 0 && pos < BOARD_SIZE);
	x = pos % MAP_SIZE;
	y = pos / MAP_SIZE;
}

const Pos2D Pos2D::move(const int direction) const
{
	switch (direction){
	case DIRECTION_DOWN:
		return Pos2D(x, y + 1);
	case DIRECTION_UP:
		return Pos2D(x, y - 1);
	case DIRECTION_LEFT:
		return Pos2D(x - 1, y);
	case DIRECTION_RIGHT:
		return Pos2D(x + 1, y);
	default:
		assert(false);
		return *this;
	}
}

bool Pos2D::operator==(const Pos2D &pos) const
{
	return x == pos.x && y == pos.y;
}

Pos2D Pos2D::operator=(const Pos2D &pos)
{
	x = pos.x; y = pos.y; return *this;
}

Pos2D Pos2D::operator=(const Position &pos)
{
	x = pos.x; y = pos.y; return *this;
}

int Pos2D::to1D() const
{
	return CC(x, y);
}
