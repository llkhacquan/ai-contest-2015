#include "..\AI_Template\mydefine.h"
#include "Pos2D.h"

#ifndef STATIC_FUNCTIONS
#define STATIC_FUNCTIONS

void setBit(int &number, int iBit);
void clearBit(int &number, int iBit);
void toggleBit(int &number, int iBit);
bool getBit(const int &number, int iBit);
void changeBit(int &number, int iBit, int value);
int ipowBase2(int exp);
int findCode(int block);

TMove getOpositeDirection(const TMove direction);

int getBlock(int const board[], const Pos1D pos);
int getBlock(int const board[], const int x, const int y);
int getBlock(int const board[], const Pos2D &pos);

bool setBlock(int board[], const int x, const int y, const int value);
bool setBlock(int board[], const Pos1D pos, const int value);
bool setBlock(int board[], const Pos2D &pos, const int value);

vector<TMove> getAvailableMoves(const int board[], const Pos2D &pos, bool *output = NULL);

// this method return a longest path from pos by brute force
vector<TMove>& getALongestPath(int const boardData[], const Pos2D &pos);

// this method does modify board but restore the original board in the end
void findLongestPath(int board[], Pos2D& pos, vector<TMove> &c, vector<TMove> &l);

bool move(int _board[], const Pos2D &currentPos, const TMove direction, const bool backMode = false);

vector<TMove>& findShortestPath(const int _board[], const Pos2D& start, const Pos2D& end);

int fillDistance(int _board[121], const Pos2D &pos);

int isIsolated(const int _boardData[], const Pos2D &pos1, const Pos2D &pos2);

void printBoard(const int board[], const bool special);

void createNewBoard(int board[], int nObstacle);
#ifdef OPENCV
static Mat empty;
static Mat obstacle;
static Mat player_1;
static Mat player_1_trail;
static Mat player_2;
static Mat player_2_trail;
cv::Mat toImage(int board[], bool special = false);

void setupImage();
#endif // OPENCV


#endif // STATIC_FUNCTIONS
