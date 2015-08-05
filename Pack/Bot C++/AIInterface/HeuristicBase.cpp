#pragma inline_depth(20)

#include "HeuristicBase.h"
#include "BiconnectedComponents.h"
#include "MyAI.h"
#include "MyTimer.h"
#include "GameState.h"
#include "TranspositionTable.h"
#include "ArticulationPoints.h"
#include "FastPos1DDeque.h"


CHeuristicBase::CHeuristicBase(){}

CHeuristicBase::~CHeuristicBase(){}


int CHeuristicBase::rateBoardTT(TBlock _board[], const Pos1D &_p1, const Pos1D &_p2, const TPlayer next, const vector<TMove> &history)
{
	static CTranspositionTable *tt = CTranspositionTable::getInstance();
	CGameState state(history);
	CGameState* ttEntry = tt->get(state);
	if (rateBoard != &CHeuristicBase::voronoiRateBoard)
	{
		if (ttEntry && ttEntry->lowerbound != -MY_INFINITY){
			return ttEntry->lowerbound;
		}
		else {
			int result = rateBoard(_board, _p1, _p2, next);
			if (!ttEntry)
				ttEntry = tt->put(state);
			ttEntry->lowerbound = result;
			return result;
		}
	}
	else {
		assert(rateBoard == &CHeuristicBase::voronoiRateBoard);
		if (ttEntry && ttEntry->upperBound != MY_INFINITY){
			return ttEntry->upperBound;
		}
		else {
			int result = rateBoard(_board, _p1, _p2, next);
			if (!ttEntry)
				ttEntry = tt->put(state);
			ttEntry->upperBound = result;
			return result;
		}
	}


}

int CHeuristicBase::quickRateBoardTT(TBlock _board[], const Pos1D &_p1, const Pos1D &_p2, const TPlayer next, const vector<TMove> &history)
{
	static CTranspositionTable *tt = CTranspositionTable::getInstance();
	CGameState state(history);
	CGameState* ttEntry = tt->get(state);
	if (ttEntry && ttEntry->upperBound != MY_INFINITY){
		return ttEntry->upperBound;
	}
	else {
		int result = quickRateBoard(_board, _p1, _p2, next);
		if (!ttEntry)
			ttEntry = tt->put(state);
		ttEntry->upperBound = result;
		return result;
	}
}

void CHeuristicBase::exploreToPathLongestPath(TBlock board[], Pos1D &pos, vector<TMove> &cPath, vector<TMove> &oldPath, int &oldL, int depth)
{
	assert(GET_BLOCK(board, pos) == BLOCK_PLAYER_1 || GET_BLOCK(board, pos) == BLOCK_PLAYER_2);
	assert(depth >= 0);
	auto availMoves = getAvailableMoves(board, pos);
	bool bOk;
	if (availMoves.size() == 0 || depth == 0){
		int l;
		if (depth == 0)
			l = CHeuristicBase::getUpperLengthOfTheLongestPath(board, pos);
		else l = 0;
		if (cPath.size() + l > oldPath.size() + oldL){
			oldL = l;
			oldPath = vector<TMove>(cPath);
		}
		else if (cPath.size() + l == oldPath.size() + oldL && oldL > l){
			oldL = l;
			oldPath = vector<TMove>(cPath);
		}
	}
	else {
		for (unsigned int iMove = 0; iMove < availMoves.size(); iMove++){
			TMove m = availMoves[iMove];
			cPath.push_back(m);
			bOk = move(board, pos, m, false); assert(bOk);
			pos = MOVE(pos, m);

			exploreToPathLongestPath(board, pos, cPath, oldPath, oldL, depth - 1);

			bOk = move(board, pos, getOpositeDirection(m), true); assert(bOk);
			pos = MOVE(pos, getOpositeDirection(m));
			cPath.pop_back();
		}
	}
}

TMove CHeuristicBase::getFirstMoveOfTheLongestPath(const TBlock boardData[], const Pos1D &pos, const int nDepth)
{
	assert(nDepth > 0);
	auto t = getAvailableMoves(boardData, pos);

	if (t.size() == 0)
		return 0;
	else  if (t.size() == 1)
		return t[0];
	return getTheLongestPath(boardData, pos, nDepth)[0];
}

vector<TMove> CHeuristicBase::getTheLongestPath(const TBlock _board[], const Pos1D &pos, const int nDepth)
{
	TBlock board[BOARD_SIZE];
	memcpy(board, _board, BOARD_SIZE*sizeof(TBlock));
	vector<TMove> c, l;
	c.reserve(BOARD_SIZE);
	l.reserve(BOARD_SIZE);
	int length = 0;
	auto t = getAvailableMoves(_board, pos);

	if (t.size() == 0)
		return l;

	Pos1D p = pos;
	exploreToPathLongestPath(board, p, c, l, length, nDepth);
	assert(l.size() > 0);
	return l;
}

