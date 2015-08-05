#include "StaticFunctions.h"
#include "FastPos1DDeque.h"

vector<TMove> getAvailableMoves(const TBlock board[], const Pos1D &pos, bool *output)
{
	assert(pos >= 0 && pos < BOARD_SIZE);
	vector<TMove> result;
	for (TMove i = 1; i <= 4; i++)
		if (GET_BLOCK(board, MOVE(pos, i)) == BLOCK_EMPTY){
			if (output != NULL)
				output[i - 1] = true;
			result.push_back(i);
		}
		else
			if (output != NULL)
				output[i - 1] = false;

	return result;
}

vector<TMove> &getALongestPath(TBlock const boardData[], const Pos1D &pos)
{
	assert(pos >= 0 && pos < BOARD_SIZE);
	static TBlock board[121];
	memcpy(board, boardData, BOARD_SIZE*sizeof(TBlock));

	Pos1D p = pos;

	static vector<TMove> c(BOARD_SIZE), l(BOARD_SIZE); // store the current path and the longest path
	c.clear();
	l.clear();
	findLongestPath(board, p, c, l);
	return l;
}

void findLongestPath(TBlock board[], Pos1D& pos, vector<TMove> &c, vector<TMove> &l)
{
#ifdef _DEBUG
	int backup[121];
	memcpy(backup, board, BOARD_SIZE*sizeof(TBlock));
#endif // _DEBUG
	static bool bOk;
	bool avalableMove[4];
	if (getAvailableMoves(board, pos, avalableMove).size() == 0)
		if (c.size() > l.size())
			l = vector<TMove>(c);
	for (TMove iMove = 1; iMove <= 4; iMove++){
		if (!avalableMove[iMove - 1])
			continue;
		bOk = move(board, pos, iMove); assert(bOk);
		pos = MOVE(pos, iMove);
		c.push_back(iMove);

		findLongestPath(board, pos, c, l);

		bOk = move(board, pos, getOpositeDirection(iMove), true); assert(bOk);
		pos = MOVE(pos, getOpositeDirection(iMove));
		c.pop_back();
	}

#ifdef _DEBUG
	assert(memcmp(backup, board, BOARD_SIZE*sizeof(TBlock)) == 0);
#endif // _DEBUG
}

bool move(TBlock _board[], const Pos1D &currentPos, const TMove direction, const bool backMode /*= false*/)
{
	TBlock block = GET_BLOCK(_board, currentPos);
	assert(block == BLOCK_PLAYER_1 || block == BLOCK_PLAYER_2);
	Pos1D newPos = MOVE(currentPos, direction);
	if (backMode){
		if (block == BLOCK_PLAYER_1 && GET_BLOCK(_board, newPos) == BLOCK_PLAYER_1_TRAIL){
			setBlock(_board, newPos, BLOCK_PLAYER_1);
			setBlock(_board, currentPos, BLOCK_EMPTY);
			return true;
		}
		else if (block == BLOCK_PLAYER_2 && GET_BLOCK(_board, newPos) == BLOCK_PLAYER_2_TRAIL){
			setBlock(_board, newPos, BLOCK_PLAYER_2);
			setBlock(_board, currentPos, BLOCK_EMPTY);
			return true;
		}
		else
			return false;
	}
	else {
		if (GET_BLOCK(_board, newPos) == BLOCK_EMPTY){
			setBlock(_board, newPos, block); // set newPos as current block_player_current
			setBlock(_board, currentPos, block == BLOCK_PLAYER_1 ? BLOCK_PLAYER_1_TRAIL : BLOCK_PLAYER_2_TRAIL);
			return true;
		}
		else
			return false;
	}
}

// this method modify board, fill each connected empty block with pos a value equal to: SPECIAL_BLOCK + distance;
// distance = shortest path from pos to the block. path.size >= 1
// average time: 0.02ms for release
// return the sum of all distances
int fillDistance(TBlock _board[], const Pos1D &pos) {
	TBlock result = 0;
	setBlock(_board, pos, SPECIAL_BLOCK);
	CFastPos1DDeque q;
	q.push_back(pos);

	while (q.size() > 0){
		Pos1D p(q.pop_front());
		for (int i = 1; i <= 4; i++){
			const Pos1D newP = MOVE(p, i);
			if (GET_BLOCK(_board, newP) == BLOCK_EMPTY){
				result += GET_BLOCK(_board, p) + 1 - SPECIAL_BLOCK;
				setBlock(_board, newP, GET_BLOCK(_board, p) + 1);
				q.push_back(newP);
			}
		}
	}
	return (int)result;
}

