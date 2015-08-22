#pragma inline_depth(20)

#include "HeuristicBase.h"
#include "BiconnectedComponents.h"
#include "MyAI.h"
#include "MyTimer.h"
#include "GameState.h"
#include "TranspositionTable.h"
#include "ArticulationPoints.h"
#include "SmallDeque.h"


CHeuristicBase::CHeuristicBase(){}

CHeuristicBase::~CHeuristicBase(){}

TPoint CHeuristicBase::rateBoardTT(const TBlock _board[], const TPos &_p1, const TPos &_p2, const TPlayer next, const vector<TMove> &history)const
{
	static CTranspositionTable *tt = CTranspositionTable::getInstance();
	CGameState state(history);
	CGameState* ttEntry = tt->get(state);
	if (rateBoard != &CHeuristicBase::voronoiRateBoard)
	{
		if (ttEntry && ttEntry->simple != MY_INFINITY){
			return ttEntry->simple;
		}
		else {
			int result = rateBoard(_board, _p1, _p2, next);
			if (!ttEntry)
				ttEntry = tt->put(state);
			ttEntry->simple = result;
			return result;
		}
	}
	else {
		assert(rateBoard == &CHeuristicBase::voronoiRateBoard);
		if (ttEntry && ttEntry->vono != MY_INFINITY){
			return ttEntry->vono;
		}
		else {
			int result = rateBoard(_board, _p1, _p2, next);
			if (!ttEntry)
				ttEntry = tt->put(state);
			ttEntry->vono = result;
			return result;
		}
	}
}
TPoint CHeuristicBase::quickRateBoardTT(const TBlock _board[], const TPos &_p1, const TPos &_p2, const TPlayer next, const vector<TMove> &history)const
{
	static CTranspositionTable *tt = CTranspositionTable::getInstance();
	CGameState state(history);
	CGameState* ttEntry = tt->get(state);
	if (ttEntry && ttEntry->vono != MY_INFINITY){
		return ttEntry->vono;
	}
	else {
		int result = quickRateBoard(_board, _p1, _p2, next);
		if (!ttEntry)
			ttEntry = tt->put(state);
		ttEntry->vono = result;
		return result;
	}
}

