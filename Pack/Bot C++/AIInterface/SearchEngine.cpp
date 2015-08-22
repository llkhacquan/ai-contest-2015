#include "SearchEngine.h"
#include "HeuristicBase.h"
#include "MyAI.h"
#include "GameState.h"
#include "TranspositionTable.h"
#include "StaticFunctions.h"
#include "MyTimer.h"


CSearchEngine::CSearchEngine(){}


CSearchEngine::~CSearchEngine(){}


TPoint CSearchEngine::alphaBeta(TBlock board[], const TPos&_p1, const TPos&_p2, const TPlayer next, vector<TMove> &history,
	int depth, TPoint a, TPoint b)
{
#ifdef _DEBUG
	TBlock backup[BOARD_SIZE];
	memcpy(backup, board, BOARD_SIZE*sizeof(TBlock));
	vector<TMove> _history(history);
#endif // _DEBUG
	assert((GET_BLOCK(board, _p1)) == BLOCK_PLAYER_1);
	assert((GET_BLOCK(board, _p2)) == BLOCK_PLAYER_2);
	assert(next == PLAYER_1 || next == PLAYER_2);
	static CMyAI* pAI = CMyAI::getInstance();
	static CMyTimer *timer = CMyTimer::getInstance();
	if (pAI->shouldEndMoveNow())
		return TIMEOUT_POINTS;

	bool bOk;
	TPoint bestValue = -MY_INFINITY;

	TPoint point = CHeuristicBase::evaluateBoardTT(board, _p1, _p2, next, history);
	if (point == TIMEOUT_POINTS)
		return point;
	if (point != 0)
	{
		assert(point > POINTS / 2 || point < -POINTS / 2);
		return point;
	}

	if (depth == 0)
	{
		TPoint t = heuristic.rateBoardTT(board, _p1, _p2, next, history);
		assert(abs(t) <= MY_INFINITY);
		return t;
	}

	vector<TMove> moves;
	moves = next == PLAYER_1 ? getAvailableMoves(board, _p1) : getAvailableMoves(board, _p2);

	CHeuristicBase::sortMoves(moves, board, _p1, _p2, next, history);

	TPoint value;
	TPoint ab;
	if (next == PLAYER_1){
		value = -MY_INFINITY;
		for (auto m = moves.begin(); m != moves.end(); m++){
			bOk = move(board, _p1, *m, false); assert(bOk);
			history.push_back(*m);
			value = max(value, ab = alphaBeta(board, MOVE(_p1, *m), _p2, PLAYER_2, history, depth - 1, a, b));
			bOk = move(board, MOVE(_p1, *m), getOpositeDirection(*m), true); assert(bOk);
			history.pop_back();
			if (ab == TIMEOUT_POINTS)
				return TIMEOUT_POINTS;
			assert(ab >= -MY_INFINITY && ab <= MY_INFINITY);
			a = max(value, a);
			if (b <= a)
				break;
		}
	}
	else {
		value = MY_INFINITY;
		for (auto m = moves.begin(); m != moves.end(); m++){
			bOk = move(board, _p2, *m, false); assert(bOk);
			history.push_back(*m);
			value = min(value, ab = alphaBeta(board, _p1, MOVE(_p2, *m), PLAYER_1, history, depth - 1, a, b));
			bOk = move(board, MOVE(_p2, *m), getOpositeDirection(*m), true); assert(bOk);
			history.pop_back();
			if (ab == TIMEOUT_POINTS)
				return TIMEOUT_POINTS;
			assert(ab >= -MY_INFINITY && ab <= MY_INFINITY);
			b = min(value, b);
			if (b <= a)
				break;
		}
	}

#ifdef _DEBUG
	assert(memcmp(board, backup, BOARD_SIZE*sizeof(TBlock)) == 0);
	assert(_history.size() == history.size());
	assert(equal(_history.begin(), _history.end(), history.begin()));
#endif // _DEBUG
	assert(value >= -MY_INFINITY && value <= MY_INFINITY);
	return value;
}

