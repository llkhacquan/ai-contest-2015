#include "StaticFunctions.h"

void setBit(int &number, int iBit)
{
	number |= 1 << iBit;
	assert(number >= 0);
}
void clearBit(int &number, int iBit)
{
	number &= ~(1 << iBit);
	assert(number >= 0);
}
void toggleBit(int &number, int iBit)
{
	number ^= 1 << iBit;
	assert(number >= 0);
}
bool getBit(const int &number, int iBit)
{
	return ((number >> iBit) & 1);
}
void changeBit(int &number, int iBit, int value)
{
	number ^= (-value ^ number) & (1 << iBit);
}
int ipowBase2(int exp)
{
	return 1 << exp;
}
int findCode(int block)
{
	int t = block - SPECIAL_BLOCK;
	int code = 0;
	while (((t & 1) == 0) && (t > 0)){
		t >>= 1;
		code++;
	}
	return code;
}

TMove getOpositeDirection(const TMove direction)
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

int getBlock(int const board[], const Pos1D pos)
{
	assert(pos >= 0 && pos < BOARD_SIZE);
	return getBlock(board, pos%MAP_SIZE, pos / MAP_SIZE);
}
int getBlock(int const board[], const int x, const int y)
{
	if (x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE)
		return BLOCK_OUT_OF_BOARD;
	else
		return board[CC(x, y)];
}
int getBlock(int const board[], const Pos2D &pos)
{
	return getBlock(board, pos.x, pos.y);
}

bool setBlock(int board[], const int x, const int y, const int value)
{
	if (x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE)
		return false;
	else{
		board[CC(x, y)] = value;
		return true;
	}
}
bool setBlock(int board[], const Pos1D pos, const int value)
{
	assert(pos >= 0 && pos < BOARD_SIZE);
	return setBlock(board, pos %MAP_SIZE, pos / MAP_SIZE, value);
}
bool setBlock(int board[], const Pos2D &pos, const int value)
{
	return setBlock(board, pos.x, pos.y, value);
}

vector<TMove> getAvailableMoves(const int board[], const Pos2D &pos, bool *output)
{
	assert(pos.x >= 0 && pos.x < MAP_SIZE && pos.y >= 0 && pos.y < MAP_SIZE);
	vector<TMove> result;
	for (TMove i = 1; i <= 4; i++)
		if (getBlock(board, pos.move(i)) == BLOCK_EMPTY){
			if (output != NULL)
				output[i - 1] = true;
			result.push_back(i);
		}
		else
			if (output != NULL)
				output[i - 1] = true;

	return result;
}

vector<TMove>& getALongestPath(int const boardData[], const Pos2D &pos)
{
	assert(pos.x >= 0 && pos.x < MAP_SIZE && pos.y >= 0 && pos.y < MAP_SIZE);
	static int board[121];
	memcpy(board, boardData, BOARD_SIZE*sizeof(int));

	Pos2D p = pos;

	static vector<int> c(BOARD_SIZE), l(BOARD_SIZE); // store path of one of the longest paths
	c.clear();
	l.clear();
	findLongestPath(board, p, c, l);
	return l;
}

void findLongestPath(int board[], Pos2D& pos, vector<TMove> &c, vector<TMove> &l)
{
	static bool bOk;
	static bool avalableMove[4];
	getAvailableMoves(board, pos, avalableMove);
	for (TMove iMove = 1; iMove <= 4; iMove++){
		if (!avalableMove[iMove - 1])
			continue;
		bOk = move(board, pos, iMove);
		assert(bOk);
		c.push_back(iMove);
		if (c.size() > l.size())
			l = c;
		pos = pos.move(iMove);
		findLongestPath(board, pos, c, l);

		bOk = move(board, pos, getOpositeDirection(iMove), true); assert(bOk);
		pos = pos.move(getOpositeDirection(iMove));
		c.pop_back();
	}
}

