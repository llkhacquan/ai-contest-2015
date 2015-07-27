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
#include <thread>
#include <chrono>

#include "..\AI_Template\include\ai\defines.h"
#include "..\AI_Template\include\ai\AI.h"

#define BOT_ACTIVE	1
#define SHOW_DEBUG_INFORMATION	0

#ifdef _DEBUG
#define MIN_DEPTH	 7
#define MAX_DEPTH	 20
#define ISOLATED_DEPTH 10
#define DANGER_TIME		2500
#else
#define MIN_DEPTH	 12
#define MAX_DEPTH	 27
#define ISOLATED_DEPTH 15
#define DANGER_TIME		2700
#endif

typedef long long TBlock;
typedef int TPlayer;
typedef int TMove;
typedef int Pos1D;
using namespace std;

#define TIMEOUT_POINTS 2000
#define POINTS 1000
#define CC(x,y)	CONVERT_COORD(x,y)
#define BOARD_SIZE (MAP_SIZE*MAP_SIZE)
#define MY_INFINITY	(POINTS + 10)
#define BLOCK_ENEMY_AREA (-2)

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