TPoint CSearchEngine::alphaBetaTT(TBlock board[], const TPos&_p1, const TPos&_p2, const TPlayer next, vector<TMove> &history,
	int depth, TPoint alpha, TPoint beta)
{
#ifdef _DEBUG
	TBlock backup[BOARD_SIZE];
	memcpy(backup, board, BOARD_SIZE*sizeof(TBlock));
	vector<TMove> _history(history);
#endif // _DEBUG
	assert((GET_BLOCK(board, _p1)) == BLOCK_PLAYER_1);
	assert((GET_BLOCK(board, _p2)) == BLOCK_PLAYER_2);
	assert(next == PLAYER_1 || next == PLAYER_2);
	static CMyTimer *timer = CMyTimer::getInstance();
	static CMyAI* pAI = CMyAI::getInstance();
	if (pAI->shouldEndMoveNow())
		return TIMEOUT_POINTS;
	static CTranspositionTable *tt = CTranspositionTable::getInstance();
	CGameState state(history);
	state.depth = depth;
	CGameState* ttEntry = tt->get(state);
	if (ttEntry && ttEntry->depth >= depth){
		if (ttEntry->lower >= beta)
		{
			return ttEntry->lower;
		}
		if (ttEntry->upper <= alpha)
		{
			return ttEntry->upper;
		}
		alpha = max(alpha, ttEntry->lower);
		beta = min(beta, ttEntry->upper);
	}
	bool bOk;
	TPoint g;
	TPoint point = CHeuristicBase::evaluateBoardTT(board, _p1, _p2, next, history);
	if (point != 0)	{
		g = point;
	}
	else if (depth == 0)
	{
		g = heuristic.rateBoardTT(board, _p1, _p2, next, history);
	}
	else {
		vector<TMove> moves;
		moves = next == PLAYER_1 ? getAvailableMoves(board, _p1) : getAvailableMoves(board, _p2);
		CHeuristicBase::sortMoves(moves, board, _p1, _p2, next, history);
		TPoint ab;
		if (next == PLAYER_1){
			g = -MY_INFINITY;
			TPoint a = alpha;
			for (auto m = moves.begin(); m != moves.end(); m++){
				bOk = move(board, _p1, *m, false); assert(bOk);
				history.push_back(*m);
				g = max(g, ab = alphaBetaTT(board, MOVE(_p1, *m), _p2, PLAYER_2, history, depth - 1, a, beta));
				bOk = move(board, MOVE(_p1, *m), getOpositeDirection(*m), true); assert(bOk);
				history.pop_back();
				if (ab == TIMEOUT_POINTS)
					return TIMEOUT_POINTS;
				assert(ab >= -MY_INFINITY && ab <= MY_INFINITY);
				a = max(g, a);
			}
		}
		else {
			g = MY_INFINITY;
			TPoint b = beta;
			for (auto m = moves.begin(); m != moves.end(); m++){
				bOk = move(board, _p2, *m, false); assert(bOk);
				history.push_back(*m);
				g = min(g, ab = alphaBetaTT(board, _p1, MOVE(_p2, *m), PLAYER_1, history, depth - 1, alpha, b));
				bOk = move(board, MOVE(_p2, *m), getOpositeDirection(*m), true); assert(bOk);
				history.pop_back();
				if (ab == TIMEOUT_POINTS)
					return TIMEOUT_POINTS;
				assert(ab >= -MY_INFINITY && ab <= MY_INFINITY);
				b = min(g, b);
			}
		}
	}

	if (!ttEntry)
		ttEntry = tt->put(state);
	if (g <= alpha)
		ttEntry->upper = g;
	if (g > alpha && g < beta){
		ttEntry->lower = g;
		ttEntry->upper = g;
	}
	if (g >= beta)
		ttEntry->lower = g;
	ttEntry->depth = depth;

#ifdef _DEBUG
	assert(memcmp(board, backup, BOARD_SIZE*sizeof(TBlock)) == 0);
	assert(_history.size() == history.size());
	assert(equal(_history.begin(), _history.end(), history.begin()));
#endif // _DEBUG

	return g;
}

TPoint CSearchEngine::mtdF(TBlock board[], const TPos&_p1, const TPos&_p2, const TPlayer next, vector<TMove> &history,
	TPoint f, int depth)
{
	TPoint beta;
	TPoint g = f;
	TPoint upperBound = MY_INFINITY;
	TPoint lowerBound = -MY_INFINITY;
	do {
		if (g == lowerBound)
			beta = g + 1;
		else
			beta = g;
		g = alphaBetaTT(board, _p1, _p2, next, history, depth, beta - 1, beta);
		if (g == TIMEOUT_POINTS)
			return g;
		if (g < beta)
			upperBound = g;
		else
			lowerBound = g;
	} while (lowerBound < upperBound);
	return g;
}

TPoint CSearchEngine::mtdfIterativeDeepening(TBlock board[], const TPos&_p1, const TPos&_p2, const TPlayer next, vector<TMove> &history,
	int maxDepth)
{
	static CTranspositionTable *table = CTranspositionTable::getInstance();
	startTime = clock();
	int firstGuest = 0;
	int d = 1;
	for (d = MIN_DEPTH; d <= maxDepth; d++){
		int lastGuest = mtdF(board, _p1, _p2, next, history, firstGuest, d);
		if (lastGuest == TIMEOUT_POINTS)
			break;
		else
			firstGuest = lastGuest;
	}
	cout << "\t\tThe reached depth = " << d << endl;
	return firstGuest;
}

