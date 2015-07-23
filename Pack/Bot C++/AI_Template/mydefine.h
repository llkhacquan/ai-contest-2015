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
#include <bitset>

#include "include\ai\defines.h"
#include "include\ai\AI.h"

#define BOT_ACTIVE	1
#define SHOW_DEBUG_INFORMATION	0
// 2600 ms is danger
#define DANGER_TIME		2700



#ifdef _DEBUG
#define MAX_DEPTH	 7
#else
#define MAX_DEPTH	 13
#endif

typedef long long TBlock;
typedef int TPlayer;
typedef int TMove;
typedef int Pos1D;
using namespace std;

#define POINTS 1000
#define CC(x,y)	CONVERT_COORD(x,y)
#define BOARD_SIZE (MAP_SIZE*MAP_SIZE)
#define MY_INFINITY	(MAX_POINTS + 10)
#define MAX_POINTS	 POINTS
#define MIN_POINTS	(-MAX_POINTS)
#define BLOCK_ENEMY_AREA -2

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
#ifndef _DEBUG
#undef SHOW_DEBUG_INFORMATION
#define  SHOW_DEBUG_INFORMATION 0
#endif // _DEBUG

#if SHOW_DEBUG_INFORMATION 
#define DEBUG(x);	x;
#else
#define DEBUG(x);	
#endif

#define MAX_N_AREAS		62
static const int SPECIAL_BIT = 62;
#define SPECIAL_BLOCK (1i64 << SPECIAL_BIT)

#endif


