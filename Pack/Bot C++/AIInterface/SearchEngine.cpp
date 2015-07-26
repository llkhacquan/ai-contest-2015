#include "SearchEngine.h"
#include "HeuristicBase.h"
#include "..\AI_Template\MyAI.h"
#include "GameState.h"
#include "TranspositionTable.h"
#include "StaticFunctions.h"
#include "MyTimer.h"


CSearchEngine::CSearchEngine()
{
}


CSearchEngine::~CSearchEngine()
{
}


int CSearchEngine::negaMax(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int alpha, int beta)
{
	assert((getBlock(board, _p1)) == BLOCK_PLAYER_1);
	assert((getBlock(board, _p2)) == BLOCK_PLAYER_2);
	assert(next == PLAYER_1 || next == PLAYER_2);

	int color = next == PLAYER_1 ? 1 : -1;
	int point;
	int winner = CHeuristicBase::evaluateBoard(board, _p1, _p2, next, point);
	switch (winner){
	case PLAYER_1:
		return color*((3 * MAX_POINTS + MIN_POINTS) / 4 + point);
	case PLAYER_2:
		return color*((MAX_POINTS + 3 * MIN_POINTS) / 4 - point);
	}

	assert(!isIsolated(board, _p1, _p2));

	if (depth == MAX_DEPTH)
	{
		CMyAI::getInstance()->doneDepth = max(CMyAI::getInstance()->doneDepth, depth - 1);
		return color*heuristic.rateBoard(board, _p1, _p2, next);
	}

	int bestValue = -MY_INFINITY;
	bool bOk;
	Pos2D newPos;
	vector<TMove> moves;
	moves = next == PLAYER_1 ? getAvailableMoves(board, _p1) : getAvailableMoves(board, _p2);
	CHeuristicBase::sortMoves(moves, board, _p1, _p2, next);
	for (auto m = moves.begin(); m != moves.end(); m++){
		if (next == PLAYER_1){
			bOk = move(board, _p1, *m); assert(bOk);
			int val = -negaMax(board, _p1.move(*m), _p2, PLAYER_2, depth + 1, -beta, -alpha);
			bOk = move(board, _p1.move(*m), getOpositeDirection(*m), true); assert(bOk);
			bestValue = max(bestValue, val);
			alpha = max(alpha, val);
			if (alpha >= beta)
				break;
		}
		else {
			bOk = move(board, _p2, *m); assert(bOk);
			int val = -negaMax(board, _p1, _p2.move(*m), PLAYER_1, depth + 1, -beta, -alpha);
			bOk = move(board, _p2.move(*m), getOpositeDirection(*m), true); assert(bOk);
			bestValue = max(bestValue, val);
			alpha = max(alpha, val);
			if (alpha >= beta)
				break;
		}
	}
	return bestValue;
}

int CSearchEngine::negaScout(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int alpha, int beta)
{
	assert((getBlock(board, _p1)) == BLOCK_PLAYER_1);
	assert((getBlock(board, _p2)) == BLOCK_PLAYER_2);
	assert(next == PLAYER_1 || next == PLAYER_2);

	bool bOk;
	int color = next == PLAYER_1 ? 1 : -1;

	int point;
	int winner = CHeuristicBase::evaluateBoard(board, _p1, _p2, next, point);
	if (winner == PLAYER_1){
		return (MAX_POINTS / 2 + point)* color;
	}
	else if (winner == PLAYER_2){
		return (MIN_POINTS / 2 - point)* color;
	}

	assert(!isIsolated(board, _p1, _p2));


	if (depth == MAX_DEPTH){
		return heuristic.rateBoard(board, _p1, _p2, next) * color;
	}

	int bestValue = MIN_POINTS - 1;
	Pos2D newPos;
	vector<TMove> moves;
	moves = next == PLAYER_1 ? getAvailableMoves(board, _p1) : getAvailableMoves(board, _p2);

	CHeuristicBase::sortMoves(moves, board, _p1, _p2, next);

	int value;
	for (auto m = moves.begin(); m != moves.end(); m++){
		if (next == PLAYER_1){
			bOk = move(board, _p1, *m, false); assert(bOk);
			if (m != moves.begin())
			{
				value = -negaScout(board, _p1.move(*m), _p2, PLAYER_2, depth + 1, -alpha - 1, -alpha);
				if (alpha < value && value < beta)
					value = -negaScout(board, _p1.move(*m), _p2, PLAYER_2, depth + 1, -beta, -value);
			}
			else {
				value = -negaScout(board, _p1.move(*m), _p2, PLAYER_2, depth + 1, -beta, -alpha);
			}
			bOk = move(board, _p1.move(*m), getOpositeDirection(*m), true); assert(bOk);
			alpha = max(alpha, value);
			if (alpha >= beta)
				break;
		}
		else
		{
			bOk = move(board, _p2, *m, false); assert(bOk);
			if (m != moves.begin())
			{
				value = -negaScout(board, _p1, _p2.move(*m), PLAYER_1, depth + 1, -alpha - 1, -alpha);
				if (alpha < value && value < beta)
					value = -negaScout(board, _p1, _p2.move(*m), PLAYER_1, depth + 1, -beta, -value);
			}
			else {
				value = -negaScout(board, _p1, _p2.move(*m), PLAYER_1, depth + 1, -beta, -alpha);
			}
			bOk = move(board, _p2.move(*m), getOpositeDirection(*m), true); assert(bOk);
			alpha = max(alpha, value);
			if (alpha >= beta)
				break;
		}

	}
	return alpha;
}