int CHeuristicBase::evaluateBoard(const TBlock _board[], const Pos1D &_p1, const Pos1D &_p2, const TPlayer next, int &point) {
	assert(GET_BLOCK(_board, _p1) == BLOCK_PLAYER_1);
	assert(GET_BLOCK(_board, _p2) == BLOCK_PLAYER_2);
	int length1, length2;
	if (isIsolated(_board, _p1, _p2)){
		length1 = CHeuristicBase::getUpperLengthOfTheLongestPath(_board, _p1);
		length2 = CHeuristicBase::getUpperLengthOfTheLongestPath(_board, _p2);
		if (abs(length2 - length1) < 6)
		{
			length1 = CHeuristicBase::getLowerLengthOfTheLongestPath(_board, _p1);
			length2 = CHeuristicBase::getLowerLengthOfTheLongestPath(_board, _p2);
		}
		if (length1 > length2) {
			point = (length1 - length2);
			return PLAYER_1;
		}
		else if (length1 < length2) {
			point = (length2 - length1);
			return PLAYER_2;
		}
		else if (next == PLAYER_1) { // player 1 lose
			point = 1;
			return PLAYER_2;
		}
		else { // player 1 win
			point = 1;
			return PLAYER_1;
		}
	}
	point = 0;
	return OBSERVER;
}

int CHeuristicBase::simpleRateBoard(TBlock board[], const Pos1D &_p1, const Pos1D &_p2, const TPlayer next){
#ifdef _DEBUG
	TBlock backup[BOARD_SIZE];
	memcpy(backup, board, BOARD_SIZE*sizeof(TBlock));
#endif // _DEBUG

	// not a leaf node
	assert(next == PLAYER_1 || next == PLAYER_2);
	assert(GET_BLOCK(board, _p1) == BLOCK_PLAYER_1 && GET_BLOCK(board, _p2) == BLOCK_PLAYER_2);
	int result = 0;
	int n1 = 0, n2 = 0;

	TBlock board1[BOARD_SIZE], board2[BOARD_SIZE], *b;
	memcpy(board1, board, BOARD_SIZE*sizeof(TBlock));
	memcpy(board2, board, BOARD_SIZE*sizeof(TBlock));

	bool visited[BOARD_SIZE] = { false };
	CFastPos1DDeque q1, q2, *q;
	q1.push_back(_p1); q2.push_back(_p2);
	visited[_p1] = visited[_p2] = true;
	TPlayer currentPlayer = next;

	while (!q1.empty() && !q2.empty()){
		if (currentPlayer == PLAYER_1){
			q = &q1;
			b = board2;
			currentPlayer = PLAYER_2;
		}
		else{
			q = &q2;
			b = board1;
			currentPlayer = PLAYER_1;
		}
		if (!q->empty()){
			Pos1D u = q->pop_front();
			for (int m = 1; m <= 4; m++){
				Pos1D v = MOVE(u, m);
				if (GET_BLOCK(board, v) == BLOCK_EMPTY && !visited[v]){
					visited[v] = true;
					q->push_back(v);
					b[v] = BLOCK_ENEMY_AREA;
				}
			}
		}
	}

	board1[_p2] = BLOCK_ENEMY_AREA;
	board2[_p1] = BLOCK_ENEMY_AREA;

	n1 = CBiconnectedComponents::getEstimatedLength(board1, _p1, -1);
	n2 = CBiconnectedComponents::getEstimatedLength(board2, _p2, -1);
	if (n1 + n2 == 0){
		if (next == PLAYER_1) // player 1 is lost
			return -POINTS / 2 - 1;
		else
			return POINTS / 2 + 1;
	}
	DEBUG(cout << "n1/n2 = " << n1 << "/" << n2 << endl);
	result = n1 - n2;
#ifdef _DEBUG
	assert(memcmp(board, backup, BOARD_SIZE*sizeof(TBlock)) == 0);
#endif // _DEBUG

	return result;
}

