#include "MyAI.h"
#include "mydefine.h"
#include "Algorithm.h"




CMyAI::CMyAI()
{
	p_ai = AI::GetInstance();
}

CMyAI::~CMyAI()
{

}

//////////////////////////////////////////////////////////////////////////
//					NON STATIC FUNCTIONS								//
//////////////////////////////////////////////////////////////////////////

bool CMyAI::setBlock(const int x, const int y, int value)
{
	return setBlock(boardData, x, y, value);
}

int CMyAI::getBlock(const int x, const int y)
{
	return getBlock(boardData, x, y);
}

void CMyAI::updateBoard()
{
	const int *newBoard = p_ai->GetBoard();

	// check consistency of new board and old board. it ASSERT false, there is something wrong with server.

	bool isNewGame = newBoard[0] == BLOCK_PLAYER_1 || newBoard[MAP_SIZE*MAP_SIZE - 1] == BLOCK_PLAYER_2;

	static Position newPostOfEnemy, *oldPostOfEnemy;
	newPostOfEnemy = p_ai->GetEnemyPosition();
	if (!isNewGame){
		if (weArePlayer1){
			oldPostOfEnemy = &posPlayer2;
		}
		else {
			oldPostOfEnemy = &posPlayer1;
		}
		if (oldPostOfEnemy->x == newPostOfEnemy.x){
			if (oldPostOfEnemy->y - 1 == newPostOfEnemy.y)
				history.push_back(DIRECTION_UP);
			else if (oldPostOfEnemy->y + 1 == newPostOfEnemy.y)
				history.push_back(DIRECTION_DOWN);
			else ASSERT(false);
		}
		else if (oldPostOfEnemy->x - 1 == newPostOfEnemy.x){
			if (oldPostOfEnemy->y == newPostOfEnemy.y)
				history.push_back(DIRECTION_LEFT);
			else ASSERT(false);
		}
		else  if (oldPostOfEnemy->x + 1 == newPostOfEnemy.x){
			if (oldPostOfEnemy->y == newPostOfEnemy.y)
				history.push_back(DIRECTION_RIGHT);
			else ASSERT(false);
		}
		else ASSERT(false);

		for (int i = 0; i < MAP_SIZE*MAP_SIZE; i++){
			switch (boardData[i]){
			case BLOCK_OBSTACLE:
			case BLOCK_PLAYER_1_TRAIL:
			case BLOCK_PLAYER_2_TRAIL:
				ASSERT(newBoard[i] == boardData[i]);
				break;
			case BLOCK_PLAYER_2:
				if (weArePlayer1){
					ASSERT(newBoard[i] == BLOCK_PLAYER_2_TRAIL);
				}
				else{
					ASSERT(newBoard[i] == BLOCK_PLAYER_2);
				}
				break;
			case BLOCK_PLAYER_1:
				if (weArePlayer1){
					ASSERT(newBoard[i] == BLOCK_PLAYER_1);
				}
				else{
					ASSERT(newBoard[i] == BLOCK_PLAYER_1_TRAIL);
				}
				break;
			case BLOCK_EMPTY:
				ASSERT(newBoard[i] == BLOCK_EMPTY || newBoard[i] == BLOCK_PLAYER_1 || newBoard[i] == BLOCK_PLAYER_2);
				break;
			default:
				ASSERT(false);
			}
		}
	}
	else { // new game
		// update player index
		cout << "New game start!!!" << endl;
		iGame++;
		weArePlayer1 = (newBoard[CC(p_ai->GetMyPosition().x, p_ai->GetMyPosition().y)] == BLOCK_PLAYER_1);
		if (weArePlayer1)
			myPos = &posPlayer1;
		else myPos = &posPlayer2;

		// update first player
		if (newBoard[0] == BLOCK_PLAYER_1)
			player1Gofirst = false;
		else
			player1Gofirst = true;
	}

	// update newBoard to boardData and players' positions
	memcpy(boardData, newBoard, MAP_SIZE*MAP_SIZE*sizeof(int));
	posPlayer1 = weArePlayer1 ? p_ai->GetMyPosition() : p_ai->GetEnemyPosition();
	posPlayer2 = weArePlayer1 ? p_ai->GetEnemyPosition() : p_ai->GetMyPosition();
}