bool isIsolated(const TBlock _board[], const Pos1D &_p1, const Pos1D &_p2)
{
	assert(_board[_p1] == BLOCK_PLAYER_1);
	assert(_board[_p2] == BLOCK_PLAYER_2);
	static TBlock board[BOARD_SIZE];
	memcpy(board, _board, BOARD_SIZE*sizeof(TBlock));

	bool visisted[BOARD_SIZE] = { false };
	// search for _pos2 form _pos1
	CFastPos1DDeque queue;
	queue.push_back(_p1);
	visisted[_p1] = true;
	while (!queue.empty())
	{
		Pos1D p = queue.pop_front();
		board[p] = SPECIAL_BLOCK;
		for (int i = 1; i <= 4; i++){
			Pos1D u = MOVE(p, i);
			if (GET_BLOCK(board, u) != BLOCK_EMPTY)
				continue;
			if (visisted[u])
				continue;
			queue.push_back(u);
			visisted[u] = true;
		}
	}

	for (int i = 1; i <= 4; i++){
		if (GET_BLOCK(board, MOVE(_p2, i)) == SPECIAL_BLOCK)
			return false;
	}
	return true;
}

#ifdef OPENCV
cv::Mat toImage(TBlock board[], bool special)
{
	static cv::Mat result;
	result = cv::Mat(11 * PIXEL_PER_BLOCK, 11 * PIXEL_PER_BLOCK, CV_8UC3);
	static cv::Mat *target;

	for (int i = 0; i < BOARD_SIZE; i++) {
		cv::Mat newMat = Mat(PIXEL_PER_BLOCK - HALF_PIXEL_PER_LINE * 2, PIXEL_PER_BLOCK - HALF_PIXEL_PER_LINE * 2,
			CV_8UC3, Scalar(50, 50, 50));
		int x = i % MAP_SIZE;
		int y = i / MAP_SIZE;
		TBlock block = board[i];
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

void createNewBoard(TBlock board[], int nObstacle)
{
	memset(board, 0, BOARD_SIZE*sizeof(TBlock));

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

void printBoard(const TBlock board[], const bool special)
{
#if SHOW_DEBUG_INFORMATION
	static std::string line;

	for (int y = 0; y < MAP_SIZE; y++) {
		line = "| ";
		for (int x = 0; x < MAP_SIZE; x++){
			TBlock block = board[CC(x, y)];
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

void fillChamberWithBattleFields(const TBlock gatesBoard[], const TBlock board[], CFastPos1DDeque &enemies, TBlock fillBoard[])
{
	memcpy(fillBoard, board, sizeof(TBlock)*BOARD_SIZE);
	while (enemies.size() > 0){
		Pos1D v(enemies[enemies.size() - 1]);
		enemies.pop_back();

		fillBoard[v] = BLOCK_ENEMY_AREA;
		for (int i = 1; i <= 4; i++){
			Pos1D u = MOVE(v, i);
			if (GET_BLOCK(fillBoard, u) == BLOCK_EMPTY && GET_BLOCK(gatesBoard, u) != SPECIAL_BLOCK)
				enemies.push_back(u);
		}
	}
}

int lengthWhenTryToReachBattleFields(const TBlock board[], const TBlock dBoard[], const TBlock filledBoard[], const Pos1D& _p){
	CFastPos1DDeque q;
	q.push_back(_p);
	TBlock ourBoard[BOARD_SIZE];

	bool visitted[BOARD_SIZE] = { false };
	Pos1D parrent[BOARD_SIZE];
	visitted[_p] = true;
	while (!q.empty()){
		Pos1D v(q.front());
		if (GET_BLOCK(filledBoard, v) == BLOCK_ENEMY_AREA)
			break;
		q.pop_front();
		for (int i = 1; i <= 4; i++){
			Pos1D u = MOVE(v, i);
			TBlock block = GET_BLOCK(board, u);
			if (!(block == BLOCK_EMPTY || block == BLOCK_ENEMY_AREA))
				continue;
			if (visitted[u])
				continue;
			visitted[u] = true;
			parrent[u] = v;
			q.push_back(u);
		}
	}

	TBlock minDistance = dBoard[q.front()];
	int maxLength = -1;
	while (!q.empty()){
		Pos1D v(q.pop_front());
		if (dBoard[v] > SPECIAL_BLOCK + minDistance)
			continue;
		if (filledBoard[v] != BLOCK_ENEMY_AREA)
			continue;
		memcpy(ourBoard, board, sizeof(BOARD_SIZE));
		ourBoard[v] = board[_p];

		TBlock oldBlock;
		do {
			oldBlock = ourBoard[parrent[v]];
			ourBoard[parrent[v]] = BLOCK_OBSTACLE;
			v = parrent[v];
		} while (oldBlock != board[_p]);
		maxLength = max(maxLength,
			CBiconnectedComponents::getEstimatedLength(ourBoard, v, -1) + (int)dBoard[v]);
	}
	return maxLength;
}
