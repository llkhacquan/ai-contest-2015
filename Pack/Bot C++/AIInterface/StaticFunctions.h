#include "..\AI_Template\mydefine.h"
#include "Pos2D.h"
#include "BiconnectedComponents.h"

#ifndef STATIC_FUNCTIONS
#define STATIC_FUNCTIONS

void setBit(TBlock &number, int iBit);
void clearBit(TBlock &number, int iBit);
void toggleBit(TBlock &number, int iBit);
bool getBit(const TBlock &number, int iBit);
void changeBit(TBlock &number, int iBit, int value);

void setBit(unsigned char &number, int iBit);
void clearBit(unsigned char &number, int iBit);
void toggleBit(unsigned char &number, int iBit);
bool getBit(const unsigned char &number, int iBit);
void changeBit(unsigned char &number, int iBit, int value);

TBlock ipowBase2(int exp);
int findCode(TBlock block);

TMove getOpositeDirection(const TMove direction);

TBlock getBlock(TBlock const board[], const Pos1D pos);
TBlock getBlock(TBlock const board[], const int x, const int y);
TBlock getBlock(TBlock const board[], const Pos2D &pos);

bool setBlock(TBlock board[], const int x, const int y, const TBlock value);
bool setBlock(TBlock board[], const Pos1D pos, const TBlock value);
bool setBlock(TBlock board[], const Pos2D &pos, const TBlock value);

vector<TMove> getAvailableMoves(const TBlock board[], const Pos2D &pos, bool *output = NULL);

// this method return a longest path from pos by brute force
vector<TMove> &getALongestPath(TBlock const boardData[], const Pos2D &pos);

// this method does modify board but restore the original board in the end
void findLongestPath(TBlock board[], Pos2D& pos, vector<TMove> &c, vector<TMove> &l);

bool move(TBlock _board[], const Pos2D &currentPos, const TMove direction, const bool backMode = false);

vector<TMove> &findShortestPath(const TBlock _board[], const Pos2D& start, const Pos2D& end);

int fillDistance(TBlock _board[121], const Pos2D &pos);

bool isIsolated(const TBlock _boardData[], const Pos2D &pos1, const Pos2D &pos2);

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
int lengthWhenTryToReachBattleFields(const TBlock board[], const TBlock dBoard[], const TBlock filledBoard[], const Pos2D& _p);

#endif // STATIC_FUNCTIONS
