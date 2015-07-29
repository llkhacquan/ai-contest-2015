#include "HeuristicBase.h"
#include "BiconnectedComponents.h"
#include "MyAI.h"
#include "MyTimer.h"
#include "GameState.h"
#include "TranspositionTable.h"
#include "ArticulationPoints.h"
#include "FastPos1DDeque.h"


CHeuristicBase::CHeuristicBase()
{
}

CHeuristicBase::~CHeuristicBase()
{
}

void CHeuristicBase::exploreToPathLongestPath(TBlock board[], Pos2D &pos, vector<TMove> &cPath, vector<TMove> &oldPath, int &oldL, int depth)
{
	assert(getBlock(board, pos) == BLOCK_PLAYER_1 || getBlock(board, pos) == BLOCK_PLAYER_2);
	assert(depth >= 0);
	auto availMoves = getAvailableMoves(board, pos);
	bool bOk;
	if (availMoves.size() == 0)
	{
		if (cPath.size() > oldPath.size() + oldL){
			oldL = 0;
			oldPath = vector<TMove>(cPath);
		}
		else if (cPath.size() == oldPath.size() + oldL && oldL > 0){
			oldL = 0;
			oldPath = vector<TMove>(cPath);
		}
	}
	else if (depth == 0) {
		int l = CHeuristicBase::getUpperLengthOfTheLongestPath(board, pos);
		if (cPath.size() + l > oldPath.size() + oldL){
			oldL = l;
			oldPath = vector<TMove>(cPath);
		}
		else if (cPath.size() == oldPath.size() + oldL && oldL > l){
			oldL = l;
			oldPath = vector<TMove>(cPath);
		}
	}
	else
		for (unsigned int i = 0; i < availMoves.size(); i++){
			bOk = move(board, pos, availMoves[i]); assert(bOk);
			pos = pos.move(availMoves[i]);
			cPath.push_back(availMoves[i]);

			exploreToPathLongestPath(board, pos, cPath, oldPath, oldL, depth - 1);

			TMove back = getOpositeDirection(availMoves[i]);
			bOk = move(board, pos, back, true); assert(bOk);
			pos = pos.move(back);
			cPath.pop_back();
		}
}

TMove CHeuristicBase::getFirstMoveOfTheLongestPath(const TBlock boardData[], const Pos2D &pos, const int nDepth)
{
	assert(nDepth > 0);
	TBlock board[BOARD_SIZE];
	memcpy(board, boardData, BOARD_SIZE*sizeof(TBlock));
	vector<TMove> c, l;
	c.reserve(nDepth);
	l.reserve(nDepth);
	int length = 0;
	auto t = getAvailableMoves(boardData, pos);
	if (t.size() == 1)
		return t[0];

	if (t.size() == 0)
		return 0;

	Pos2D p = pos;
	exploreToPathLongestPath(board, p, c, l, length, nDepth);
	assert(l.size() > 0);
	return l[0];
}

vector<TMove> CHeuristicBase::getFirstMovesOfTheLongestPath(const TBlock boardData[], const Pos2D &pos)
{
	TBlock board[BOARD_SIZE];
	memcpy(board, boardData, BOARD_SIZE*sizeof(TBlock));
	int iMax = 0;
	vector<TMove> result(4);
	result.clear();

	bool bMoves[4];
	{
		vector<TMove> availMoves = getAvailableMoves(boardData, pos, bMoves);
		if (availMoves.size() == 1) {
			result.push_back(availMoves[0]);
		}
	}

	int lengths[4] = { -1 };
	int maxLength = -1;
	for (unsigned int iMove = 1; iMove <= 4; iMove++){
		if (!bMoves[iMove - 1])
			continue;

		bool bOk = move(board, pos, iMove); assert(bOk);
		lengths[iMove - 1] = getUpperLengthOfTheLongestPath(board, pos.move(iMove));
		if (lengths[iMove - 1] > maxLength){
			iMax = iMove;
			maxLength = lengths[iMove - 1];
		}
		bOk = move(board, pos.move(iMove), getOpositeDirection(iMove), true);
		assert(bOk);
	}
	DEBUG(cout << "estimated length = " << maxLength << endl);
	for (unsigned int i = 0; i < 4; i++){
		if (lengths[i] == maxLength)
			result.push_back(i + 1);
	}
	return result;
}