int CSearchEngine::alphaBeta(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int a, int b)
{
	assert((getBlock(board, _p1)) == BLOCK_PLAYER_1);
	assert((getBlock(board, _p2)) == BLOCK_PLAYER_2);
	assert(next == PLAYER_1 || next == PLAYER_2);

	bool bOk;
	int bestValue = MIN_POINTS - 1;

	int point;
	int winner = CHeuristicBase::evaluateBoard(board, _p1, _p2, next, point);
	switch (winner){
	case PLAYER_1:
		return ((3 * MAX_POINTS + MIN_POINTS) / 4 + point);
	case PLAYER_2:
		return ((MAX_POINTS + 3 * MIN_POINTS) / 4 - point);
	}

	assert(!isIsolated(board, _p1, _p2));

	if (depth == MAX_DEPTH)
	{
		CMyAI::getInstance()->doneDepth = depth - 1;
		return heuristic.rateBoard(board, _p1, _p2, next);
	}

	Pos2D newPos;
	vector<TMove> moves;
	moves = next == PLAYER_1 ? getAvailableMoves(board, _p1) : getAvailableMoves(board, _p2);

	CHeuristicBase::sortMoves(moves, board, _p1, _p2, next);

	int value;
	if (next == PLAYER_1){
		value = -MY_INFINITY;
		for (auto m = moves.begin(); m != moves.end(); m++){
			bOk = move(board, _p1, *m, false); assert(bOk);
			value = max(value, alphaBeta(board, _p1.move(*m), _p2, PLAYER_2, depth + 1, a, b));
			a = max(value, a);
			bOk = move(board, _p1.move(*m), getOpositeDirection(*m), true); assert(bOk);
			if (b <= a)
				break;
		}
		return value;
	}
	else {
		value = MY_INFINITY;
		for (auto m = moves.begin(); m != moves.end(); m++){
			bOk = move(board, _p2, *m, false); assert(bOk);
			value = min(value, alphaBeta(board, _p1, _p2.move(*m), PLAYER_1, depth + 1, a, b));
			b = min(value, b);
			bOk = move(board, _p2.move(*m), getOpositeDirection(*m), true); assert(bOk);
			if (b <= a)
				break;
		}
		return value;
	}
}

int CSearchEngine::mtdF(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int f, int depth)
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
		g = negaMaxWithMemory(board, _p1, _p2, next, depth, beta - 1, beta);
		if (g < beta)
			upperBound = g;
		else
			lowerBound = g;
	} while (lowerBound < upperBound);
	return g;
}