TPoint CHeuristicBase::simpleRateBoard(const TBlock board[], const TPos &_p1, const TPos &_p2, const TPlayer next){
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
	CSmallDeque q1, q2, *q;
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
			TPos u = q->pop_front();
			for (int m = 1; m <= 4; m++){
				TPos v = MOVE(u, m);
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

	n1 = CBC::calculateLength(board1, _p1, false, ESTIMATE_ALL);
	assert(n1 != TIMEOUT_POINTS); // because exact == false
	n2 = CBC::calculateLength(board2, _p2, false, ESTIMATE_ALL);
	assert(n2 != TIMEOUT_POINTS); // because exact == false
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
TPoint CHeuristicBase::voronoiRateBoard(const TBlock _board[], const TPos &_p1, const TPos &_p2, const TPlayer next)
{
#ifdef _DEBUG
	TBlock backup[BOARD_SIZE];
	memcpy(backup, _board, BOARD_SIZE*sizeof(TBlock));
#endif // _DEBUG
	static CTranspositionTable *table = CTranspositionTable::getInstance();
	// not a leaf node
	assert(next == PLAYER_1 || next == PLAYER_2);
	assert(GET_BLOCK(_board, _p1) == BLOCK_PLAYER_1 && GET_BLOCK(_board, _p2) == BLOCK_PLAYER_2);

	TPoint result = 0;
	TPoint n1_[2], n2_[2]; // even first
	n1_[0] = n1_[1] = n2_[0] = n2_[1] = 0;

	TBlock board[BOARD_SIZE];
	memcpy(board, _board, sizeof(TBlock)*BOARD_SIZE);
	CSmallDeque q1, q2, *q;
	TPoint *n;
	q1.push_back(_p1); q2.push_back(_p2);
	TPlayer currentPlayer = next;

	while (!q1.empty() && !q2.empty()){
		if (currentPlayer == PLAYER_1){
			q = &q1;
			n = n1_;
			currentPlayer = PLAYER_2;
		}
		else{
			q = &q2;
			n = n2_;
			currentPlayer = PLAYER_1;
		}
		if (!q->empty()){
			TPos u = q->pop_front();

			for (TMove m = 1; m <= 4; m++){
				TPos v = MOVE(u, m);
				if (GET_BLOCK(board, v) == BLOCK_EMPTY){
					board[v] = BLOCK_OBSTACLE;
					q->push_back(v);
					if (v % 2 == 0)
						*n = *n + 1;
					else
						*(n + 1) = *(n + 1) + 1;
				}
			}
		}
	}

	TPoint n1, n2;
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
	assert(memcmp(_board, backup, BOARD_SIZE*sizeof(TBlock)) == 0);
#endif // _DEBUG

	return result;
}

TMove CHeuristicBase::getFirstMove(const TBlock _board[], const TPos &p, const EXACT_LEVEL exact, const int currentExactLength){
	assert(currentExactLength >= -1);
	auto moves = getAvailableMoves(_board, p);

	if (moves.size() == 0)
		return 0;
	else  if (moves.size() == 1)
		return moves[0];

	TBlock board[BOARD_SIZE];
	memcpy(board, _board, sizeof(TBlock)*BOARD_SIZE);
	bool bOk;

	int points[4];
	int max = -1;
	for (unsigned int i = 0; i < moves.size(); i++){
		TMove m = moves[i];
		bOk = move(board, p, m, false); assert(bOk);
		points[i] = CBC::calculateLength(board, MOVE(p, m), false, exact);
		if (points[i] == TIMEOUT_POINTS)
			return 0;
		if (currentExactLength > 0 && points[i] >= currentExactLength - 1)
			return m;
		bOk = move(board, MOVE(p, m), getOpositeDirection(m), true); assert(bOk);
		if (max < points[i])
			max = points[i];
	}

	for (unsigned int i = 0; i < moves.size(); i++){
		if (max == points[i])
			return moves[i];
	}
	return 0;
}
TMove CHeuristicBase::getExactFirstMove(const TBlock _board[], const TPos &p, const int currentExactLength){
	assert(currentExactLength >= 0);
	auto moves = getAvailableMoves(_board, p);

	if (moves.size() == 0)
	{
		assert(currentExactLength == 0);
		return 0;
	}
	else  if (moves.size() == 1)
	{
		assert(currentExactLength > 0);
		return moves[0];
	}

	TBlock board[BOARD_SIZE];
	memcpy(board, _board, sizeof(TBlock)*BOARD_SIZE);
	bool bOk;

	for (unsigned int i = 0; i < moves.size(); i++){
		TMove m = moves[i];
		bOk = move(board, p, m, false); assert(bOk);
		int point = CBC::calculateLength(board, MOVE(p, m), false, EXACT);
		bOk = move(board, MOVE(p, m), getOpositeDirection(m), true); assert(bOk);

		if (point == currentExactLength - 1)
			return m;
	}

	assert(false);
	system("pause");
	return 0;
}

int CHeuristicBase::getLowerLengthOfTheLongestPath(TBlock const board[], const TPos &playerPos)
{
	TBlock board2[BOARD_SIZE];
	memcpy(board2, board, sizeof(TBlock)*BOARD_SIZE);
	TPos p = playerPos;

	int length = 0;
	vector<TMove> path; path.reserve(BOARD_SIZE);
	TMove m;
	while (true){
		m = CHeuristicBase::getFirstMove(board2, p, EXACT_AREA_BELOW_25, -1);
		if (m == 0)
			break;
		assert(m > 0 && m < 5);
		bool bOk = move(board2, p, m, false); assert(bOk);
		p = MOVE(p, m);
		path.push_back(m);
	}
	return path.size();
}

TPoint CHeuristicBase::evaluateBoardTT(const TBlock _board[], const TPos &_p1, const TPos &_p2, const TPlayer next, const vector<TMove> &history) {
	assert(GET_BLOCK(_board, _p1) == BLOCK_PLAYER_1);
	assert(GET_BLOCK(_board, _p2) == BLOCK_PLAYER_2);
	static CMyAI* pAI = CMyAI::getInstance();
	static CTranspositionTable *tt = CTranspositionTable::getInstance();
	CGameState state(history);
	CGameState* ttEntry = tt->get(state);
	static CBCO o1, o2;
	if (!ttEntry)
		ttEntry = tt->put(state);
	bool init1 = false, init2 = false;

	// not isolation mode 
	// check calculated results
	if (ttEntry->length1 == -2){ // calculated, not in isolation mode
		assert(ttEntry->length2 == -2);
		return 0;
	}

	int length1, length2;
	EXACT_LEVEL exact1, exact2;
	EXACT_LEVEL exactLevel2;

	if (ttEntry->length1 == -1){
		assert(ttEntry->length2 == -1);
		// have not calculated anything
		if (!isIsolated(_board, _p1, _p2)){
			ttEntry->length1 = ttEntry->length2 = -2;
			return 0;
		}
		else {
			CBC::calculateBCs(_board, &o1, _p1); init1 = true;
			ttEntry->length1 = length1 = o1.findLengthOfLongestPath(EXACT_AREA_BELOW_10);
			ttEntry->exact1 = o1.outputExact;
			assert(ttEntry->exact1 >= EXACT_AREA_BELOW_10);
			CBC::calculateBCs(_board, &o2, _p2); init2 = true;
			ttEntry->length2 = length2 = o2.findLengthOfLongestPath(EXACT_AREA_BELOW_10);
			ttEntry->exact2 = o2.outputExact;
			assert(ttEntry->exact2 >= EXACT_AREA_BELOW_10);
			if (abs(length2 - length1) < 4){
				ttEntry->length1 = length1 = o1.findLengthOfLongestPath(EXACT_AREA_BELOW_25);
				ttEntry->exact1 = o1.outputExact;
				assert(ttEntry->exact1 >= EXACT_AREA_BELOW_25);
				ttEntry->length2 = length2 = o2.findLengthOfLongestPath(EXACT_AREA_BELOW_25);
				ttEntry->exact2 = o2.outputExact;
				assert(ttEntry->exact2 >= EXACT_AREA_BELOW_25);
			}
		}
	}

	// isolation mode
	if (pAI->foundAnEnd)
		exactLevel2 = EXACT;
	else
		exactLevel2 = EXACT_AREA_BELOW_25;

	length1 = ttEntry->length1;
	length2 = ttEntry->length2;
	exact1 = (EXACT_LEVEL)ttEntry->exact1;
	exact2 = (EXACT_LEVEL)ttEntry->exact2;

	assert(length1 >= 0 && length2 >= 0);

	if (abs(length1 - length2) < 4 && exactLevel2 == EXACT){
		if (exact1 < EXACT)
		{
			if (!init1){
				CBC::calculateBCs(_board, &o1, _p1);
				init1 = true;
			}
			length1 = o1.findLengthOfLongestPath(EXACT);
			if (length1 != TIMEOUT_POINTS){
				ttEntry->length1 = length1;
				ttEntry->exact1 = EXACT;
				assert(o1.outputExact == EXACT);
			}
			else {
				length1 = ttEntry->length1;
			}
		}

		if (exact2 < EXACT)
		{
			if (!init2){
				CBC::calculateBCs(_board, &o2, _p2);
				init2 = true;
			}
			length2 = o2.findLengthOfLongestPath(EXACT);
			if (length2 != TIMEOUT_POINTS){
				ttEntry->length2 = length2;
				ttEntry->exact2 = EXACT;
				assert(o2.outputExact == EXACT);
			}
			else {
				length2 = ttEntry->length2;
			}
		}
	}

	// we've got length 1 and length 2
	if (length1 > length2){
		return POINTS / 2 + length1 - length2;
	}
	else if (length1 < length2){
		return -POINTS / 2 + length1 - length2;
	}
	else if (next == PLAYER_2){
		return POINTS / 2 + 1;
	}
	else
		return -POINTS / 2 - 1;
}

void CHeuristicBase::sortMoves(vector<TMove> &moves, TBlock* board, const TPos &_p1, const TPos &_p2, const TPlayer next, vector<TMove> &history)
{
#ifdef _DEBUG
	TBlock backup[BOARD_SIZE];
	memcpy(backup, board, BOARD_SIZE*sizeof(TBlock));
#endif // _DEBUG
	static CMyAI* ai = CMyAI::getInstance();
	assert(moves.size() <= 4);
	if (moves.size() == 1)
		return;
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