int CHeuristicBase::getUpperLengthOfTheLongestPath(TBlock const board[], const Pos2D &playerPos, const int depth)
{
	return CBiconnectedComponents::getEstimatedLength(board, playerPos, depth);
}

// return the player will win : PLAYER_1 or PLAYER_2 or OBSERVER if unknown
int CHeuristicBase::evaluateBoard(const TBlock _board[121], const Pos2D &_p1, const Pos2D &_p2,
	const TPlayer next, int &point, const bool goodEstimate) {
	assert(getBlock(_board, _p1) == BLOCK_PLAYER_1);
	assert(getBlock(_board, _p2) == BLOCK_PLAYER_2);
	int length1, length2;
	if (isIsolated(_board, _p1, _p2)){
		length1 = goodEstimate ? CHeuristicBase::getLowerLengthOfTheLongestPath(_board, _p1) :
			CHeuristicBase::getUpperLengthOfTheLongestPath(_board, _p1);
		length2 = goodEstimate ? CHeuristicBase::getLowerLengthOfTheLongestPath(_board, _p2) :
			CHeuristicBase::getUpperLengthOfTheLongestPath(_board, _p2);
		if (length1 > length2) {
			point = (length1 - length2)*POINTS / 2 / max(length1, length2);
			return PLAYER_1;
		}
		else if (length1 < length2) {
			point = (length2 - length1)*POINTS / 2 / max(length1, length2);
			return PLAYER_2;
		}
		else if (next == PLAYER_1) { // player 1 lose
			point = 1 * POINTS / 2 / (max(length1, length2) + 2);
			return PLAYER_2;
		}
		else { // player 1 win
			point = 1 * POINTS / 2 / (max(length1, length2) + 2);
			return PLAYER_1;
		}
	}
	point = 0;
	return OBSERVER;
}

int CHeuristicBase::simpleRateBoard(TBlock board[], const Pos2D &_p1, const Pos2D &_p2, const TPlayer next){
#ifdef _DEBUG
	TBlock backup[BOARD_SIZE];
	memcpy(backup, board, BOARD_SIZE*sizeof(TBlock));
#endif // _DEBUG

	// not a leaf node
	assert(next == PLAYER_1 || next == PLAYER_2);
	assert(getBlock(board, _p1) == BLOCK_PLAYER_1 && getBlock(board, _p2) == BLOCK_PLAYER_2);
	int result = 0;
	int n1 = 0, n2 = 0;

	static TBlock dBoard1[BOARD_SIZE], dBoard2[BOARD_SIZE];
	static TBlock board1[BOARD_SIZE], board2[BOARD_SIZE];

	memcpy(dBoard1, board, BOARD_SIZE*sizeof(TBlock));
	memcpy(dBoard2, board, BOARD_SIZE*sizeof(TBlock));
	fillDistance(dBoard1, _p1);
	fillDistance(dBoard2, _p2);

	memcpy(board1, board, BOARD_SIZE*sizeof(TBlock));
	memcpy(board2, board, BOARD_SIZE*sizeof(TBlock));

	for (int i = 0; i < BOARD_SIZE; i++){
		if (getBit(dBoard1[i], SPECIAL_BIT) && getBit(dBoard2[i], SPECIAL_BIT)){
			if (dBoard1[i] > dBoard2[i])
			{
				board1[i] = BLOCK_ENEMY_AREA;
			}
			else if (dBoard1[i] < dBoard2[i]){
				board2[i] = BLOCK_ENEMY_AREA;
			}
			else if (next == PLAYER_1){
				board2[i] = BLOCK_ENEMY_AREA;
			}
			else
				board1[i] = BLOCK_ENEMY_AREA;
		}
	}

	board1[_p2] = BLOCK_ENEMY_AREA;
	board2[_p1] = BLOCK_ENEMY_AREA;

	n1 = CBiconnectedComponents::getEstimatedLength(board1, _p1, Pos2D(-1, -1));
	n2 = CBiconnectedComponents::getEstimatedLength(board2, _p2, Pos2D(-1, -1));
	if (n1 + n2 == 0){
		if (next == PLAYER_1) // player 1 is lost
			return -POINTS / 2 - 1;
		else
			return POINTS / 2 + 1;
	}
	DEBUG(cout << "n1/n2 = " << n1 << "/" << n2 << endl);
	result = n1 *POINTS / (n1 + n2) - POINTS / 2;
#ifdef _DEBUG
	assert(memcmp(board, backup, BOARD_SIZE*sizeof(TBlock)) == 0);
#endif // _DEBUG

	return result;
}