bool move(int _board[], const Pos2D &currentPos, const TMove direction, const bool backMode /*= false*/)
{
	int block = getBlock(_board, currentPos);
	assert(block == BLOCK_PLAYER_1 || block == BLOCK_PLAYER_2);
	Pos2D newPos = currentPos.move(direction);
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

vector<TMove>& findShortestPath(const int _board[], const Pos2D& start, const Pos2D& end)
{
	static vector<TMove> path(BOARD_SIZE);
	path.clear();
	if (start == end)
		return path;

	int board[BOARD_SIZE];
	memcpy(board, _board, BOARD_SIZE*sizeof(int));

	// we move from end point to every point
	fillDistance(board, end);

	// we scan the 4 blocks near the start point, find the block with minimum distance from end point (if there is no such point, start and end are seperated
	Pos2D cPos, mPos(-1, -1);
	for (TMove i = 1; i <= 4; i++){
		int block = getBlock(board, cPos = start.move(i));
		if (getBlock(_board, end) == BLOCK_EMPTY)
		{
			if (block >= SPECIAL_BLOCK){
				if (mPos.x == -1){
					mPos = cPos;
					path.push_back(i);
				}
				else if (block < getBlock(board, cPos)){
					path[0] = i;
					mPos = cPos;
				}
			}
		}
		else
			if (block > SPECIAL_BLOCK){
				if (mPos.x == -1){
					mPos = cPos;
					path.push_back(i);
				}
				else if (block < getBlock(board, cPos)){
					path[0] = i;
					mPos = cPos;
				}
			}
	}

	if (mPos.x == -1){
		// cant reach end pos from start pos
		return path;
	}

	assert(path.size() == 1);

	// current point = mPoint (minPoint)
	cPos = mPos;
	while (true){
		int block = getBlock(board, cPos);
		if (block == SPECIAL_BLOCK){
			// we reach the end point
			assert(cPos == end);
			break;
		}
		for (TMove i = 1; i <= 4; i++){
			if (getBlock(board, (mPos = cPos.move(i))) == block - 1){
				path.push_back(i);
				cPos = mPos;
				break;
			}
		}
	}

#ifdef _DEBUG
	memcpy(board, _board, BOARD_SIZE*sizeof(int));
	Pos2D p = start;
	for (unsigned int i = 0; i < path.size(); i++){
		bool bOk = move(board, p, path[i]); assert(bOk);
		p = p.move(path[i]);
	}
	assert(p == end);
#endif
	return path;
}

// this method modify board, fill each connected empty block with pos a value equal to: SPECIAL_BLOCK + distance;
// distance = shortest path from pos to the block. path.size >= 1
// average time: 0.02ms for release
// return the sum of all distances
int fillDistance(int _board[121], const Pos2D &pos) {
	int result = 0;
	setBlock(_board, pos.x, pos.y, SPECIAL_BLOCK);
	queue<Pos2D> q;
	q.push(pos);

	while (q.size() > 0){
		Pos2D p = q.front();
		q.pop();
		for (int i = 1; i <= 4; i++){
			Pos2D newP = p.move(i);
			if (getBlock(_board, newP) == BLOCK_EMPTY){
				result += getBlock(_board, p) + 1 - SPECIAL_BLOCK;
				setBlock(_board, newP, getBlock(_board, p) + 1);
				q.push(newP);
			}
		}
	}
	return result;
}

// check if 2 positions is isolated or not
// 2 positions is not isolated when it does exist a path (length>0) of empty blocks that connects them
// this method does not modify _boardData
// return -1: if isolated
// else return length of shortest path from _pos1 to _pos2 (thought the empty block of course)
int isIsolated(const int _board[121], const Pos2D &_p1, const Pos2D &_p2)
{
	static int board[BOARD_SIZE]; // static for fast
	// copy for safety
	memcpy(board, _board, BOARD_SIZE*sizeof(int));

	// search for _pos2 form _pos1
	setBlock(board, _p1.x, _p1.y, SPECIAL_BLOCK);
	queue<Pos2D> q;
	q.push(_p1);

	while (q.size() > 0){
		Pos2D p = q.front();
		q.pop();

		for (TMove i = 1; i <= 4; i++){
			if (p.move(i) == _p2)
				return getBlock(board, p) + 1 - SPECIAL_BLOCK;
		}

		for (int i = 1; i <= 4; i++){
			Pos2D newP = p.move(i);
			if (getBlock(board, newP) == BLOCK_EMPTY){
				setBlock(board, newP, getBlock(board, p) + 1);
				q.push(newP);
			}
		}
	}
	return -1;
}

#ifdef OPENCV
cv::Mat toImage(int board[], bool special)
{
	static cv::Mat result;
	result = cv::Mat(11 * PIXEL_PER_BLOCK, 11 * PIXEL_PER_BLOCK, CV_8UC3);
	static cv::Mat *target;

	for (int i = 0; i < BOARD_SIZE; i++) {
		cv::Mat newMat = Mat(PIXEL_PER_BLOCK - HALF_PIXEL_PER_LINE * 2, PIXEL_PER_BLOCK - HALF_PIXEL_PER_LINE * 2,
			CV_8UC3, Scalar(50, 50, 50));
		int x = i % MAP_SIZE;
		int y = i / MAP_SIZE;
		int block = board[i];
		switch (block) {
		case BLOCK_EMPTY:
			target = &empty;
			break;
		case BLOCK_OBSTACLE:
			target = &obstacle;
			break;
		case BLOCK_PLAYER_1:
			target = &player_1;
			break;
		case BLOCK_PLAYER_1_TRAIL:
			target = &player_1_trail;
			break;
		case BLOCK_PLAYER_2:
			target = &player_2;
			break;
		case BLOCK_PLAYER_2_TRAIL:
			target = &player_2_trail;
			break;
		default:
			target = &newMat;
			putText(result, std::to_string(block - SPECIAL_BLOCK), cvPoint(3, 3),
				FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
		}
		target->copyTo(result.rowRange(PIXEL_PER_BLOCK*y, PIXEL_PER_BLOCK*(y + 1)).
			colRange(PIXEL_PER_BLOCK*x, PIXEL_PER_BLOCK*(x + 1)));
	}
	return result;
}
#endif // OPENCV

void createNewBoard(int board[], int nObstacle)
{
	memset(board, 0, BOARD_SIZE*sizeof(int));

	int nObject = 0;
	while (nObject < 5){
		int random;
		do {
			random = rand() % 121;
		} while (random == 0 || random == 120 || random == 60 || board[random] != BLOCK_EMPTY);
		nObject++;
		board[random] = BLOCK_OBSTACLE;
		board[120 - random] = BLOCK_OBSTACLE;
	}
	board[0] = BLOCK_PLAYER_1;
	board[BOARD_SIZE - 1] = BLOCK_PLAYER_2;

	int count = 0;
	while (count < nObstacle){
		int random = rand() % 121;
		if (board[random] == BLOCK_EMPTY){
			board[random] = BLOCK_OBSTACLE;
			count++;
		}
	}
}

void printBoard(const int board[], const bool special)
{
#if SHOW_DEBUG_INFORMATION
	static std::string line;

	for (int y = 0; y < MAP_SIZE; y++) {
		line = "| ";
		for (int x = 0; x < MAP_SIZE; x++){
			int block = board[CC(x, y)];
			if (special)
				if (block >= SPECIAL_BLOCK)
					cout << setfill(' ') << setw(5) << block - SPECIAL_BLOCK << "|";
				else if (block >= 0)
					cout << setfill(' ') << setw(5) << "-----" << "|";
				else
					cout << setfill(' ') << setw(5) << block << "|";
			else
				cout << setfill(' ') << setw(5) << block << "|";
		}
		cout << endl;
	}
	cout << endl;
#endif
}

#ifdef OPENCV
void setupImage()
{
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
#endif // OPENCV
