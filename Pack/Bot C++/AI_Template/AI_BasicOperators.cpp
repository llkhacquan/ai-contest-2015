#include "MyAI.h"
#include "mydefine.h"

int CMyAI::DEPTH;
Mat CMyAI::empty;
Mat CMyAI::obstacle;
Mat CMyAI::player_1;
Mat CMyAI::player_1_trail;
Mat CMyAI::player_2;
Mat CMyAI::player_2_trail;

CMyAI::CMyAI()
{
	ifstream f;
	int d;
	f.open("config.txt");
	if (f.is_open()){
		f >> d;
		f.close();
	}
	else
		d = DEFAULT_DEPTH;
	DEPTH = d;
	p_ai = AI::GetInstance();

	empty = Mat(PIXEL_PER_BLOCK - HALF_PIXEL_PER_LINE * 2, PIXEL_PER_BLOCK - HALF_PIXEL_PER_LINE * 2,
		CV_8UC3, Scalar(50, 50, 50));
	obstacle = Mat(PIXEL_PER_BLOCK - HALF_PIXEL_PER_LINE * 2, PIXEL_PER_BLOCK - HALF_PIXEL_PER_LINE * 2,
		CV_8UC3, Scalar(100, 100, 100));
	player_1 = Mat(PIXEL_PER_BLOCK - HALF_PIXEL_PER_LINE * 2, PIXEL_PER_BLOCK - HALF_PIXEL_PER_LINE * 2,
		CV_8UC3, Scalar(255, 0, 0));
	player_1_trail = Mat(PIXEL_PER_BLOCK - HALF_PIXEL_PER_LINE * 2, PIXEL_PER_BLOCK - HALF_PIXEL_PER_LINE * 2,
		CV_8UC3, Scalar(100, 0, 0));
	player_2 = Mat(PIXEL_PER_BLOCK - HALF_PIXEL_PER_LINE * 2, PIXEL_PER_BLOCK - HALF_PIXEL_PER_LINE * 2,
		CV_8UC3, Scalar(0, 255, 0));
	player_2_trail = Mat(PIXEL_PER_BLOCK - HALF_PIXEL_PER_LINE * 2, PIXEL_PER_BLOCK - HALF_PIXEL_PER_LINE * 2,
		CV_8UC3, Scalar(0, 100, 0));

	copyMakeBorder(empty, empty, HALF_PIXEL_PER_LINE, HALF_PIXEL_PER_LINE,
		HALF_PIXEL_PER_LINE, HALF_PIXEL_PER_LINE, BORDER_CONSTANT, Scalar(0, 0, 0));
	copyMakeBorder(obstacle, obstacle, HALF_PIXEL_PER_LINE, HALF_PIXEL_PER_LINE,
		HALF_PIXEL_PER_LINE, HALF_PIXEL_PER_LINE, BORDER_CONSTANT, Scalar(0, 0, 0));
	copyMakeBorder(player_1, player_1, HALF_PIXEL_PER_LINE, HALF_PIXEL_PER_LINE,
		HALF_PIXEL_PER_LINE, HALF_PIXEL_PER_LINE, BORDER_CONSTANT, Scalar(0, 0, 0));
	copyMakeBorder(player_1_trail, player_1_trail, HALF_PIXEL_PER_LINE, HALF_PIXEL_PER_LINE,
		HALF_PIXEL_PER_LINE, HALF_PIXEL_PER_LINE, BORDER_CONSTANT, Scalar(0, 0, 0));
	copyMakeBorder(player_2, player_2, HALF_PIXEL_PER_LINE, HALF_PIXEL_PER_LINE,
		HALF_PIXEL_PER_LINE, HALF_PIXEL_PER_LINE, BORDER_CONSTANT, Scalar(0, 0, 0));
	copyMakeBorder(player_2_trail, player_2_trail, HALF_PIXEL_PER_LINE, HALF_PIXEL_PER_LINE,
		HALF_PIXEL_PER_LINE, HALF_PIXEL_PER_LINE, BORDER_CONSTANT, Scalar(0, 0, 0));
}

CMyAI::~CMyAI()
{

}

/* Get block */

// get block. return -1 if out of block

int CMyAI::getBlock(const int x, const int y)
{
	return getBlock(boardData, x, y);
}