int CHeuristicBase::voronoiRateBoard(TBlock board[], const Pos2D &_p1, const Pos2D &_p2, const TPlayer next)
{
#ifdef _DEBUG
	TBlock backup[BOARD_SIZE];
	memcpy(backup, board, BOARD_SIZE*sizeof(TBlock));
#endif // _DEBUG
	static CTranspositionTable *table = CTranspositionTable::getInstance();
	// not a leaf node
	assert(next == PLAYER_1 || next == PLAYER_2);
	assert(getBlock(board, _p1) == BLOCK_PLAYER_1 && getBlock(board, _p2) == BLOCK_PLAYER_2);

	int result = 0;
	int n1 = 0, n2 = 0;

	static TBlock dBoard1[BOARD_SIZE], dBoard2[BOARD_SIZE];
	memcpy(dBoard1, board, BOARD_SIZE*sizeof(TBlock));
	memcpy(dBoard2, board, BOARD_SIZE*sizeof(TBlock));

	bool visited[BOARD_SIZE] = { false };
	CFastPos1DDeque q1, q2, *q;
	int *n;
	q1.push_back(_p1); q2.push_back(_p2);
	visited[_p1] = visited[_p2] = true;
	TPlayer currentPlayer = next;
	int iV;

	while (!q1.empty() && !q2.empty()){
		if (currentPlayer == PLAYER_1){
			q = &q1;
			n = &n1;
			currentPlayer = PLAYER_2;
		}
		else{
			q = &q2;
			n = &n2;
			currentPlayer = PLAYER_1;
		}
		if (!q->empty()){
			Pos2D u(q->pop_front());

			for (int m = 1; m <= 4; m++){
				Pos2D v = u.move(m);
				iV = v;
				if (visited[iV])
					continue;

				if (getBlock(board, v) == BLOCK_EMPTY){
					visited[iV] = true;
					q->push_back(iV);
					*n = *n + 1;
				}
			}
		}
	}

	DEBUG(cout << "n1/n2 = " << n1 << "/" << n2 << endl);
	if (n1 + n2 != 0)
		result = n1 *POINTS / (n1 + n2) - POINTS / 2;
	else if (next == PLAYER_2) // player 1 win
		result = POINTS / 2 + 1;
	else
		result = -POINTS / 2 - 1;
#ifdef _DEBUG
	assert(memcmp(board, backup, BOARD_SIZE*sizeof(TBlock)) == 0);
#endif // _DEBUG

	return result;
}

