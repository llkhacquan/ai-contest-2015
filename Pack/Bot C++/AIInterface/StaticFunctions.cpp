#include "StaticFunctions.h"
#include "SmallDeque.h"

vector<TMove> getAvailableMoves(const TBlock board[], const TPos &pos, bool *output)
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

vector<TMove> &getALongestPath(TBlock const boardData[], const TPos &pos)
{
	assert(pos >= 0 && pos < BOARD_SIZE);
	static TBlock board[121];
	memcpy(board, boardData, BOARD_SIZE*sizeof(TBlock));

	TPos p = pos;

	static vector<TMove> c(BOARD_SIZE), l(BOARD_SIZE); // store the current path and the longest path
	c.clear();
	l.clear();
	findLongestPath(board, p, c, l);
	return l;
}

void findLongestPath(TBlock board[], TPos& pos, vector<TMove> &c, vector<TMove> &l)
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

bool move(TBlock _board[], const TPos &currentPos, const TMove direction, const bool backMode /*= false*/)
{
	TBlock block = GET_BLOCK(_board, currentPos);
	assert(block == BLOCK_PLAYER_1 || block == BLOCK_PLAYER_2);
	TPos newPos = MOVE(currentPos, direction);
	if (backMode){
		if (block == BLOCK_PLAYER_1 && GET_BLOCK(_board, newPos) == BLOCK_PLAYER_1_TRAIL){
			SET_BLOCK(_board, newPos, BLOCK_PLAYER_1);
			SET_BLOCK(_board, currentPos, BLOCK_EMPTY);
			return true;
		}
		else if (block == BLOCK_PLAYER_2 && GET_BLOCK(_board, newPos) == BLOCK_PLAYER_2_TRAIL){
			SET_BLOCK(_board, newPos, BLOCK_PLAYER_2);
			SET_BLOCK(_board, currentPos, BLOCK_EMPTY);
			return true;
		}
		else
			return false;
	}
	else {
		if (GET_BLOCK(_board, newPos) == BLOCK_EMPTY){
			SET_BLOCK(_board, newPos, block); // set newPos as current block_player_current
			SET_BLOCK(_board, currentPos, block == BLOCK_PLAYER_1 ? BLOCK_PLAYER_1_TRAIL : BLOCK_PLAYER_2_TRAIL);
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
int fillDistance(TBlock _board[], const TPos &pos) {
	TBlock result = 0;
	SET_BLOCK(_board, pos, SPECIAL_BLOCK);
	CSmallDeque q;
	q.push_back(pos);

	while (q.size() > 0){
		TPos p(q.pop_front());
		for (int i = 1; i <= 4; i++){
			const TPos newP = MOVE(p, i);
			if (GET_BLOCK(_board, newP) == BLOCK_EMPTY){
				result += GET_BLOCK(_board, p) + 1 - SPECIAL_BLOCK;
				SET_BLOCK(_board, newP, GET_BLOCK(_board, p) + 1);
				q.push_back(newP);
			}
		}
	}
	return (int)result;
}

bool isIsolated(const TBlock _board[], const TPos &_p1, const TPos &_p2)
{
	assert(_board[_p1] == BLOCK_PLAYER_1);
	assert(_board[_p2] == BLOCK_PLAYER_2);
	static TBlock board[BOARD_SIZE];
	memcpy(board, _board, BOARD_SIZE*sizeof(TBlock));

	bool visisted[BOARD_SIZE] = { false };
	// search for _pos2 form _pos1
	CSmallDeque queue;
	queue.push_back(_p1);
	visisted[_p1] = true;
	while (!queue.empty())
	{
		TPos p = queue.pop_front();
		board[p] = SPECIAL_BLOCK;
		for (int i = 1; i <= 4; i++){
			TPos u = MOVE(p, i);
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

int exploreMapWithoutRecursion(const TBlock _board[], const int overEstimatedResult, const TPos& _pos)
{
	TMove path[BOARD_SIZE];
	memset(path, 0, sizeof(TMove)*BOARD_SIZE);
	bool bOk;
	CSmallDeque history;
	int maxLength = 0;
	TPos pos = _pos;
	int cDepth = 0;
	TBlock board[BOARD_SIZE];
	memcpy(board, _board, sizeof(TBlock)*BOARD_SIZE);

	int noOfFailTry = 0;
	while (cDepth >= 0){
		assert(cDepth < BOARD_SIZE);
		path[cDepth]++;
		if (path[cDepth] == 5){
			if (history.size() == 0)
				break;
			path[cDepth] = 0;
			bOk = move(board, pos, getOpositeDirection(history.back()), true);
			pos = MOVE(pos, getOpositeDirection(history.back()));
			assert(bOk);
			history.pop_back();
			cDepth--;
			noOfFailTry = 0;
			continue;
		}
		assert(path[cDepth] > 0 && path[cDepth] <= 4);
		bOk = move(board, pos, path[cDepth]);
		if (!bOk){
			noOfFailTry++;
			if (noOfFailTry == 4) // we have 4 fail try, it is an end position
			{
				// processing end position
				if (maxLength < (int) history.size())
					maxLength = (int) history.size();
				assert(history.size() == cDepth);
				if (maxLength == overEstimatedResult)
					break;
				// end processing end position
			}
		}
		else {
			pos = MOVE(pos, path[cDepth]);
			history.push_back(path[cDepth]);
			noOfFailTry = 0;// reset this, it is not an end position

			// process the mediate position
			assert(history.size() == cDepth + 1);
			// end processing
			cDepth++;
		}
	}

	assert(maxLength <= overEstimatedResult);
	return maxLength;
}

int exploreMapWithoutRecursion(const TBlock _board[], const int overEstimatedResult, const TPos& _pos, const TPos &_endPos)
{
	TMove path[BOARD_SIZE];
	memset(path, 0, sizeof(TMove)*BOARD_SIZE);
	bool bOk;
	CSmallDeque history;
	int maxLength = 0;
	TPos pos = _pos;
	int cDepth = 0;
	TBlock board[BOARD_SIZE];
	memcpy(board, _board, sizeof(TBlock)*BOARD_SIZE);

	int noOfFailTry = 0;
	while (cDepth >= 0){
		assert(cDepth < BOARD_SIZE);
		path[cDepth]++;
		if (path[cDepth] == 5){
			if (history.size() == 0)
				break;
			path[cDepth] = 0;
			bOk = move(board, pos, getOpositeDirection(history.back()), true);
			assert(bOk);
			pos = MOVE(pos, getOpositeDirection(history.back()));
			history.pop_back();
			cDepth--;
			noOfFailTry = 0;
			continue;
		}
		assert(path[cDepth] > 0 && path[cDepth] <= 4);
		bOk = move(board, pos, path[cDepth]);
		if (!bOk){
			noOfFailTry++;
			if (noOfFailTry == 4) // we have 4 fail try, it is an end position
			{
				// processing end position (an end position is also an mediate position
				assert(history.size() == cDepth);
				// end processing end position
			}
		}
		else {
			pos = MOVE(pos, path[cDepth]);
			history.push_back(path[cDepth]);
			noOfFailTry = 0;// reset this, it is not an end position

			// process the mediate position
			if (pos == _endPos){
				if (maxLength < (int)history.size())
					maxLength = history.size();
				assert(history.size() == cDepth + 1);
				if (abs(maxLength - overEstimatedResult) % 2 == 1)
					maxLength++;
				assert(maxLength <= overEstimatedResult);
				if (maxLength == overEstimatedResult)
					break;
				// return now
				cDepth++;
				path[cDepth] = 4;
				continue;
			}
			// end processing
			cDepth++;
		}
	}

	assert(maxLength <= overEstimatedResult);
	assert(abs(maxLength - overEstimatedResult) % 2 == 0);
	return maxLength;
}

int exploreMapWithoutRecursion(bool visited[], const int overEstimatedResult, const TPos &u, const TPos&v)
{
	TMove path[BOARD_SIZE];
	memset(path, 0, sizeof(TMove)*BOARD_SIZE);
	bool bOk;
	CSmallDeque history;
	int maxLength = 0;
	TPos pos = u;
	int cDepth = 0;

	int noOfFailTry = 0;
	while (cDepth >= 0){
		assert(cDepth < BOARD_SIZE);
		path[cDepth]++;
		if (path[cDepth] == 5){
			if (history.size() == 0)
				break;
			path[cDepth] = 0;
			TPos newPos = getOpositeDirection(history.back());
			assert(newPos >= 0 && newPos < BOARD_SIZE);
			assert(visited[pos]);
			visited[pos] = false;
			pos = MOVE(pos, getOpositeDirection(history.back()));
			history.pop_back();
			cDepth--;
			noOfFailTry = 0;
			continue;
		}
		assert(path[cDepth] > 0 && path[cDepth] <= 4);
		// bOk = move(board, pos, path[cDepth]);
		TPos newPos = MOVE(pos, path[cDepth]);
		if (newPos < 0)
			bOk = false;
		else
		{
			bOk = !visited[newPos];
			visited[newPos] = true;
		}
		if (!bOk){
			noOfFailTry++;
			if (noOfFailTry == 4) // we have 4 fail try, it is an end position
			{
				// processing end position (an end position is also an mediate position
				assert(history.size() == cDepth);
				// end processing end position
			}
		}
		else {
			pos = MOVE(pos, path[cDepth]);
			history.push_back(path[cDepth]);
			noOfFailTry = 0;// reset this, it is not an end position

			// process the mediate position
			if (pos == v){
				if (maxLength < (int)history.size())
					maxLength = history.size();
				assert(history.size() == cDepth + 1);
				if (abs(maxLength - overEstimatedResult) % 2 == 1)
					maxLength++;
				assert(maxLength <= overEstimatedResult);
				if (maxLength == overEstimatedResult)
					break;
				// return now
				cDepth++;
				path[cDepth] = 4;
				continue;
			}
			// end processing
			cDepth++;
		}
	}

	assert(maxLength <= overEstimatedResult);
	assert(abs(maxLength - overEstimatedResult) % 2 == 0);
	return maxLength;
}

int exploreMapWithoutRecursion(bool visited[], const int overEstimatedResult, const TPos &u)
{
	TMove path[BOARD_SIZE];
	memset(path, 0, sizeof(TMove)*BOARD_SIZE);
	bool bOk;
	CSmallDeque history;
	int maxLength = 0;
	TPos pos = u;
	int cDepth = 0;

	int noOfFailTry = 0;
	while (cDepth >= 0){
		assert(cDepth < BOARD_SIZE);
		path[cDepth]++;
		if (path[cDepth] == 5){
			if (history.size() == 0)
				break;
			path[cDepth] = 0;
			TPos newPos = getOpositeDirection(history.back());
			assert(newPos >= 0 && newPos < BOARD_SIZE);
			assert(visited[pos]);
			visited[pos] = false;
			pos = MOVE(pos, getOpositeDirection(history.back()));
			history.pop_back();
			cDepth--;
			noOfFailTry = 0;
			continue;
		}
		assert(path[cDepth] > 0 && path[cDepth] <= 4);
		// bOk = move(board, pos, path[cDepth]);
		TPos newPos = MOVE(pos, path[cDepth]);
		if (newPos < 0)
			bOk = false;
		else
		{
			bOk = !visited[newPos];
			visited[newPos] = true;
		}
		if (!bOk){
			noOfFailTry++;
			if (noOfFailTry == 4) // we have 4 fail try, it is an end position
			{
				// processing end position
				if (maxLength < (int) history.size())
					maxLength = (int) history.size();
				assert(history.size() == cDepth);
				if (maxLength == overEstimatedResult)
					break;
				// end processing end position
			}
		}
		else {
			pos = MOVE(pos, path[cDepth]);
			history.push_back(path[cDepth]);
			noOfFailTry = 0;// reset this, it is not an end position

			// process the mediate position
			assert(history.size() == cDepth + 1);
			// end processing
			cDepth++;
		}
	}

	assert(maxLength <= overEstimatedResult);
	return maxLength;
}

int distanceBetween2Pos(const TPos &p1, const TPos &p2)
{
	int x1 = p1 % MAP_SIZE;
	int y1 = p1 / MAP_SIZE;
	int x2 = p2 % MAP_SIZE;
	int y2 = p2 / MAP_SIZE;
	return abs(x1 - x2) + abs(y1 - y2);
}