int CHeuristicBase::voronoiRateBoard(TBlock board[], const Pos1D &_p1, const Pos1D &_p2, const TPlayer next)
{
#ifdef _DEBUG
	TBlock backup[BOARD_SIZE];
	memcpy(backup, board, BOARD_SIZE*sizeof(TBlock));
#endif // _DEBUG
	static CTranspositionTable *table = CTranspositionTable::getInstance();
	// not a leaf node
	assert(next == PLAYER_1 || next == PLAYER_2);
	assert(GET_BLOCK(board, _p1) == BLOCK_PLAYER_1 && GET_BLOCK(board, _p2) == BLOCK_PLAYER_2);

	int result = 0;
	int n1_[2], n2_[2];

	static TBlock dBoard1[BOARD_SIZE], dBoard2[BOARD_SIZE];
	memcpy(dBoard1, board, BOARD_SIZE*sizeof(TBlock));
	memcpy(dBoard2, board, BOARD_SIZE*sizeof(TBlock));

	bool visited[BOARD_SIZE] = { false };
	CFastPos1DDeque q1, q2, *q;
	int *n;
	q1.push_back(_p1); q2.push_back(_p2);
	visited[_p1] = visited[_p2] = true;
	TPlayer currentPlayer = next;

	while (!q1.empty() && !q2.empty()){
		if (currentPlayer == PLAYER_1){
			q = &q1;
			n = &n1_[0];
			currentPlayer = PLAYER_2;
		}
		else{
			q = &q2;
			n = &n2_[0];
			currentPlayer = PLAYER_1;
		}
		if (!q->empty()){
			Pos1D u = q->pop_front();

			for (int m = 1; m <= 4; m++){
				Pos1D v = MOVE(u, m);
				if (GET_BLOCK(board, v) == BLOCK_EMPTY && !visited[v]){
					visited[v] = true;
					q->push_back(v);
					if (v % 2 == 0)
						*n = *n + 1;
					else
						*(n + 1) = *(n + 1) + 1;
				}
			}
		}
	}

	DEBUG(cout << "n1/n2 = " << n1_[0] << "/" << n2_[0] << endl);
	int n1, n2;
	if (_p1 % 2 == 0){ // start with odd
		if (n1_[1] > n1_[0])
			n1 = n1_[0] * 2 + 1;
		else
			n1 = n1_[1] * 2;
	}
	else {
		if (n1_[0] > n1_[1]) // start with even
			n1 = n1_[1] * 2 + 1;
		else
			n1 = n1_[0] * 2;
	}

	if (_p2 % 2 == 0){ // start with odd
		if (n2_[1] > n2_[0])
			n2 = n2_[0] * 2 + 1;
		else
			n2 = n2_[1] * 2;
	}
	else {
		if (n2_[0] > n2_[1]) // start with even
			n2 = n2_[1] * 2 + 1;
		else
			n2 = n2_[0] * 2;
	}

	if (n1 + n2 != 0)
		result = n1 - n2;
	else if (next == PLAYER_2) // player 1 win
		result = POINTS / 2 + 1;
	else
		result = -POINTS / 2 - 1;
#ifdef _DEBUG
	assert(memcmp(board, backup, BOARD_SIZE*sizeof(TBlock)) == 0);
#endif // _DEBUG

	return result;
}

