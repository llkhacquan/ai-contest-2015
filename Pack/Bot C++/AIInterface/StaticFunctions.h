#pragma once

#include "mydefine.h"
#include "BiconnectedComponents.h"

inline TBlock iPow2(const int &i){
	return 1i64 << i;
}

inline void setBit(TBlock &number, int iBit)
{
	number |= 1i64 << iBit;
	assert(number >= 0);
}
inline void clearBit(TBlock &number, int iBit)
{
	number &= ~(1i64 << iBit);
	assert(number >= 0);
}
inline void toggleBit(TBlock &number, int iBit)
{
	number ^= 1i64 << iBit;
	assert(number >= 0);
}
inline bool getBit(const TBlock &number, int iBit)
{
	return ((number >> iBit) & 1i64);
}
inline void changeBit(TBlock &number, int iBit, int value)
{
	number ^= (-value ^ number) & (1i64 << iBit);
}

inline void setBit(unsigned char &number, int iBit)
{
	number |= 1i8 << iBit;
	assert(number >= 0);
}
inline void clearBit(unsigned char &number, int iBit)
{
	number &= ~(1i8 << iBit);
	assert(number >= 0);
}
inline void toggleBit(unsigned char &number, int iBit)
{
	number ^= 1i8 << iBit;
	assert(number >= 0);
}
inline bool getBit(const unsigned char &number, int iBit)
{
	return ((number >> iBit) & 1i8);
}
inline void changeBit(unsigned char &number, int iBit, int value)
{
	number ^= (-value ^ number) & (1i8 << iBit);
}

inline TMove getOpositeDirection(const TMove direction)
{
	switch (direction)
	{
	case DIRECTION_DOWN:
		return DIRECTION_UP;
	case DIRECTION_UP:
		return DIRECTION_DOWN;
	case DIRECTION_LEFT:
		return DIRECTION_RIGHT;
	case DIRECTION_RIGHT:
		return DIRECTION_LEFT;
	default:
		assert(false);
		return 0;
	}
}

vector<TMove> getAvailableMoves(const TBlock board[], const TPos &pos, bool *output = NULL);

// this method return a longest path from pos by brute force
vector<TMove> &getALongestPath(TBlock const boardData[], const TPos &pos);

// this method does modify board but restore the original board in the end
void findLongestPath(TBlock board[], TPos& pos, vector<TMove> &c, vector<TMove> &l);

int exploreMapWithoutRecursion(const TBlock _board[], const int overEstimatedResult, const TPos& _pos);

int exploreMapWithoutRecursion(const TBlock _board[], const int overEstimatedResult, const TPos& _pos, const TPos &_endPos);

int exploreMapWithoutRecursion(bool visited[], const int overEstimatedResult, const TPos &u, const TPos&v);

int exploreMapWithoutRecursion(bool visited[], const int overEstimatedResult, const TPos &u);

bool move(TBlock _board[], const TPos &currentPos, const TMove direction, const bool backMode = false);

int fillDistance(TBlock _board[121], const TPos &pos);

int distanceBetween2Pos(const TPos &p1, const TPos &p2);

bool isIsolated(const TBlock _boardData[], const TPos &pos1, const TPos &pos2);

void printBoard(const TBlock board[], const bool special);

void createNewBoard(TBlock board[], int nObstacle);
#ifdef OPENCV
static Mat empty;
static Mat obstacle;
static Mat player_1;
static Mat player_1_trail;
static Mat player_2;
static Mat player_2_trail;
cv::Mat toImage(TBlock board[], bool special = false);

void setupImage();
#endif // OPENCV
