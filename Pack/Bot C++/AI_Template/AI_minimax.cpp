#include "MyAI.h"
#include "mydefine.h"


// this alpha beta method help player_1 win
int CMyAI::ab(int board[], const CPos&_p1, const CPos&_p2, Player next, int depth, int a, int b)
{
	int v;
	assert((v = getBlock(board, _p1)) == BLOCK_PLAYER_1);
	assert((v = getBlock(board, _p2)) == BLOCK_PLAYER_2);
	bool bOk;

	int winner = evaluateBoard(board, _p1, _p2, next);
	if (winner != OBSERVER || depth == 0){
		return rateBoard2(board, _p1, _p2, next);
	}

	CPos newPos;
	vector<Direction> allMoves; allMoves.reserve(3);
	allMoves = next == PLAYER_1 ? getAvailableMoves(board, _p1) : getAvailableMoves(board, _p2);
	sortMoves(allMoves, _p1, _p2, next);
	if (next == PLAYER_1){
		v = MIN_POINTS;
		for (int iMove = 0; iMove < (int)allMoves.size(); iMove++){
			newPos = _p1.move(allMoves[iMove]);
			bOk = move(board, _p1, allMoves[iMove]); assert(bOk);
			v = max(v, ab(board, newPos, _p2, PLAYER_2, depth - 1, a, b));
			bOk = move(board, newPos, getOpositeDirection(allMoves[iMove]), true); assert(bOk);
			a = max(a, v);
			if (b <= a)
				break;
		}
	}
	else {
		v = MAX_POINTS;
		for (int iMove = 0; iMove < (int)allMoves.size(); iMove++){
			newPos = _p2.move(allMoves[iMove]);
			bOk = move(board, _p2, allMoves[iMove]); assert(bOk);
			v = min(v, ab(board, _p1, newPos, PLAYER_1, depth - 1, a, b));
			bOk = move(board, newPos, getOpositeDirection(allMoves[iMove]), true); assert(bOk);
			b = min(b, v);
			if (b <= a)
				break;
		}
	}
	return v;
}

// evaluate board
// return the player will win : PLAYER_1 or PLAYER_2 or OBSERVER if unknown
int CMyAI::evaluateBoard(const int _board[121], const CPos &_player1, const CPos &_player2, const Player next) {
	assert(getBlock(_board, _player1) == BLOCK_PLAYER_1);
	assert(getBlock(_board, _player2) == BLOCK_PLAYER_2);
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
	return OBSERVER;
}

// this method rate a board in normal case by recursion searching
// this method does modify the board but in the end, it restore the original state of the board
// range: 0 - 1000; 0: player 1 lost, 1000:player 1 lost

void CMyAI::sortMoves(vector<Direction> moves, const CPos &_player1, const CPos &_player2, const Player nextPlayer)
{
	assert(moves.size() <= 4);
	int points[4];

	static CPos currentPlayer, oppositePlayer;
	if (nextPlayer == PLAYER_1)
		for (int i = 0; i < (int)moves.size(); i++){
			static CPos pos = _player1.move(moves[i]);
			points[i] = abs(pos.x - _player2.x) + abs(pos.y - _player2.y);
		}
	else
		for (int i = 0; i < (int)moves.size(); i++){
			static CPos pos = _player2.move(moves[i]);
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

// check if 2 positions is isolated or not
// 2 positions is not isolated when it does exist a path (length>0) of empty blocks that connects them
// this method does not modify _boardData
// return -1: if isolated
// else return length of shortest path from _pos1 to _pos2 (thought the empty block of course)
int CMyAI::isIsolated(const int _boardData[121], const CPos &_p1, const CPos &_p2)
{
	static int board[BOARD_SIZE]; // static for fast
	// copy for safety
	memcpy(board, _boardData, BOARD_SIZE*sizeof(int));

	// search for _pos2 form _pos1
	setBlock(board, _p1.x, _p1.y, SPECIAL_BLOCK);
	queue<CPos> q;
	q.push(_p1);

	while (q.size() > 0){
		CPos p = q.front();
		q.pop();

		for (Direction i = 1; i <= 4; i++){
			if (p.move(i) == _p2)
				return getBlock(board, p) + 1 - SPECIAL_BLOCK;
		}

		for (int i = 1; i <= 4; i++){
			CPos newP = p.move(i);
			if (getBlock(board, newP) == BLOCK_EMPTY){
				setBlock(board, newP, getBlock(board, p) + 1);
				q.push(newP);
			}
		}
	}
	return -1;
}

bool CMyAI::isIsolated()
{
	return isIsolated(boardData, posPlayer1, posPlayer2) < 0;
}

// this method modify board, fill each connected empty block with pos a value equal to: SPECIAL_BLOCK + distance;
// distance = shortest path from pos to the block. path.size >= 1
// average time: 0.02ms for release
// return the sum of all distances
int CMyAI::fillDistance(int _board[121], const CPos &pos)
{
	int result = 0;
	setBlock(_board, pos.x, pos.y, SPECIAL_BLOCK);
	queue<CPos> q;
	q.push(pos);

	while (q.size() > 0){
		CPos p = q.front();
		q.pop();
		for (int i = 1; i <= 4; i++){
			CPos newP = p.move(i);
			if (getBlock(_board, newP) == BLOCK_EMPTY){
				result += getBlock(_board, p) + 1 - SPECIAL_BLOCK;
				setBlock(_board, newP, getBlock(_board, p) + 1);
				q.push(newP);
			}
		}
	}
	return result;
}