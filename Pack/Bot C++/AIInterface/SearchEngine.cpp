#include "SearchEngine.h"
#include "HeuristicBase.h"
#include "MyAI.h"
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

int CSearchEngine::alphaBeta(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int a, int b)
{
	assert((getBlock(board, _p1)) == BLOCK_PLAYER_1);
	assert((getBlock(board, _p2)) == BLOCK_PLAYER_2);
	assert(next == PLAYER_1 || next == PLAYER_2);
	static CMyTimer *timer = CMyTimer::getInstance();
	if (timer->timeUp())
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

	assert(!isIsolated(board, _p1, _p2));

	if (depth == 0)
	{
		return heuristic.rateBoard(board, _p1, _p2, next);
	}

	Pos2D newPos;
	vector<TMove> moves;
	moves = next == PLAYER_1 ? getAvailableMoves(board, _p1) : getAvailableMoves(board, _p2);

	CHeuristicBase::sortMoves(moves, board, _p1, _p2, next);

	int value;
	int ab;
	if (next == PLAYER_1){
		value = -MY_INFINITY;
		for (auto m = moves.begin(); m != moves.end(); m++){
			bOk = move(board, _p1, *m, false); assert(bOk);
			value = max(value, ab = alphaBeta(board, _p1.move(*m), _p2, PLAYER_2, depth - 1, a, b));
			if (ab == TIMEOUT_POINTS)
				return TIMEOUT_POINTS;
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
			value = min(value, ab = alphaBeta(board, _p1, _p2.move(*m), PLAYER_1, depth - 1, a, b));
			if (ab == TIMEOUT_POINTS)
				return TIMEOUT_POINTS;
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
		g = alphaBeta(board, _p1, _p2, next, depth, beta - 1, beta);
		if (g < beta)
			upperBound = g;
		else
			lowerBound = g;
	} while (lowerBound < upperBound);
	return g;
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

TMove CSearchEngine::optimalMove(TBlock board[121], const Pos2D &_p1, const Pos2D &_p2, const TPlayer next, int maxDepth)
{
#ifdef _DEBUG
	TBlock backup[BOARD_SIZE];
	memcpy(backup, board, BOARD_SIZE*sizeof(TBlock));
#endif // _DEBUG
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
				points[iMove] = alphaBeta(board, newPos, _p2, PLAYER_2, maxDepth, -MY_INFINITY, MY_INFINITY);
				if (points[iMove] == TIMEOUT_POINTS)
					return TIMEOUT_POINTS;
				break;
			case MTDF_ITERATIVE_DEEPENING:
				points[iMove] = mtdfIterativeDeepening(board, newPos, _p2, PLAYER_2);
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
				points[iMove] = alphaBeta(board, _p1, newPos, PLAYER_1, maxDepth, -MY_INFINITY, MY_INFINITY);
				if (points[iMove] == TIMEOUT_POINTS)
					return TIMEOUT_POINTS;
				break;
			case MTDF_ITERATIVE_DEEPENING:
				points[iMove] = mtdfIterativeDeepening(board, _p1, newPos, PLAYER_1);
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

TMove CSearchEngine::optimalMove(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next)
{
	int bestMove = 0;
	int newBestMove;
	int d = 1;
	for (d = MIN_DEPTH; d <= MAX_DEPTH; d++){
		newBestMove = optimalMove(board, _p1, _p2, next, d);
		if (newBestMove == TIMEOUT_POINTS)
			break;
		else
			bestMove = newBestMove;
	}
	cout << "Reached depth = " << d << endl;
	return bestMove;
}