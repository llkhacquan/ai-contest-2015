#include "mydefine.h"
#include "BiconnectedComponents.h"

#ifndef STATIC_FUNCTIONS
#define STATIC_FUNCTIONS

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

inline TBlock getBlock(TBlock const board[], const Pos1D pos)
{
	if (pos >= 0 && pos < BOARD_SIZE)
		return board[pos];
	else
		return BLOCK_OUT_OF_BOARD;
}

inline Pos1D move(const Pos1D &i, const TMove direction){
	int x = i % MAP_SIZE;
	int y = i / MAP_SIZE;
	switch (direction){
	case DIRECTION_DOWN:
		if (y + 1 < MAP_SIZE)
			return i + MAP_SIZE;
		else
			return -1;
	case DIRECTION_UP:
		if (y - 1 >= 0)
			return i - MAP_SIZE;
		else
			return -1;
	case DIRECTION_LEFT:
		if (x - 1 >= 0)
			return i - 1;
		else
			return -1;
	case DIRECTION_RIGHT:
		if (x + 1 < MAP_SIZE)
			return i + 1;
		else
			return -1;
	default:
		assert(false);
		return -1;
	}
}

inline bool setBlock(TBlock board[], const Pos1D pos, const TBlock value){
	if (pos >= 0 && pos < BOARD_SIZE)
	{
		board[pos] = value;
		return true;
	}
	else
		return false;
}

vector<TMove> getAvailableMoves(const TBlock board[], const Pos1D &pos, bool *output = NULL);

// this method return a longest path from pos by brute force
vector<TMove> &getALongestPath(TBlock const boardData[], const Pos1D &pos);

// this method does modify board but restore the original board in the end
void findLongestPath(TBlock board[], Pos1D& pos, vector<TMove> &c, vector<TMove> &l);

bool move(TBlock _board[], const Pos1D &currentPos, const TMove direction, const bool backMode = false);

vector<TMove> &findShortestPath(const TBlock _board[], const Pos1D& start, const Pos1D& end);

int fillDistance(TBlock _board[121], const Pos1D &pos);

bool isIsolated(const TBlock _boardData[], const Pos1D &pos1, const Pos1D &pos2);

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
void fillChamberWithBattleFields(const TBlock gatesBoard[], const TBlock board[], vector<Pos1D> enemies, TBlock oBoard[]);
int lengthWhenTryToReachBattleFields(const TBlock board[], const TBlock dBoard[], const TBlock filledBoard[], const Pos1D& _p);

#endif // STATIC_FUNCTIONS
