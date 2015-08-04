#include "SearchEngine.h"
#include "HeuristicBase.h"
#include "MyAI.h"
#include "GameState.h"
#include "TranspositionTable.h"
#include "StaticFunctions.h"
#include "MyTimer.h"


CSearchEngine::CSearchEngine(){}


CSearchEngine::~CSearchEngine(){}

int CSearchEngine::alphaBeta(TBlock board[], const Pos1D&_p1, const Pos1D&_p2, const TPlayer next, vector<TMove> &history,
	int depth, int a, int b)
{
#ifdef _DEBUG
	TBlock backup[BOARD_SIZE];
	memcpy(backup, board, BOARD_SIZE*sizeof(TBlock));
	vector<TMove> _history(history);
#endif // _DEBUG
	assert((getBlock(board, _p1)) == BLOCK_PLAYER_1);
	assert((getBlock(board, _p2)) == BLOCK_PLAYER_2);
	assert(next == PLAYER_1 || next == PLAYER_2);
	static CMyAI* pAI = CMyAI::getInstance();
	static CMyTimer *timer = CMyTimer::getInstance();
	if (pAI->shouldEndMoveNow())
		return TIMEOUT_POINTS;

	bool bOk;
	int bestValue = -MY_INFINITY;

	int point;
	int winner = CHeuristicBase::evaluateBoard(board, _p1, _p2, next, point);
	assert(point >= 0);
	if (winner == PLAYER_1){
		return (POINTS / 2 + point);
	}
	else if (winner == PLAYER_2){
		return (-POINTS / 2 - point);
	}

	if (depth == 0)
	{
		pAI->iRateBoard++;
		return heuristic.rateBoardTT(board, _p1, _p2, next, history);
	}

	vector<TMove> moves;
	moves = next == PLAYER_1 ? getAvailableMoves(board, _p1) : getAvailableMoves(board, _p2);

	CHeuristicBase::sortMoves(moves, board, _p1, _p2, next, history);

	int value;
	int ab;
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
	return value;
}

int CSearchEngine::alphaBetaTT(TBlock board[], const Pos1D&_p1, const Pos1D&_p2, const TPlayer next, vector<TMove> &history,
	int depth, int alpha, int beta)
{
#ifdef _DEBUG
	TBlock backup[BOARD_SIZE];
	memcpy(backup, board, BOARD_SIZE*sizeof(TBlock));
	vector<TMove> _history(history);
#endif // _DEBUG
	assert((getBlock(board, _p1)) == BLOCK_PLAYER_1);
	assert((getBlock(board, _p2)) == BLOCK_PLAYER_2);
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
		if (ttEntry->lowerbound >= beta)
		{
			return ttEntry->lowerbound;
		}
		if (ttEntry->upperBound <= alpha)
		{
			return ttEntry->upperBound;
		}
		alpha = max(alpha, ttEntry->lowerbound);
		beta = min(beta, ttEntry->upperBound);
	}
	bool bOk;
	int g;
	int point;
	int winner = CHeuristicBase::evaluateBoard(board, _p1, _p2, next, point);
	assert(point >= 0);
	if (winner == PLAYER_1){
		g = (POINTS / 2 + point);
	}
	else if (winner == PLAYER_2){
		g = (-POINTS / 2 - point);
	}
	else if (depth == 0)
	{
		pAI->iRateBoard++;
		g = heuristic.rateBoard(board, _p1, _p2, next);
	}
	else {
		vector<TMove> moves;
		moves = next == PLAYER_1 ? getAvailableMoves(board, _p1) : getAvailableMoves(board, _p2);
		CHeuristicBase::sortMoves(moves, board, _p1, _p2, next, history);
		int ab;
		if (next == PLAYER_1){
			g = -MY_INFINITY;
			int a = alpha;
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
			int b = beta;
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
		ttEntry->upperBound = g;
	if (g > alpha && g < beta){
		ttEntry->lowerbound = g;
		ttEntry->upperBound = g;
	}
	if (g >= beta)
		ttEntry->lowerbound = g;
	ttEntry->depth = depth;

#ifdef _DEBUG
	assert(memcmp(board, backup, BOARD_SIZE*sizeof(TBlock)) == 0);
	assert(_history.size() == history.size());
	assert(equal(_history.begin(), _history.end(), history.begin()));
#endif // _DEBUG

	return g;
}

int CSearchEngine::mtdF(TBlock board[], const Pos1D&_p1, const Pos1D&_p2, const TPlayer next, vector<TMove> &history,
	int f, int depth)
{
	int beta;
	int g = f;
	int upperBound = MY_INFINITY;
	int lowerBound = -MY_INFINITY;
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

int CSearchEngine::mtdfIterativeDeepening(TBlock board[], const Pos1D&_p1, const Pos1D&_p2, const TPlayer next, vector<TMove> &history,
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

pair<TMove, int> CSearchEngine::getOptimalMoveByAB(TBlock board[], const Pos1D&_p1, const Pos1D&_p2, const TPlayer next, const vector<TMove> &_history, int depth)
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
		return pair<TMove, int>(rand() % 4 + 1, 0);
	}

	CHeuristicBase::sortMoves(moves, board, _p1, _p2, next, history);

	int value;
	int ab;
	int a = -MY_INFINITY;
	int b = MY_INFINITY;
	int bestMove = 0;
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
				return pair<TMove, int>(bestMove, TIMEOUT_POINTS);
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
				return pair<TMove, int>(bestMove, TIMEOUT_POINTS);
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
	return pair<TMove, int>(bestMove, value);
}

pair<TMove, int> CSearchEngine::optimalMove(TBlock board[], const Pos1D&_p1, const Pos1D&_p2, const TPlayer next, const vector<TMove> &history)
{
	static CMyAI *pAI = CMyAI::getInstance();
	pair<TMove, int> result;
	pair<TMove, int> bestMove;
	int d = 1;
	for (d = max(MIN_DEPTH, pAI->lastReachedDepth - 3); d <= MAX_DEPTH; d++){
		result = getOptimalMoveByAB(board, _p1, _p2, next, history, d);
		if (result.second == TIMEOUT_POINTS)
			break;
		else
		{
			bestMove = result;
			cout << result.second << endl;
		}
	}
	pAI->lastReachedDepth = d;
	if (!pAI->isCalculatingInEnemyTurn)
		cout << "Reached depth = " << d << endl;

	CTranspositionTable::getInstance()->printStatic();
	return bestMove;
}