int CSearchEngine::negaMaxWithMemory(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next,
	int depth, int alpha, int beta)
{
	assert((getBlock(board, _p1)) == BLOCK_PLAYER_1);
	assert((getBlock(board, _p2)) == BLOCK_PLAYER_2);
	assert(next == PLAYER_1 || next == PLAYER_2);
	int alphaOrig = alpha;

	CTranspositionTable *table = CTranspositionTable::getInstance();
	CGameState gameState(board, _p1, _p2, next);
	CGameState* ttEntry = table->get(gameState);

	if (ttEntry && ttEntry->depth <= depth){
		switch (ttEntry->flag)
		{
		case CGameState::EXACT:
			return ttEntry->value;
		case CGameState::LOWERBOUND:
			alpha = max(alpha, ttEntry->value);
			break;
		case CGameState::UPPERBOUND:
			beta = min(beta, ttEntry->value);
			break;
		}
		if (alpha >= beta)
			return ttEntry->value;
	}

	int color = next == PLAYER_1 ? 1 : -1;
	bool bOk;
	int point;
	int winner = CHeuristicBase::evaluateBoard(board, _p1, _p2, next, point);
	if (winner == PLAYER_1){
		return color * ((MAX_POINTS * 3 + MIN_POINTS) / 4 + point);
	}
	else if (winner == PLAYER_2){
		return color * ((MAX_POINTS + 3 * MIN_POINTS) / 4 - point);
	}
	else if (depth == MAX_DEPTH){
		return color * heuristic.rateBoard(board, _p1, _p2, next);
	}


	int bestValue = -MY_INFINITY;
	Pos2D newPos;
	vector<TMove> moves;
	moves = next == PLAYER_1 ? getAvailableMoves(board, _p1) : getAvailableMoves(board, _p2);
	CHeuristicBase::sortMoves(moves, board, _p1, _p2, next);

	int val;
	for (auto m = moves.begin(); m != moves.end(); m++){
		if (next == PLAYER_1){
			bOk = move(board, _p1, *m); assert(bOk);
			val = -negaMaxWithMemory(board, _p1.move(*m), _p2, PLAYER_2, depth + 1, -beta, -alpha);
			bOk = move(board, _p1.move(*m), getOpositeDirection(*m), true); assert(bOk);
			bestValue = max(bestValue, val);
			alpha = max(alpha, val);
			if (alpha >= beta)
				break;
		}
		else {
			bOk = move(board, _p2, *m); assert(bOk);
			val = -negaMaxWithMemory(board, _p1, _p2.move(*m), PLAYER_1, depth + 1, -beta, -alpha);
			bOk = move(board, _p2.move(*m), getOpositeDirection(*m), true); assert(bOk);
			bestValue = max(bestValue, val);
			alpha = max(alpha, val);
			if (alpha >= beta)
				break;
		}
	}

	if (!ttEntry)
		ttEntry = table->put(gameState);
	ttEntry->value = bestValue;
	if (bestValue <= alphaOrig)
		ttEntry->flag = CGameState::UPPERBOUND;
	else if (bestValue >= beta)
		ttEntry->flag = CGameState::LOWERBOUND;
	else
		ttEntry->flag = CGameState::EXACT;
	ttEntry->depth = depth;

	return bestValue;
}

int CSearchEngine::mtdfIterativeDeepening(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth = 19)
{
	static CTranspositionTable *table = CTranspositionTable::getInstance();
	startTime = clock();
	int firstGuest = 0;
	int d = 1;
	for (d = MIN_DEPTH; d <= depth; d++){
		firstGuest = mtdF(board, _p1, _p2, next, firstGuest, d);
		double time = double(clock() - startTime);
		if (CMyTimer::getInstance()->timeUp())
		{
			break;
		}
	}
	cout << "\t\tThe reached depth = " << d << endl;
	cout << "\t\tgetok/getmiss/put = " << table->nGetOk << "/" << table->nGetMiss << "/" << table->nPut << endl;
	return firstGuest;
}