int CMyAI::getBlock(int const board[], const Vertex pos)
{
	assert(pos >= 0 && pos < BOARD_SIZE);
	return getBlock(board, pos%MAP_SIZE, pos / MAP_SIZE);
}

int CMyAI::getBlock(int const board[], const int x, const int y)
{
	if (x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE)
		return BLOCK_OUT_OF_BOARD;
	else
		return board[CC(x, y)];
}

int CMyAI::getBlock(int const board[], const CPos &pos)
{
	return getBlock(board, pos.x, pos.y);
}

// set block. return false if out of block

bool CMyAI::setBlock(int board[], const int x, const int y, const int value)
{
	if (x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE)
		return false;
	else{
		board[CC(x, y)] = value;
		return true;
	}
}

bool CMyAI::setBlock(int board[], const Vertex pos, const int value)
{
	assert(pos >= 0 && pos < BOARD_SIZE);
	return setBlock(board, pos %MAP_SIZE, pos / MAP_SIZE, value);
}

bool CMyAI::setBlock(int board[], const CPos &pos, const int value)
{
	return setBlock(board, pos.x, pos.y, value);
}

bool CMyAI::setBlock(const int x, const int y, int value)
{
	return setBlock(boardData, x, y, value);
}

// 

Direction CMyAI::getOpositeDirection(const Direction direction)
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
		return -1;
	}
}

bool CMyAI::move(int _board[], const CPos &currentPos, const Direction direction, const bool backMode){
	int block = getBlock(_board, currentPos);
	assert(block == BLOCK_PLAYER_1 || block == BLOCK_PLAYER_2);
	CPos newPos = currentPos.move(direction);
	if (backMode){
		if (block == BLOCK_PLAYER_1 && getBlock(_board, newPos) == BLOCK_PLAYER_1_TRAIL){
			setBlock(_board, newPos, BLOCK_PLAYER_1);
			setBlock(_board, currentPos, BLOCK_EMPTY);
			return true;
		}
		else if (block == BLOCK_PLAYER_2 && getBlock(_board, newPos) == BLOCK_PLAYER_2_TRAIL){
			setBlock(_board, newPos, BLOCK_PLAYER_2);
			setBlock(_board, currentPos, BLOCK_EMPTY);
			return true;
		}
		else
			return false;
	}
	else {
		if (getBlock(_board, newPos) == BLOCK_EMPTY){
			setBlock(_board, newPos, block); // set newPos as current block_player_current
			setBlock(_board, currentPos, block == BLOCK_PLAYER_1 ? BLOCK_PLAYER_1_TRAIL : BLOCK_PLAYER_2_TRAIL);
			return true;
		}
		else
			return false;
	}
}

void CMyAI::move(Direction direction)
{
	bool bOk = move(boardData, *myPos, direction);
	assert(bOk);
	// history.push_back(direction);
	*myPos = myPos->move(direction);
}


// return available move if a player is in the given position
vector<int> CMyAI::getAvailableMoves(const int board[], const CPos &pos)
{
	assert(pos.x >= 0 && pos.x < MAP_SIZE && pos.y >= 0 && pos.y < MAP_SIZE);
	static vector<int> result;
	result.reserve(4);
	result.clear();
	if (getBlock(board, pos.x - 1, pos.y) == BLOCK_EMPTY)
		result.push_back(DIRECTION_LEFT);
	if (getBlock(board, pos.x + 1, pos.y) == BLOCK_EMPTY)
		result.push_back(DIRECTION_RIGHT);
	if (getBlock(board, pos.x, pos.y - 1) == BLOCK_EMPTY)
		result.push_back(DIRECTION_UP);
	if (getBlock(board, pos.x, pos.y + 1) == BLOCK_EMPTY)
		result.push_back(DIRECTION_DOWN);
	return result;
}

// result a vector of position that is BLOCK_EMPTY next to pos
vector<CPos> CMyAI::getNearEmptyBlock(const int board[], const CPos &pos)
{
	assert(pos.x >= 0 && pos.x < MAP_SIZE && pos.y >= 0 && pos.y < MAP_SIZE);
	vector<CPos> result;
	result.clear();
	for (Direction i = 1; i <= 4; i++){
		if (getBlock(board, pos.move(i)) == BLOCK_EMPTY)
			result.push_back(pos.move(i));
	}
	return result;
}