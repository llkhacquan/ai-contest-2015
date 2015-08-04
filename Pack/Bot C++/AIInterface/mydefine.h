#pragma once

#ifndef MY_DEFINE
#define MY_DEFINE

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <stack>
#include <fstream>
#include <ctime>
#include <cassert>
#include <algorithm>
#include <set>
#include <bitset>
#include <thread>
#include <chrono>
#include <bitset>

#include "..\AI_Template\include\ai\defines.h"
#include "..\AI_Template\include\ai\AI.h"

#define BOT_ACTIVE	1
#define USING_MEMORY 0
#define SHOW_DEBUG_INFORMATION	0
#define TRY_FOLLOWING	 0

// set DISABLE_TIMEOUT 1 when run You vs P1 Debug
#define DISABLE_TIMEOUT 0

#ifdef _DEBUG
#if USING_MEMORY
#define MIN_DEPTH	 7
#else
#define MIN_DEPTH	 10
#endif
#if DISABLE_TIMEOUT
#define MAX_DEPTH	 11
#else
#define MAX_DEPTH	 17
#endif // DISABLE_TIMEOUT
#define ISOLATED_DEPTH 10
#define DISTANCE 3
#define DANGER_TIME		2700
#else
#if USING_MEMORY
#define MIN_DEPTH	 14
#else
#define MIN_DEPTH	 14
#endif
#if DISABLE_TIMEOUT
#define MAX_DEPTH	 25
#else
#define MAX_DEPTH	 30
#endif // DISABLE_TIMEOUT
#define ISOLATED_DEPTH 15
#define DISTANCE 9
#define DANGER_TIME		2600
#endif

typedef long long TBlock;
typedef int TPlayer;
typedef int Pos1D;
typedef int TMove;
using namespace std;

#define TIMEOUT_POINTS 2000
#define POINTS 1000
#define CC(x,y)	CONVERT_COORD(x,y)
#define BOARD_SIZE (MAP_SIZE*MAP_SIZE)
#define MY_INFINITY	(POINTS + 10)
#define BLOCK_ENEMY_AREA (-2)

extern Pos1D moveTable[BOARD_SIZE][4];
#define MOVE(x, y) moveTable[x][(y)-1]

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