int CHeuristicBase::pureTreeOfChamber(TBlock board[], const Pos1D &_p1, const Pos1D &_p2, const TPlayer next)
{
#ifdef _DEBUG
	TBlock backup[BOARD_SIZE];
	memcpy(backup, board, BOARD_SIZE*sizeof(TBlock));
#endif // _DEBUG
	TBlock gatesBoard[BOARD_SIZE];
	CArticulationPoints::getArticulationPoints(board, _p1, _p2, gatesBoard);

	// not isolated mode, not a leaf node
	assert(next == PLAYER_1 || next == PLAYER_2);
	assert(GET_BLOCK(board, _p1) == BLOCK_PLAYER_1 && GET_BLOCK(board, _p2) == BLOCK_PLAYER_2);
	int result = 0;
	int n1 = 0, n2 = 0;

	CFastPos1DDeque area2, area1;
	TBlock board1[BOARD_SIZE], board2[BOARD_SIZE];
	{
		TBlock *b;
		memcpy(board1, board, BOARD_SIZE*sizeof(TBlock));
		memcpy(board2, board, BOARD_SIZE*sizeof(TBlock));
		bool visited[BOARD_SIZE] = { false };
		CFastPos1DDeque *q;
		area1.push_back(_p1); area2.push_back(_p2);
		visited[_p1] = visited[_p2] = true;
		TPlayer currentPlayer = next;

		while (!area1.empty() && !area2.empty()){
			if (currentPlayer == PLAYER_1){
				q = &area1;
				b = board2;
				currentPlayer = PLAYER_2;
			}
			else{
				q = &area2;
				b = board1;
				currentPlayer = PLAYER_1;
			}
			if (!q->empty()){
				Pos1D u = q->pop_front();
				for (int m = 1; m <= 4; m++){
					Pos1D v = MOVE(u, m);
					if (GET_BLOCK(board, v) == BLOCK_EMPTY && !visited[v]){
						visited[v] = true;
						q->push_back(v);
						b[v] = BLOCK_ENEMY_AREA;
					}
				}
			}
		}
	}
	board1[_p2] = BLOCK_ENEMY_AREA; area2.push_back(_p2);
	board2[_p1] = BLOCK_ENEMY_AREA; area1.push_back(_p1);

	static TBlock filledBoard1[BOARD_SIZE];
	fillChamberWithBattleFields(gatesBoard, board1, area2, filledBoard1);
	int k1 = CBiconnectedComponents::getEstimatedLength(filledBoard1, _p1, -1);

	static TBlock filledBoard2[BOARD_SIZE];
	fillChamberWithBattleFields(gatesBoard, board2, area1, filledBoard2);
	int k2 = CBiconnectedComponents::getEstimatedLength(filledBoard2, _p2, -1);

	int l1 = -1, l2 = -1;

	bool p1isInChamberWithBattleField = false;
	for (int i = 1; i <= 4; i++){
		if (GET_BLOCK(filledBoard1, MOVE(_p1, i)) == BLOCK_ENEMY_AREA)
		{
			p1isInChamberWithBattleField = true;
			l1 = CBiconnectedComponents::getEstimatedLength(board1, _p1, -1);
			break;
		}
	}

	bool p2isInChamberWithBattleField = false;
	for (int i = 1; i <= 4; i++){
		if (GET_BLOCK(filledBoard1, MOVE(_p2, i)) == BLOCK_ENEMY_AREA)
		{
			p2isInChamberWithBattleField = true;
			l1 = CBiconnectedComponents::getEstimatedLength(board2, _p2, -1);
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

void CHeuristicBase::sortMoves(vector<TMove> &moves, TBlock* board, const Pos1D &_p1, const Pos1D &_p2, const TPlayer next, vector<TMove> &history)
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
			history.push_back(*m);
			points[i] = ai->searcher.heuristic.quickRateBoardTT(board, MOVE(_p1, *m), _p2, PLAYER_2, history);
			bOk = move(board, MOVE(_p1, *m), getOpositeDirection(*m), true); assert(bOk);
			history.pop_back();
			if (points[i] == TIMEOUT_POINTS)
				return;
			assert(points[i] >= -MY_INFINITY && points[i] <= MY_INFINITY);
		}
		else{
			bOk = move(board, _p2, *m, false); assert(bOk);
			history.push_back(*m);
			points[i] = ai->searcher.heuristic.quickRateBoardTT(board, _p1, MOVE(_p2, *m), PLAYER_1, history);
			bOk = move(board, MOVE(_p2, *m), getOpositeDirection(*m), true); assert(bOk);
			history.pop_back();
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

void CHeuristicBase::sortMovesWithTT(vector<TMove> &moves, TBlock* board, const Pos1D &_p1, const Pos1D &_p2, const TPlayer next, vector<TMove> &history)
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
			history.push_back(*m);
			points[i] = ai->searcher.alphaBetaTT(board, MOVE(_p1, *m), _p2, PLAYER_2, history, 0, -MY_INFINITY, MY_INFINITY);
			bOk = move(board, MOVE(_p1, *m), getOpositeDirection(*m), true); assert(bOk);
			history.pop_back();
			if (points[i] == TIMEOUT_POINTS)
				return;
		}
		else{
			bOk = move(board, _p2, *m, false); assert(bOk);
			history.push_back(*m);
			points[i] = ai->searcher.alphaBetaTT(board, _p1, MOVE(_p2, *m), PLAYER_1, history, 0, -MY_INFINITY, MY_INFINITY);
			bOk = move(board, MOVE(_p2, *m), getOpositeDirection(*m), true); assert(bOk);
			history.pop_back();
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

int CHeuristicBase::getLowerLengthOfTheLongestPath(TBlock const board[], const Pos1D &playerPos)
{
	TBlock board2[BOARD_SIZE];
	memcpy(board2, board, sizeof(TBlock)*BOARD_SIZE);
	Pos1D p = playerPos;

	int length = 0;
	vector<TMove> path; path.reserve(BOARD_SIZE);
	TMove m;
	while (true){
		m = CHeuristicBase::getFirstMoveOfTheLongestPath(board2, p, 1);
		if (m == 0)
			break;
		assert(m > 0 && m < 5);
		bool bOk = move(board2, p, m, false); assert(bOk);
		p = MOVE(p, m);
		path.push_back(m);
	}
	return path.size();
}

int CHeuristicBase::getUpperLengthOfTheLongestPath(TBlock const board[], const Pos1D &playerPos, const int depth)
{
	return CBiconnectedComponents::getEstimatedLength(board, playerPos, depth);
}

