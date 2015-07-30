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
#ifdef _DEBUG
	TBlock backup[BOARD_SIZE];
	memcpy(backup, board, BOARD_SIZE*sizeof(TBlock));
#endif // _DEBUG
	assert((getBlock(board, _p1)) == BLOCK_PLAYER_1);
	assert((getBlock(board, _p2)) == BLOCK_PLAYER_2);
	assert(next == PLAYER_1 || next == PLAYER_2);
	static CMyAI* pAI = CMyAI::getInstance();
	static CMyTimer *timer = CMyTimer::getInstance();
	if (timer->timeUp())
		return TIMEOUT_POINTS;

	bool bOk;
	int bestValue = -MY_INFINITY;

	int point;
	int winner = CHeuristicBase::evaluateBoard(board, _p1, _p2, next, point, pAI->useGoodEvaluation || _p1 - _p2 < DISTANCE);
	assert(point >= 0);
	if (winner == PLAYER_1){
		return (POINTS / 2 + point);
	}
	else if (winner == PLAYER_2){
		return (-POINTS / 2 - point);
	}

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
			bOk = move(board, _p1.move(*m), getOpositeDirection(*m), true); assert(bOk);
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
			value = min(value, ab = alphaBeta(board, _p1, _p2.move(*m), PLAYER_1, depth - 1, a, b));
			bOk = move(board, _p2.move(*m), getOpositeDirection(*m), true); assert(bOk);
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
#endif // _DEBUG
	return value;
}