int CHeuristicBase::pureTreeOfChamber(TBlock board[], const Pos2D &_p1, const Pos2D &_p2, const TPlayer next)
{
#ifdef _DEBUG
	TBlock backup[BOARD_SIZE];
	memcpy(backup, board, BOARD_SIZE*sizeof(TBlock));
#endif // _DEBUG
	TBlock gatesBoard[BOARD_SIZE];
	CArticulationPoints::getArticulationPoints(board, _p1, _p2, gatesBoard);

	// not isolated mode, not a leaf node
	assert(next == PLAYER_1 || next == PLAYER_2);
	assert(getBlock(board, _p1) == BLOCK_PLAYER_1 && getBlock(board, _p2) == BLOCK_PLAYER_2);
	int result = 0;
	int n1 = 0, n2 = 0;

	static TBlock board1[BOARD_SIZE], board2[BOARD_SIZE];
	memcpy(board1, board, BOARD_SIZE*sizeof(TBlock));
	memcpy(board2, board, BOARD_SIZE*sizeof(TBlock));
	vector<Pos1D> enemyOfP1, enemyOfP2;
	enemyOfP1.reserve(BOARD_SIZE);
	enemyOfP2.reserve(BOARD_SIZE);

	static TBlock dBoard1[BOARD_SIZE], dBoard2[BOARD_SIZE];
	memcpy(dBoard1, board, BOARD_SIZE*sizeof(TBlock));
	memcpy(dBoard2, board, BOARD_SIZE*sizeof(TBlock));
	fillDistance(dBoard1, _p1);
	fillDistance(dBoard2, _p2);
	for (int i = 0; i < BOARD_SIZE; i++){
		if (dBoard1[i] > SPECIAL_BLOCK && dBoard2[i] > SPECIAL_BLOCK){
			if (dBoard1[i] > dBoard2[i]) {
				board1[i] = BLOCK_ENEMY_AREA;
				enemyOfP1.push_back(i);
			}
			else if (dBoard1[i] < dBoard2[i]) {
				board2[i] = BLOCK_ENEMY_AREA;
				enemyOfP2.push_back(i);
			}
			else if (next == PLAYER_1) {
				board2[i] = BLOCK_ENEMY_AREA;
				enemyOfP2.push_back(i);
			}
			else {
				board1[i] = BLOCK_ENEMY_AREA;
				enemyOfP1.push_back(i);
			}
		}
	}
	board1[_p2] = BLOCK_ENEMY_AREA; enemyOfP1.push_back(_p2);
	board2[_p1] = BLOCK_ENEMY_AREA; enemyOfP2.push_back(_p1);

	static TBlock filledBoard1[BOARD_SIZE];
	fillChamberWithBattleFields(gatesBoard, board1, enemyOfP1, filledBoard1);
	int k1 = CBiconnectedComponents::getEstimatedLength(filledBoard1, _p1, Pos2D(-1, -1));

	static TBlock filledBoard2[BOARD_SIZE];
	fillChamberWithBattleFields(gatesBoard, board2, enemyOfP2, filledBoard2);
	int k2 = CBiconnectedComponents::getEstimatedLength(filledBoard2, _p2, Pos2D(-1, -1));

	int l1 = -1, l2 = -1;

	bool p1isInChamberWithBattleField = false;
	for (int i = 1; i <= 4; i++){
		if (getBlock(filledBoard1, _p1.move(i)) == BLOCK_ENEMY_AREA)
		{
			p1isInChamberWithBattleField = true;
			l1 = CBiconnectedComponents::getEstimatedLength(board1, _p1, Pos2D(-1, -1));
			break;
		}
	}

	bool p2isInChamberWithBattleField = false;
	for (int i = 1; i <= 4; i++){
		if (getBlock(filledBoard1, _p2.move(i)) == BLOCK_ENEMY_AREA)
		{
			p2isInChamberWithBattleField = true;
			l1 = CBiconnectedComponents::getEstimatedLength(board2, _p2, Pos2D(-1, -1));
			break;
		}
	}

	n1 = max(k1, l1);
	n2 = max(k2, l2);
	DEBUG(cout << "n1/n2 = " << n1 << "/" << n2 << endl);

	if (n1 + n2 == 0)
		if (next == PLAYER_2)
			result = POINTS / 2 + 1;
		else
			result = -POINTS / 2 - 1;
	else
		result = n1 *(POINTS) / (n1 + n2) - POINTS / 2;
#ifdef _DEBUG
	assert(memcmp(board, backup, BOARD_SIZE*sizeof(TBlock)) == 0);
#endif // _DEBUG
	return result;
}