TMove CSearchEngine::optimalMove(TBlock board[121], const Pos2D &_p1, const Pos2D &_p2, const TPlayer next)
{
#ifdef _DEBUG
	TBlock backup[BOARD_SIZE];
	memcpy(backup, board, BOARD_SIZE*sizeof(TBlock));
#endif // _DEBUG

	CMyTimer::getInstance()->reset();
	bool bOk;
	Pos2D newPos;
	vector<TMove> allMoves; allMoves.clear();
	allMoves = next == PLAYER_1 ? getAvailableMoves(board, _p1) : getAvailableMoves(board, _p2);

	if (allMoves.size() == 0){
		// we die
		return rand() % 4 + 1;
	}

	int points[4];

	for (int iMove = 0; iMove < (int)allMoves.size(); iMove++){
		if (next == PLAYER_1){
			newPos = _p1.move(allMoves[iMove]);
			bOk = move(board, _p1, allMoves[iMove]); assert(bOk);
			switch (flag){
			case ALPHA_BETA:
				points[iMove] = alphaBeta(board, newPos, _p2, PLAYER_2, 0, -MY_INFINITY, MY_INFINITY);
				break;
			case NEGAMAX_WITH_MEMORY:
				points[iMove] = negaMaxWithMemory(board, newPos, _p2, PLAYER_2, 0, -MY_INFINITY, MY_INFINITY);
				break;
			case MTDF_ITERATIVE_DEEPENING:
				points[iMove] = mtdfIterativeDeepening(board, newPos, _p2, PLAYER_2);
				break;
			case NEGAMAX:
				points[iMove] = negaMax(board, newPos, _p2, PLAYER_2, 0, -MY_INFINITY, MY_INFINITY);
				break;
			case NEGASCOUT:
				points[iMove] = negaScout(board, newPos, _p2, PLAYER_2, 0, -MY_INFINITY, MY_INFINITY);
				break;
			case NEGA_WITH_MEMORY:
				points[iMove] = negaScoutIterativeDeepening(board, newPos, _p2, PLAYER_2, MAX_DEPTH);
				break;
			default:
				assert(false);
				break;
			}
			bOk = move(board, newPos, getOpositeDirection(allMoves[iMove]), true); assert(bOk);
		}
		else {
			newPos = _p2.move(allMoves[iMove]);
			bOk = move(board, _p2, allMoves[iMove]); assert(bOk);
			switch (flag){
			case ALPHA_BETA:
				points[iMove] = alphaBeta(board, _p1, newPos, PLAYER_1, 0, -MY_INFINITY, MY_INFINITY);
				break;
			case NEGAMAX_WITH_MEMORY:
				points[iMove] = negaMaxWithMemory(board, _p1, newPos, PLAYER_1, 0, -MY_INFINITY, MY_INFINITY);
				break;
			case MTDF_ITERATIVE_DEEPENING:
				points[iMove] = mtdfIterativeDeepening(board, _p1, newPos, PLAYER_1);
				break;
			case NEGAMAX:
				points[iMove] = negaMax(board, _p1, newPos, PLAYER_1, 0, -MY_INFINITY, MY_INFINITY);
				break;
			case NEGASCOUT:
				points[iMove] = negaScout(board, _p1, newPos, PLAYER_1, 0, -MY_INFINITY, MY_INFINITY);
				break;
			case NEGA_WITH_MEMORY:
				points[iMove] = negaScoutIterativeDeepening(board, newPos, _p2, PLAYER_2, MAX_DEPTH);
				break;
			default:
				assert(false);
				break;
			}
			bOk = move(board, newPos, getOpositeDirection(allMoves[iMove]), true); assert(bOk);
		}
	}

	int iMax = 0;
	for (int i = 0; i < (int)allMoves.size(); i++){
		cout << points[i] << "\t";
		if (next == PLAYER_1){
			if (points[iMax] < points[i])
				iMax = i;
		}
		else if (points[iMax] > points[i])
			iMax = i;
	}
	cout << endl;

#ifdef _DEBUG
	assert(memcmp(board, backup, BOARD_SIZE*sizeof(TBlock)) == 0);
#endif // _DEBUG
	return allMoves[iMax];
}

int CSearchEngine::negaScoutIterativeDeepening(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth){
	static CTranspositionTable *table = CTranspositionTable::getInstance();
	startTime = clock();
	int firstGuest = 0;
	int d = 1;
	for (d = MIN_DEPTH; d <= depth; d++){
		firstGuest = negaScout(board, _p1, _p2, next, d, -MY_INFINITY, MY_INFINITY);
		double time = double(clock() - startTime);
		if (CMyTimer::getInstance()->timeUp())
		{
			break;
		}
	}
	cout << "\t\tThe reached depth = " << d << endl;
	cout << "\t\tgetok/getmiss/put = " << table->nGetOk << "/" << table->nGetMiss << "/" << table->nPut << endl;
	return firstGuest;
}