bool CMyAI::isIsolated()
{
	return isIsolated(boardData, posPlayer1, posPlayer2) < 0;
}

// move our player to a new direction
void CMyAI::move(Direction direction)
{
	history.push_back(direction);
	bool bOk = move(boardData, *myPos, direction);
	ASSERT(bOk);
	*myPos = myPos->move(direction);
}

bool CMyAI::move(int* _board, const Position &playerPos, const Direction direction, const bool backMode)
{
	static Position newPos; newPos = playerPos.move(direction);
	static int block; block = getBlock(_board, playerPos);
	ASSERT(block == BLOCK_PLAYER_1 || block == BLOCK_PLAYER_2);
	if (!backMode){
		if (getBlock(_board, newPos) == BLOCK_EMPTY){
			if (block == BLOCK_PLAYER_1)
				setBlock(_board, playerPos, BLOCK_PLAYER_1_TRAIL);
			else
				setBlock(_board, playerPos, BLOCK_PLAYER_2_TRAIL);

			setBlock(_board, newPos, block);
			return true;
		}
		else
			return false;
	}
	else {
		if (block == BLOCK_PLAYER_1){
			if (getBlock(_board, newPos) != BLOCK_PLAYER_1_TRAIL)
				return false;
			setBlock(_board, newPos, PLAYER_1);
			setBlock(_board, playerPos, BLOCK_EMPTY);
			return true;
		}
		else{
			if (getBlock(_board, newPos) == BLOCK_PLAYER_2_TRAIL)
				return false;
			setBlock(_board, newPos, PLAYER_2);
			setBlock(_board, playerPos, BLOCK_EMPTY);
			return true;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//						STATIC FUNCTIONS								//
//////////////////////////////////////////////////////////////////////////

void CMyAI::findLongestPath(int* board, Position& pos, vector<Direction> &c, vector<Direction> &l){
	static bool bOk;
	vector<Direction> allMoves = getAvalableMoves(board, pos);
	for (int i = 0; i < (int)allMoves.size(); i++){
		bOk = move(board, pos, allMoves[i]);
		ASSERT(bOk);
		c.push_back(allMoves[i]);
		if (c.size()>l.size())
			l = c;
		pos = pos.move(allMoves[i]);

		findLongestPath(board, pos, c, l);

		bOk = move(board, pos, getOpositeDirection(allMoves[i]), true); ASSERT(bOk);
		pos = pos.move(getOpositeDirection(allMoves[i]));
	}
}

vector<int> CMyAI::getALongestPath(const int* boardData, const Position &pos)
{
	ASSERT(pos.x >= 0 && pos.x < MAP_SIZE && pos.y >= 0 && pos.y < MAP_SIZE);
	static int board[121];
	memcpy(board, boardData, MAP_SIZE*MAP_SIZE*sizeof(int));

	Position p = pos;

	static vector<int> c, l; // store path of one of the longest paths
	c.clear();
	l.clear();
	findLongestPath(board, p, c, l);
	return l;
}

// check if 2 positions is isolated or not
// 2 positions is not isolated when it does exist a path (length>0) of empty blocks that connects them
// this method does not modify _boardData
// return -1: if isolated
// else return length of shortest path from _pos1 to _pos2 (thought the empty block of course)
int CMyAI::isIsolated(const int* _boardData, const Position &_p1, const Position &_p2)
{
	static int board[MAP_SIZE*MAP_SIZE]; // static for fast
	// copy for safety
	memcpy(board, _boardData, MAP_SIZE*MAP_SIZE*sizeof(int));

	// search for _pos2 form _pos1
	setBlock(board, _p1.x, _p1.y, SPECIAL_BLOCK);
	queue<Position> q;
	q.push(_p1);

	while (q.size() > 0){
		Position p = q.front();
		q.pop();

		for (Direction i = 1; i <= 4; i++){
			if (p.move(i) == _p2)
				return getBlock(board, p) + 1 - SPECIAL_BLOCK;
		}
		vector<Position> avalableBlocks = getNearEmptyBlock(board, p);
		for (int i = 0; i < (int)avalableBlocks.size(); i++){
			setBlock(board, avalableBlocks[i].x, avalableBlocks[i].y, getBlock(board, p.x, p.y) + 1);
			q.push(avalableBlocks[i]);
		}
	}
	return -1;
}

// get block. return false if out of block
int CMyAI::getBlock(const int* board, const int x, const int y)
{
	if (x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE)
		return BLOCK_OUT_OF_BOARD;
	else
		return board[CC(x, y)];
}

int CMyAI::getBlock(const int board[], const Position &pos)
{
	return getBlock(board, pos.x, pos.y);
}

int CMyAI::getBlock(const int board[BOARD_SIZE], const Vertex &u)
{
	assert(u >= 0 && u < BOARD_SIZE);
	int x = u % MAP_SIZE;
	return board[u];
}

// set block. return false if out of block
bool CMyAI::setBlock(int* board, const int x, const int y, const int value)
{
	if (x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE)
		return false;
	else{
		board[CC(x, y)] = value;
		return true;
	}
}

bool CMyAI::setBlock(int* board, const Position &pos, const int value)
{
	return setBlock(board, pos.x, pos.y, value);
}

int CMyAI::getOpositeDirection(const int direction)
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
		ASSERT(false);
		return -1;
	}
}


// return available move if a player is in the given position
vector<int> CMyAI::getAvalableMoves(const int* board, const Position &pos)
{
	ASSERT(pos.x >= 0 && pos.x < MAP_SIZE && pos.y >= 0 && pos.y < MAP_SIZE);
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
vector<Position> CMyAI::getNearEmptyBlock(int* board, const Position &pos)
{
	ASSERT(pos.x >= 0 && pos.x < MAP_SIZE && pos.y >= 0 && pos.y < MAP_SIZE);
	static vector<Position> result;
	result.clear();
	if (getBlock(board, pos.x - 1, pos.y) == BLOCK_EMPTY)
		result.push_back(Position(pos.x - 1, pos.y));
	if (getBlock(board, pos.x + 1, pos.y) == BLOCK_EMPTY)
		result.push_back(Position(pos.x + 1, pos.y));
	if (getBlock(board, pos.x, pos.y - 1) == BLOCK_EMPTY)
		result.push_back(Position(pos.x, pos.y - 1));
	if (getBlock(board, pos.x, pos.y + 1) == BLOCK_EMPTY)
		result.push_back(Position(pos.x, pos.y + 1));
	return result;
}

// this method modify board, fill each connected empty block with pos a value equal to: SPECIAL_BLOCK + distance;
// distance = shortest path from pos to the block. path.size >= 1
// average time: 0.02ms for release
void CMyAI::fillDistance(int* _board, const Position &pos)
{
	setBlock(_board, pos.x, pos.y, SPECIAL_BLOCK);
	queue<Position> q;
	q.push(pos);

	while (q.size() > 0){
		Position p = q.front();
		q.pop();
		vector<Position> avalableBlocks = getNearEmptyBlock(_board, p);
		for (int i = 0; i < (int)avalableBlocks.size(); i++){
			setBlock(_board, avalableBlocks[i].x, avalableBlocks[i].y, getBlock(_board, p.x, p.y) + 1);
			q.push(avalableBlocks[i]);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//			DEBUG FUNCTIONS
//////////////////////////////////////////////////////////////////////////
void CMyAI::printBoard(const int* board, const bool specialBlock)
{
	static std::string line;

	for (int y = 0; y < MAP_SIZE; y++) {
		line = "| ";
		for (int x = 0; x < MAP_SIZE; x++){
			int block = board[CC(x, y)];
			if (specialBlock)
				if (block >= SPECIAL_BLOCK)
					cout << setfill(' ') << setw(3) << block - SPECIAL_BLOCK << " | ";
				else
					cout << setfill(' ') << setw(3) << "___" << " | ";
			else
				cout << setfill(' ') << setw(3) << block << " | ";
		}
		cout << endl;
	}
	cout << endl;
}

// this method create a random new game in the board.
// this method does not alocate memory for board
// TODO: implement the noRandomMove
void CMyAI::createNewBoard(int* board, int noRandomMove)
{
	memset(board, 0, MAP_SIZE*MAP_SIZE*sizeof(int));

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
	board[MAP_SIZE*MAP_SIZE - 1] = BLOCK_PLAYER_2;

	int count = 0;
	while (count < noRandomMove){
		int random = rand() % 121;
		if (board[random] == BLOCK_EMPTY){
			board[random] = BLOCK_OBSTACLE;
			count++;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//					MAIN FUNCTIONS
//////////////////////////////////////////////////////////////////////////

// calculate and give an optimal move for ourselves 
Direction CMyAI::newTurn()
{
	static int tempBoard[MAP_SIZE*MAP_SIZE];
	// update board first
	updateBoard();

	// start ISOLATED MODE
	if (isIsolated()){
		vector<int> path = getALongestPath(boardData, p_ai->GetMyPosition());
		if (path.size() > 0)
			return path[0];
		else
		{
			return -1;
			cout << "Oh shit, we lost!!" << endl;
		}
	}
	// end ISOLATED MODE

	// start NEW_GAME_MODE
	// find the shortest path to line 5.0->0.5 for player 1 or 10.5-5.10 for player 2
	if (history.size() / 2 < 6){
		memcpy(tempBoard, boardData, MAP_SIZE*MAP_SIZE*sizeof(int));
		CMyAI::fillDistance(tempBoard, *myPos);
		vector<int> points; points.reserve(6);
		if (weArePlayer1){
			for (int i = 0; i <= 5; i++){
				if (boardData[CC(i, 5 - i)] == BLOCK_EMPTY)
					points.push_back(getAvalableMoves(boardData, Position(i, 5 - i)).size());
				else
					points.push_back(-100);
			}
		}
		else {
			for (int i = 5; i <= 10; i++){
				if (boardData[CC(i, 15 - i)] == BLOCK_EMPTY)
					points.push_back(getAvalableMoves(boardData, Position(i, 15 - i)).size());
				else
					points.push_back(-100);
			}
		}
		points[1] += 1; points[2] += 2;
		points[4] += 1; points[3] += 2;

		int iMax = 0;
		for (int i = 1; i <= 5; i++)
			if (points[iMax] <= points[i])
				iMax = i;

		static Position target;
		if (weArePlayer1)
			target = Position(iMax, 5 - iMax);
		else
			target = Position(5 + iMax, 11 - iMax);
		while (getBlock(tempBoard, target) > SPECIAL_BLOCK + 1){
			for (int i = 1; i <= 4; i++)
				if (getBlock(tempBoard, target.move(i)) == getBlock(tempBoard, target) - 1){
					target = target.move(i);
					break;
				}
		}

		for (int i = 1; i <= 4; i++)
			if (weArePlayer1){
				if (posPlayer1.move(i) == target)
					return i;
			}
			else if (posPlayer2.move(i) == target)
				return i;

	}
	// end NEW_GAME_MODE

	// start NORMAL_MODE

	if (weArePlayer1)
		return optimalMove(boardData, posPlayer1, posPlayer2, PLAYER_1);
	else
		return optimalMove(boardData, posPlayer1, posPlayer2, PLAYER_2);
	// end NORMAL_MODE

}


// evaluate board
// return the player will win : PLAYER_1 or PLAYER_2 or OBSERVER if unknown
int CMyAI::evaluateBoard(int* _board, const Position &_player1, const Position &_player2, const Player next) {
	ASSERT(getBlock(_board, _player1) == BLOCK_PLAYER_1);
	ASSERT(getBlock(_board, _player2) == BLOCK_PLAYER_2);
	if (isIsolated(_board, _player1, _player2) == -1){
		int length1 = getLengthOfLongestPath(_board, _player1);
		int length2 = getLengthOfLongestPath(_board, _player2);
		if (length1 > length2)
			return PLAYER_1;
		else if (length1 < length2)
			return PLAYER_2;
		else if (next == PLAYER_1) // length1 = length2
			return PLAYER_2;
		else
			return PLAYER_1;
	}

	if (evaluateBoardCase1(_board, _player1, _player2))
		return next;


	return OBSERVER;
}


// this method use a special method to get the length of the longest path when player move form the input position in boardData
int CMyAI::getLengthOfLongestPath(const int* boardData, const Position &pos)
{
	// copy the board
	static int board[MAP_SIZE*MAP_SIZE];
	memcpy(board, boardData, MAP_SIZE*MAP_SIZE*sizeof(int));

	// backup original value of block[pos]
	static int originalValueOfPos;
	originalValueOfPos = getBlock(board, pos);
	ASSERT(originalValueOfPos != BLOCK_OUT_OF_BOARD);

	int odd = 0, even = 0;
	setBlock(board, pos.x, pos.y, SPECIAL_BLOCK);
	queue<Position> q;
	q.push(pos);

	// TODO:

	while (q.size() > 0){
		Position p = q.front();
		q.pop();
		vector<Position> avalableBlocks = getNearEmptyBlock(board, p);
		for (int i = 0; i < (int)avalableBlocks.size(); i++){
			setBlock(board, avalableBlocks[i].x, avalableBlocks[i].y, getBlock(board, p.x, p.y) + 1);
			if ((avalableBlocks[i].x + avalableBlocks[i].y) % 2 == 0)
				even++;
			else
				odd++;
			q.push(avalableBlocks[i]);
		}
	}

	return max(odd, even) + 1;
}

// first case:
//   X 0
//   0 X
bool CMyAI::evaluateBoardCase1(const int* _board, const Position &_player1, const Position &_player2)
{
	int deltaX = _player2.x - _player1.x;
	int deltaY = _player2.y - _player1.y;

	if (abs(deltaX) == 1 && abs(deltaY) == 1
		&& getBlock(_board, _player1.x, _player2.y) == BLOCK_EMPTY
		&& getBlock(_board, _player2.x, _player1.y) == BLOCK_EMPTY)
		return true;
	else
		return false;
}

// second case:
//   X 0 0
//   0 0 0 
//   0 0 X
bool CMyAI::evaluateBoardCase2(const int* _board, const Position &_player1, const Position &_player2)
{
	int deltaX = _player2.x - _player1.x;
	int deltaY = _player2.y - _player1.y;

	if (abs(deltaX) == 1 && abs(deltaY) == 1
		&& getBlock(_board, _player1.x, _player2.y) == BLOCK_EMPTY
		&& getBlock(_board, _player2.x, _player1.y) == BLOCK_EMPTY)
		return true;
	else
		return false;
}

// this method rate a board in normal case by recursion searching
// this method does modify the board but in the end, it restore the original state of the board
// range: 0 - 100; 0: player 1 lost, 100:player 1 lost, 50: unknown
int CMyAI::rateBoard(int* board, const Position &_p1, const Position &_p2, const Player next)
{
	ASSERT(next == PLAYER_1 || next == PLAYER_2);
	ASSERT(getBlock(board, _p1) == BLOCK_PLAYER_1 && getBlock(board, _p2) == BLOCK_PLAYER_2);
	int result = evaluateBoard(board, _p1, _p2, next);
	switch (result){
	case PLAYER_1:
		return 100;
	case PLAYER_2:
		return 0;
	case OBSERVER:
		return 50;
	default:
		ASSERT(false);
		return 50;
	}
}

// this method is call to get the optimalMove for the next player
Direction CMyAI::optimalMove(int* board, const Position &_p1, const Position &_p2, const Player next)
{
	bool bOk;
	int depth = 20;
	Position newPos;
	vector<Direction> allMoves; allMoves.clear();
	allMoves = next == PLAYER_1 ? getAvalableMoves(board, _p1) : getAvalableMoves(board, _p2);

	if (allMoves.size() == 0){
		// we die
		return rand() % 4 + 1;
	}

	int points[4];

	for (int iMove = 0; iMove < (int)allMoves.size(); iMove++){
		if (next == PLAYER_1){
			newPos = _p1.move(allMoves[iMove]);
			bOk = move(board, _p1, allMoves[iMove]); ASSERT(bOk);
			points[iMove] = ab(board, newPos, _p2, PLAYER_2, depth - 1, -100, 100, true);
		}
		else {
			newPos = _p2.move(allMoves[iMove]);
			bOk = move(board, _p2, allMoves[iMove]); ASSERT(bOk);
			points[iMove] = ab(board, _p1, newPos, PLAYER_1, depth - 1, -100, 100, false);
		}
		bOk = move(board, newPos, getOpositeDirection(allMoves[iMove]), true); ASSERT(bOk);
	}
	int iMax = 0;
	for (int i = 1; i <= (int)allMoves.size(); i++){
		if (points[iMax] < points[i])
			iMax = i;
	}
	return allMoves[iMax];
}

void CMyAI::sortMoves(vector<Direction> moves, const Position &_player1, const Position &_player2, const Player nextPlayer)
{
	ASSERT(moves.size() <= 4);
	int points[4];

	static Position currentPlayer, oppositePlayer;
	if (nextPlayer == PLAYER_1)
		for (int i = 0; i < (int)moves.size(); i++){
			static Position pos = _player1.move(moves[i]);
			points[i] = abs(pos.x - _player2.x) + abs(pos.y - _player2.y);
		}
	else
		for (int i = 0; i < (int)moves.size(); i++){
			static Position pos = _player2.move(moves[i]);
			points[i] = abs(pos.x - _player1.x) + abs(pos.y - _player1.y);
		}

	for (int i = 0; i < (int)moves.size(); i++){
		bool swapped = false;
		for (int j = moves.size() - 1; j > i; j--){
			if (points[j] < points[j - 1]){
				swap(points[j], points[j - 1]);
				swap(moves[j], moves[j - 1]);
				swapped = true;
			}
		}
		if (!swapped)
			break;
	}
}

// this alpha beta method help player_1 win
int CMyAI::ab(int* board, const Position&_p1, const Position&_p2, Player next, int depth, int a, int b, bool maximize)
{
	ASSERT(getBlock(board, _p1) == BLOCK_PLAYER_1 && getBlock(board, _p2) == BLOCK_PLAYER_2);
	bool bOk;
	Position newPos;
	int v, rate = rateBoard(board, _p1, _p2, next);
	if (rate == 0 || rate == 100 || depth == 0)
		return rate;

	vector<Direction> allMoves; allMoves.clear();
	allMoves = next == PLAYER_1 ? getAvalableMoves(board, _p1) : getAvalableMoves(board, _p2);
	sortMoves(allMoves, _p1, _p2, next);
	if (maximize){
		v = 0;
		for (int iMove = 0; iMove < (int)allMoves.size(); iMove++){
			if (next == PLAYER_1){
				newPos = _p1.move(allMoves[iMove]);
				bOk = move(board, _p1, allMoves[iMove]); ASSERT(bOk);
				v = max(v, ab(board, newPos, _p2, PLAYER_2, depth - 1, a, b, false));
			}
			else {
				newPos = _p2.move(allMoves[iMove]);
				bOk = move(board, _p2, allMoves[iMove]); ASSERT(bOk);
				v = max(v, ab(board, _p1, newPos, PLAYER_1, depth - 1, a, b, false));
			}
			bOk = move(board, newPos, getOpositeDirection(allMoves[iMove]), true); ASSERT(bOk);
			a = max(a, v);
			if (b <= a)
				break;
		}
		return v;
	}
	else {
		v = 100;
		for (int iMove = 0; iMove < (int)allMoves.size(); iMove++){
			if (next == PLAYER_1){
				newPos = _p1.move(allMoves[iMove]);
				bOk = move(board, _p1, allMoves[iMove]); ASSERT(bOk);
				v = min(v, ab(board, newPos, _p2, PLAYER_2, depth - 1, a, b, true));
			}
			else {
				newPos = _p2.move(allMoves[iMove]);
				bOk = move(board, _p2, allMoves[iMove]); ASSERT(bOk);
				v = min(v, ab(board, _p1, newPos, PLAYER_1, depth - 1, a, b, true));
			}
			bOk = move(board, newPos, getOpositeDirection(allMoves[iMove]), true); ASSERT(bOk);
			b = min(b, v);
			if (b <= a)
				break;
		}
		return v;
	}
}