pair<TMove, TPoint> CSearchEngine::getOptimalMoveByAB(TBlock board[], const TPos&_p1, const TPos&_p2, const TPlayer next, const vector<TMove> &_history, int depth)
{
#ifdef _DEBUG
	TBlock backup[BOARD_SIZE];
	memcpy(backup, board, BOARD_SIZE*sizeof(TBlock));
#endif // _DEBUG
	vector<TMove> history(_history);
	bool bOk;
	vector<TMove> moves;
	moves = next == PLAYER_1 ? getAvailableMoves(board, _p1) : getAvailableMoves(board, _p2);

	if (moves.size() == 0){
		return pair<TMove, TPoint>(rand() % 4 + 1, 0);
	}

	CHeuristicBase::sortMoves(moves, board, _p1, _p2, next, history);

	TPoint value;
	TPoint ab;
	TPoint a = -MY_INFINITY;
	TPoint b = MY_INFINITY;
	TMove bestMove = 0;
	if (next == PLAYER_1){
		value = -MY_INFINITY;
		for (auto m = moves.begin(); m != moves.end(); m++){
			bOk = move(board, _p1, *m, false); assert(bOk);
			history.push_back(*m);
			if (USING_MEMORY)
				ab = alphaBetaTT(board, MOVE(_p1, *m), _p2, PLAYER_2, history, depth - 1, a, b);
			else
				ab = alphaBeta(board, MOVE(_p1, *m), _p2, PLAYER_2, history, depth - 1, a, b);
			if (value < ab){
				value = ab;
				bestMove = *m;
			}
			bOk = move(board, MOVE(_p1, *m), getOpositeDirection(*m), true); assert(bOk);
			history.pop_back();
			if (ab == TIMEOUT_POINTS)
				return pair<TMove, TPoint>(bestMove, TIMEOUT_POINTS);
			assert(ab >= -MY_INFINITY && ab <= MY_INFINITY);
			a = max(value, a);
			if (b <= a)
				break;
		}
	}
	else {
		value = MY_INFINITY;
		for (auto m = moves.begin(); m != moves.end(); m++){
			bOk = move(board, _p2, *m, false); assert(bOk);
			history.push_back(*m);
			if (USING_MEMORY)
				ab = alphaBetaTT(board, _p1, MOVE(_p2, *m), PLAYER_1, history, depth - 1, a, b);
			else
				ab = alphaBeta(board, _p1, MOVE(_p2, *m), PLAYER_1, history, depth - 1, a, b);
			if (value > ab){
				value = ab;
				bestMove = *m;
			}
			bOk = move(board, MOVE(_p2, *m), getOpositeDirection(*m), true); assert(bOk);
			history.pop_back();
			if (ab == TIMEOUT_POINTS)
				return pair<TMove, TPoint>(bestMove, TIMEOUT_POINTS);
			assert(ab >= -MY_INFINITY && ab <= MY_INFINITY);
			b = min(value, b);
			if (b <= a)
				break;
		}
	}


#ifdef _DEBUG
	assert(memcmp(board, backup, BOARD_SIZE*sizeof(TBlock)) == 0);
#endif // _DEBUG

	assert(bestMove >= 1 && bestMove <= 4);
	return pair<TMove, TPoint>(bestMove, value);
}

pair<TMove, TPoint> CSearchEngine::optimalMove(TBlock board[], const TPos&_p1, const TPos&_p2, const TPlayer next, const vector<TMove> &history)
{
	static CMyAI *pAI = CMyAI::getInstance();
	pair<TMove, TPoint> result;
	pair<TMove, TPoint> lastResult(0, 0);
	int d;
	if (pAI->inEnemyTurn)
		d = pAI->lastReachedDepth > 0 ? pAI->lastReachedDepth - 1 : MIN_DEPTH;
	else
	{
		d = pAI->lastReachedDepth > 0 ? pAI->lastReachedDepth - 2 : MIN_DEPTH;
	}
	cout << "started with d = " << d << endl;
	for (; d <= MAX_DEPTH; d++){
		result = getOptimalMoveByAB(board, _p1, _p2, next, history, d);
		if (result.second == TIMEOUT_POINTS)
		{
			if (pAI->foundAnEnd)
				if (pAI->isCalculatingInEnemyTurn)
					pAI->foundAnEnd = false; // if this is an enemy's turn then in the next turn (our turn, do it normal)
				else 
					d++;// if this is our turn, in the enemy turn, reach to this depth right!
			break;
		}
		else
		{
			TPoint p = abs(result.second);
			if (p > POINTS / 2){
				pAI->foundAnEnd = true;
				if (abs(lastResult.second) <= POINTS / 2)
					d--;
			}
			else {
				pAI->foundAnEnd = false;
			}
			lastResult = result;
			cout << lastResult.second << endl;
		}
	}
	pAI->lastReachedDepth = d - 1;
	if (!pAI->isCalculatingInEnemyTurn)
		cout << "Reached depth = " << pAI->lastReachedDepth << endl;

	CTranspositionTable::getInstance()->printStatic();
	return lastResult;
}