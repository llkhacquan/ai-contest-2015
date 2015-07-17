#ifndef MY_DEFINE

#define MY_DEFINE
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <queue>
#include <stack>
#include <fstream>
#include <ctime>
#include <cassert>
#include <algorithm>
#include <set>


#include "include\ai\defines.h"
#include "include\ai\AI.h"



struct Position;

typedef int Player;
typedef int Direction;
typedef int Vertex;

using namespace std;

#define CC(x,y)	CONVERT_COORD(x,y)
#define BOARD_SIZE (MAP_SIZE*MAP_SIZE)
#define MAX_POINTS	1000
#define MIN_POINTS	0
#define DEFAULT_DEPTH 8
#define BLOCK_ENEMY_AREA -2

#define MAXIMUM_NUMBER_OF_AREAS		30

/// GRAPHIC
#ifdef OPENCV
#include <opencv2\highgui\highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;
#define PIXEL_PER_BLOCK 50
#define HALF_PIXEL_PER_LINE	1
#define FPS				60
#endif // OPENCV


// SHOW DEBUG INFORMATION
#define SHOW_DEBUG_INFORMATION	1
#ifndef _DEBUG
#undef SHOW_DEBUG_INFORMATION
#define  SHOW_DEBUG_INFORMATION 0
#endif // _DEBUG

#if SHOW_DEBUG_INFORMATION 
#define DEBUG(x);	x;
#else
#define DEBUG(x);	
#endif



static const int SPECIAL_BIT = 30;
#define SPECIAL_BLOCK (1 << SPECIAL_BIT)


static inline void setBit(int &number, int iBit){
	number |= 1 << iBit;
	assert(number >= 0);
}
static inline void clearBit(int &number, int iBit){
	number &= ~(1 << iBit);
	assert(number >= 0);
}
static inline void toggleBit(int &number, int iBit){
	number ^= 1 << iBit;
	assert(number >= 0);
}
static inline bool getBit(const int &number, int iBit){
	return ((number >> iBit) & 1);
}
static inline void changeBit(int &number, int iBit, int value){
	number ^= (-value ^ number) & (1 << iBit);
}
static inline int ipowBase2(int exp)
{
	return 1 << exp;
}

static inline int findCode(int block){
	int t = block - SPECIAL_BLOCK;
	int code = 0;
	while (((t & 1) == 0) && (t > 0)){
		t >>= 1;
		code++;
	}
	return code;
}

#endif