void CHeuristicBase::sortMoves(vector<TMove> &moves, TBlock* board, const Pos2D &_p1, const Pos2D &_p2, const TPlayer next)
{
#ifdef _DEBUG
	TBlock backup[BOARD_SIZE];
	memcpy(backup, board, BOARD_SIZE*sizeof(TBlock));
#endif // _DEBUG
	static CMyAI* ai = CMyAI::getInstance();
	assert(moves.size() <= 4);
	int points[4];
	bool bOk;

	int i = 0;
	for (vector<TMove>::iterator m = moves.begin(); m != moves.end(); m++){
		if (next == PLAYER_1){
			bOk = move(board, _p1, *m, false); assert(bOk);
			points[i] = ai->searcher.heuristic.quickRateBoard(board, _p1.move(*m), _p2, PLAYER_2);
			bOk = move(board, _p1.move(*m), getOpositeDirection(*m), true); assert(bOk);
			if (points[i] == TIMEOUT_POINTS)
				return;
			assert(points[i] >= -MY_INFINITY && points[i] <= MY_INFINITY);
		}
		else{
			bOk = move(board, _p2, *m, false); assert(bOk);
			points[i] = ai->searcher.heuristic.quickRateBoard(board, _p1, _p2.move(*m), PLAYER_1);
			bOk = move(board, _p2.move(*m), getOpositeDirection(*m), true); assert(bOk);
			if (points[i] == TIMEOUT_POINTS)
				return;
			assert(points[i] >= -MY_INFINITY && points[i] <= MY_INFINITY);
		}

		i++;
	}
	for (unsigned int i = 0; i < moves.size(); i++){
		for (unsigned int j = i + 1; j < moves.size(); j++){
			if (next == PLAYER_1){
				if (points[j] > points[i]){
					int temp = points[i];
					points[i] = points[j];
					points[j] = temp;

					temp = moves[i];
					moves[i] = moves[j];
					moves[j] = temp;
				}
			}
			else {
				if (points[j] < points[i]){
					int temp = points[i];
					points[i] = points[j];
					points[j] = temp;

					temp = moves[i];
					moves[i] = moves[j];
					moves[j] = temp;
				}
			}
		}
	}
#ifdef _DEBUG
	assert(memcmp(board, backup, BOARD_SIZE*sizeof(TBlock)) == 0);
#endif // _DEBUG
}

void CHeuristicBase::sortMovesWithTT(vector<TMove> &moves, TBlock* board, const Pos2D &_p1, const Pos2D &_p2, const TPlayer next)
{
	static CTranspositionTable *table = CTranspositionTable::getInstance();
	static CMyAI* ai = CMyAI::getInstance();
	assert(moves.size() <= 4);
	int points[4];
	bool bOk;

	int i = 0;
	for (vector<TMove>::iterator m = moves.begin(); m != moves.end(); m++){
		if (next == PLAYER_1){
			bOk = move(board, _p1, *m, false); assert(bOk);
			points[i] = ai->searcher.alphaBetaWithTT(board, _p1.move(*m), _p2, PLAYER_2, 0, -MY_INFINITY, MY_INFINITY);
			bOk = move(board, _p1.move(*m), getOpositeDirection(*m), true); assert(bOk);
			if (points[i] == TIMEOUT_POINTS)
				return;
		}
		else{
			bOk = move(board, _p2, *m, false); assert(bOk);
			points[i] = ai->searcher.alphaBetaWithTT(board, _p1, _p2.move(*m), PLAYER_1, 0, -MY_INFINITY, MY_INFINITY);
			bOk = move(board, _p2.move(*m), getOpositeDirection(*m), true); assert(bOk);
			if (points[i] == TIMEOUT_POINTS)
				return;
		}

		i++;
	}
	for (unsigned int i = 0; i < moves.size(); i++){
		for (unsigned int j = i + 1; j < moves.size(); j++){
			if (next == PLAYER_1){
				if (points[j] > points[i]){
					int temp = points[i];
					points[i] = points[j];
					points[j] = temp;

					temp = moves[i];
					moves[i] = moves[j];
					moves[j] = temp;
				}
			}
			else {
				if (points[j] < points[i]){
					int temp = points[i];
					points[i] = points[j];
					points[j] = temp;

					temp = moves[i];
					moves[i] = moves[j];
					moves[j] = temp;
				}
			}
		}
	}
}

int CHeuristicBase::getLowerLengthOfTheLongestPath(TBlock const board[], const Pos2D &playerPos)
{
	TBlock board2[BOARD_SIZE];
	memcpy(board2, board, sizeof(TBlock)*BOARD_SIZE);
	Pos2D p = playerPos;

	int length = 0;
	vector<TMove> path; path.reserve(BOARD_SIZE);
	TMove m;
	while (true){
		m = CHeuristicBase::getFirstMoveOfTheLongestPath(board2, p, 1);
		if (m == 0)
			break;
		assert(m > 0 && m < 5);
		bool bOk = move(board2, p, m, false); assert(bOk);
		p = p.move(m);
		path.push_back(m);
	}
	return path.size();
}