int CSearchEngine::alphaBetaTT(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int alpha, int beta)
{
#ifdef _DEBUG
	TBlock backup[BOARD_SIZE];
	memcpy(backup, board, BOARD_SIZE*sizeof(TBlock));
#endif // _DEBUG
	assert((getBlock(board, _p1)) == BLOCK_PLAYER_1);
	assert((getBlock(board, _p2)) == BLOCK_PLAYER_2);
	assert(next == PLAYER_1 || next == PLAYER_2);
	static CMyTimer *timer = CMyTimer::getInstance();
	static CMyAI* pAI = CMyAI::getInstance();
	if (timer->timeUp())
		return TIMEOUT_POINTS;
	static CTranspositionTable *tt = CTranspositionTable::getInstance();
	CGameState state(board, _p1, _p2, next);
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
	int winner = CHeuristicBase::evaluateBoard(board, _p1, _p2, next, point, pAI->useGoodEvaluation || _p1 - _p2 < DISTANCE);
	assert(point >= 0);
	if (winner == PLAYER_1){
		g = (POINTS / 2 + point);
	}
	else if (winner == PLAYER_2){
		g = (-POINTS / 2 - point);
	}
	else if (depth == 0)
	{
		g = heuristic.rateBoard(board, _p1, _p2, next);
	}
	else {
		Pos2D newPos;
		vector<TMove> moves;
		moves = next == PLAYER_1 ? getAvailableMoves(board, _p1) : getAvailableMoves(board, _p2);
		CHeuristicBase::sortMoves(moves, board, _p1, _p2, next);
		int ab;
		if (next == PLAYER_1){
			g = -MY_INFINITY;
			int a = alpha;
			for (auto m = moves.begin(); m != moves.end(); m++){
				bOk = move(board, _p1, *m, false); assert(bOk);
				g = max(g, ab = alphaBetaTT(board, _p1.move(*m), _p2, PLAYER_2, depth - 1, a, beta));
				bOk = move(board, _p1.move(*m), getOpositeDirection(*m), true); assert(bOk);
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
				g = min(g, ab = alphaBetaTT(board, _p1, _p2.move(*m), PLAYER_1, depth - 1, alpha, b));
				bOk = move(board, _p2.move(*m), getOpositeDirection(*m), true); assert(bOk);
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
#endif // _DEBUG

	return g;
}

int CSearchEngine::negaMaxTT(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int alpha, int beta)
{
#ifdef _DEBUG
	TBlock backup[BOARD_SIZE];
	memcpy(backup, board, BOARD_SIZE*sizeof(TBlock));
#endif // _DEBUG
	assert((getBlock(board, _p1)) == BLOCK_PLAYER_1);
	assert((getBlock(board, _p2)) == BLOCK_PLAYER_2);
	assert(next == PLAYER_1 || next == PLAYER_2);
	static CMyTimer *timer = CMyTimer::getInstance();
	static CMyAI* pAI = CMyAI::getInstance();
	if (timer->timeUp())
		return TIMEOUT_POINTS;
	int alphaOrig = alpha;
	static CTranspositionTable *tt = CTranspositionTable::getInstance();
	CGameState state(board, _p1, _p2, next);
	state.depth = depth;
	CGameState* ttEntry = tt->get(state);
	if (ttEntry && ttEntry->depth >= depth &&ttEntry->value != TIMEOUT_POINTS){
		if (ttEntry->flag == CGameState::EXACT)
			return ttEntry->value;
		else if (ttEntry->flag == CGameState::LOWER)
			alpha = max(alpha, ttEntry->value);
		else if (ttEntry->flag == CGameState::UPPER)
			beta = min(beta, ttEntry->value);
		if (alpha >= beta)
			return ttEntry->value;
	}

	bool bOk;
	int bestValue;
	int point;
	int color = next == PLAYER_1 ? 1 : -1;
	int winner = CHeuristicBase::evaluateBoard(board, _p1, _p2, next, point, pAI->useGoodEvaluation || _p1 - _p2 < DISTANCE);
	assert(point >= 0);
	if (winner == PLAYER_1){
		bestValue = color*(POINTS / 2 + point);
	}
	else if (winner == PLAYER_2){
		bestValue = color*(-POINTS / 2 - point);
	}
	else if (depth == 0)
	{
		bestValue = color*heuristic.rateBoard(board, _p1, _p2, next);
	}
	else {
		Pos2D newPos;
		vector<TMove> moves;
		moves = next == PLAYER_1 ? getAvailableMoves(board, _p1) : getAvailableMoves(board, _p2);
		CHeuristicBase::sortMoves(moves, board, _p1, _p2, next);
		int ab;
		if (next == PLAYER_1){
			bestValue = -MY_INFINITY;
			for (auto m = moves.begin(); m != moves.end(); m++){
				bOk = move(board, _p1, *m, false); assert(bOk);
				ab = negaMaxTT(board, _p1.move(*m), _p2, PLAYER_2, depth - 1, -beta, -alpha);
				bestValue = max(bestValue, -ab);
				bOk = move(board, _p1.move(*m), getOpositeDirection(*m), true); assert(bOk);
				if (ab == TIMEOUT_POINTS)
					return TIMEOUT_POINTS;
				if (alpha >= beta)
					break;
			}
		}
		else {
			bestValue = -MY_INFINITY;
			for (auto m = moves.begin(); m != moves.end(); m++){
				bOk = move(board, _p2, *m, false); assert(bOk);
				ab = negaMaxTT(board, _p1, _p2.move(*m), PLAYER_1, depth - 1, -beta, -alpha);
				bestValue = max(bestValue, -ab);
				bOk = move(board, _p2.move(*m), getOpositeDirection(*m), true); assert(bOk);
				if (ab == TIMEOUT_POINTS)
					return TIMEOUT_POINTS;
				if (alpha >= beta)
					break;
			}
		}
	}

	if (!ttEntry)
		ttEntry = tt->put(state);
	ttEntry->value = bestValue;
	if (bestValue <= alphaOrig)
		ttEntry->flag = CGameState::UPPER;
	else if (bestValue >= beta)
		ttEntry->flag = CGameState::LOWER;
	else
		ttEntry->flag = CGameState::EXACT;
	ttEntry->depth = depth;

#ifdef _DEBUG
	assert(memcmp(board, backup, BOARD_SIZE*sizeof(TBlock)) == 0);
#endif // _DEBUG
	return bestValue;
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
		g = alphaBetaTT(board, _p1, _p2, next, depth, beta - 1, beta);
		if (g == TIMEOUT_POINTS)
			return g;
		if (g < beta)
			upperBound = g;
		else
			lowerBound = g;
	} while (lowerBound < upperBound);
	return g;
}

int CSearchEngine::mtdfIterativeDeepening(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int maxDepth)
{
	static CTranspositionTable *table = CTranspositionTable::getInstance();
	startTime = clock();
	int firstGuest = 0;
	int d = 1;
	for (d = MIN_DEPTH; d <= maxDepth; d++){
		int lastGuest = mtdF(board, _p1, _p2, next, firstGuest, d);
		if (lastGuest == TIMEOUT_POINTS)
			break;
		else
			firstGuest = lastGuest;
	}
	cout << "\t\tThe reached depth = " << d << endl;
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
	static int lastPoints[4];

	for (int iMove = 0; iMove < (int)allMoves.size(); iMove++){
		if (next == PLAYER_1){
			newPos = _p1.move(allMoves[iMove]);
			bOk = move(board, _p1, allMoves[iMove]); assert(bOk);
			switch (flag){
			case ALPHA_BETA_ITERATIVE_DEEPENING:
				if (usingTT)
					points[iMove] = alphaBetaTT(board, newPos, _p2, PLAYER_2, maxDepth, -MY_INFINITY, MY_INFINITY);
				else
					points[iMove] = alphaBeta(board, newPos, _p2, PLAYER_2, maxDepth, -MY_INFINITY, MY_INFINITY);
				if (points[iMove] == TIMEOUT_POINTS)
					return TIMEOUT_POINTS;
				break;
			case MTDF_ITERATIVE_DEEPENING:
				points[iMove] = mtdfIterativeDeepening(board, newPos, _p2, PLAYER_2, MAX_DEPTH);
				if (points[iMove] == TIMEOUT_POINTS)
					return TIMEOUT_POINTS;
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
			case ALPHA_BETA_ITERATIVE_DEEPENING:
				if (usingTT)
					points[iMove] = alphaBetaTT(board, _p1, newPos, PLAYER_1, maxDepth, -MY_INFINITY, MY_INFINITY);
				else
					points[iMove] = alphaBeta(board, _p1, newPos, PLAYER_1, maxDepth, -MY_INFINITY, MY_INFINITY);
				if (points[iMove] == TIMEOUT_POINTS)
					return TIMEOUT_POINTS;
				break;
			case MTDF_ITERATIVE_DEEPENING:
				points[iMove] = mtdfIterativeDeepening(board, _p1, newPos, PLAYER_1, MAX_DEPTH);
				if (points[iMove] == TIMEOUT_POINTS)
					return TIMEOUT_POINTS;
				break;
			default:
				assert(false);
				break;
			}
			bOk = move(board, newPos, getOpositeDirection(allMoves[iMove]), true); assert(bOk);
		}
	}

	CMyAI *ai = CMyAI::getInstance();
	if (memcmp(lastPoints, points, sizeof(points[0])*allMoves.size()) == 0 || ai->lastReachedDepth > MIN_DEPTH + 4){
		CMyAI::getInstance()->useGoodEvaluation = true;
	}
	memcpy(lastPoints, points, sizeof(points[0])*allMoves.size());

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
	static CMyAI *pAI = CMyAI::getInstance();
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
	pAI->lastReachedDepth = d;
	cout << "Reached depth = " << d << endl;

	CTranspositionTable::getInstance()->printStatic();
	return bestMove;
}