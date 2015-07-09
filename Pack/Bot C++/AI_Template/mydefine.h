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
#include <opencv2\highgui\highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;


struct Position;

typedef int Player;
typedef int Direction;
typedef int Vertex;

using namespace std;

#define CC(x,y)	CONVERT_COORD(x,y)
#define BOARD_SIZE (MAP_SIZE*MAP_SIZE)
#define MAX_POINTS	1000
#define MIN_POINTS	0
#define DEFAULT_DEPTH 10

/// GRAPHIC
#define PIXEL_PER_BLOCK 50
#define HALF_PIXEL_PER_LINE	1
#define FPS				60
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

static const int SPECIAL_BLOCK = 1073741824; // 2^30


#endif


