#include "SearchAlgorithm.h"
#include "HeuristicBase.h"
#include "..\AI_Template\MyAI.h"
#include "GameState.h"
#include "TranspositionTable.h"


CSearchAlgorithm::CSearchAlgorithm()
{
}


CSearchAlgorithm::~CSearchAlgorithm()
{
}


int CSearchAlgorithm::negaMax(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int a, int b)
{
	TBlock v;
	assert((v = getBlock(board, _p1)) == BLOCK_PLAYER_1);
	assert((v = getBlock(board, _p2)) == BLOCK_PLAYER_2);
	assert(next == PLAYER_1 || next == PLAYER_2);

	bool bOk;
	int color = next == PLAYER_1 ? 1 : -1;

	if (depth == 0){
		return heuristic.rateBoard(board, _p1, _p2, next) * color;
	}
	int point;
	int winner = CHeuristicBase::evaluateBoard(board, _p1, _p2, next, point);
	if (winner == PLAYER_1){
		return (MAX_POINTS / 2 + point)* color;
	}
	else if (winner == PLAYER_2){
		return (MIN_POINTS / 2 - point)* color;
	}

	int bestValue = MIN_POINTS - 1;
	Pos2D newPos;
	vector<TMove> moves;
	moves = next == PLAYER_1 ? getAvailableMoves(board, _p1) : getAvailableMoves(board, _p2);
	int value;
	CHeuristicBase::sortMoves(moves, board, _p1, _p2, next);
	for (auto m = moves.begin(); m != moves.end(); m++){
		if (next == PLAYER_1){
			bOk = move(board, _p1, *m, false); assert(bOk);
			value = -negaMax(board, _p1.move(*m), _p2, PLAYER_2, depth - 1, -b, -a);
			bestValue = max(bestValue, value);
			a = max(a, value);
			bOk = move(board, _p1.move(*m), getOpositeDirection(*m), true); assert(bOk);
			if (a >= b)
				break;
		}
		else {
			bOk = move(board, _p2, *m, false); assert(bOk);
			value = -negaMax(board, _p1, _p2.move(*m), PLAYER_1, depth - 1, -b, -a);
			bestValue = max(bestValue, value);
			a = max(a, value);
			bOk = move(board, _p2.move(*m), getOpositeDirection(*m), true); assert(bOk);
			if (a >= b)
				break;
		}
	}
	return bestValue;
}

int CSearchAlgorithm::negaScout(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int a, int b)
{
	TBlock v;
	assert((v = getBlock(board, _p1)) == BLOCK_PLAYER_1);
	assert((v = getBlock(board, _p2)) == BLOCK_PLAYER_2);
	assert(next == PLAYER_1 || next == PLAYER_2);

	bool bOk;
	int color = next == PLAYER_1 ? 1 : -1;

	if (depth == 0){
		return heuristic.rateBoard(board, _p1, _p2, next) * color;
	}
	int point;
	int winner = CHeuristicBase::evaluateBoard(board, _p1, _p2, next, point);
	if (winner == PLAYER_1){
		return (MAX_POINTS / 2 + point)* color;
	}
	else if (winner == PLAYER_2){
		return (MIN_POINTS / 2 - point)* color;
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
				value = -negaScout(board, _p1.move(*m), _p2, PLAYER_2, depth - 1, -a - 1, -a);
				if (a < value && value < b)
					value = -negaScout(board, _p1.move(*m), _p2, PLAYER_2, depth - 1, -b, -value);
			}
			else {
				value = -negaScout(board, _p1.move(*m), _p2, PLAYER_2, depth - 1, -b, -a);
			}
			bOk = move(board, _p1, getOpositeDirection(*m), true); assert(bOk);
			a = max(a, value);
			if (a >= b)
				break;
		}
		else
		{
			bOk = move(board, _p2, *m, false); assert(bOk);
			if (m != moves.begin())
			{
				value = -negaScout(board, _p1, _p2.move(*m), PLAYER_1, depth - 1, -a - 1, -a);
				if (a < value && value < b)
					value = -negaScout(board, _p1, _p2.move(*m), PLAYER_1, depth - 1, -b, -value);
			}
			else {
				value = -negaScout(board, _p1, _p2.move(*m), PLAYER_1, depth - 1, -b, -a);
			}
			bOk = move(board, _p2, getOpositeDirection(*m), true); assert(bOk);
			a = max(a, value);
			if (a >= b)
				break;
		}

	}
	return a;
}

int CSearchAlgorithm::alphaBeta(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int a, int b)
{
	TBlock v;
	assert((v = getBlock(board, _p1)) == BLOCK_PLAYER_1);
	assert((v = getBlock(board, _p2)) == BLOCK_PLAYER_2);
	assert(next == PLAYER_1 || next == PLAYER_2);

	bool bOk;
	int bestValue = MIN_POINTS - 1;

	if (depth == 0){
		return heuristic.rateBoard(board, _p1, _p2, next);
	}
	int point;
	int winner = CHeuristicBase::evaluateBoard(board, _p1, _p2, next, point);
	if (winner == PLAYER_1){
		return MAX_POINTS / 2 + point;
	}
	else if (winner == PLAYER_2){
		return MIN_POINTS / 2 - point;
	}

	Pos2D newPos;
	vector<TMove> moves;
	moves = next == PLAYER_1 ? getAvailableMoves(board, _p1) : getAvailableMoves(board, _p2);

	CHeuristicBase::sortMoves(moves, board, _p1, _p2, next);

	int value;
	if (next == PLAYER_1){
		value = MIN_POINTS - 1;
		for (auto m = moves.begin(); m != moves.end(); m++){
			bOk = move(board, _p1, *m, false); assert(bOk);
			value = max(value, alphaBeta(board, _p1.move(*m), _p2, PLAYER_2, depth - 1, a, b));
			a = max(value, a);
			bOk = move(board, _p1.move(*m), getOpositeDirection(*m), true); assert(bOk);
			if (b <= a)
				break;
		}
		return value;
	}
	else {
		value = MAX_POINTS + 1;
		for (auto m = moves.begin(); m != moves.end(); m++){
			bOk = move(board, _p2, *m, false); assert(bOk);
			value = min(value, alphaBeta(board, _p1, _p2.move(*m), PLAYER_1, depth - 1, a, b));
			b = min(value, b);
			bOk = move(board, _p2.move(*m), getOpositeDirection(*m), true); assert(bOk);
			if (b <= a)
				break;
		}
		return value;
	}
}

int CSearchAlgorithm::mtdF(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int a, int b)
{
	return 0;
}

int CSearchAlgorithm::negaMaxWithMemory(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int a, int b)
{
	// look in the transposition table
	int alphaOrig = a;
	CGameState gameState(board, _p1, _p2, next);
	static CTranspositionTable *table = CTranspositionTable::getInstance();
	CGameState* ttEntry = table->get(gameState);
	if (ttEntry && ttEntry->depth >= depth){
		switch (ttEntry->flag){
		case CGameState::F_EXACT:
			return ttEntry->value;
		case CGameState::F_LOWERBOUND:
			a = max(a, ttEntry->value);
			break;
		case CGameState::F_UPPERBOUND:
			b = min(b, ttEntry->value);
			break;
		default:
			assert(false);
		}
		if (a >= b)
			return ttEntry->value;
	}

	// the transposition table is shit :v

	TBlock block;
	assert((block = getBlock(board, _p1)) == BLOCK_PLAYER_1);
	assert((block = getBlock(board, _p2)) == BLOCK_PLAYER_2);
	assert(next == PLAYER_1 || next == PLAYER_2);

	bool bOk;
	int color = next == PLAYER_1 ? 1 : -1;

	if (depth == 0){
		return heuristic.rateBoard(board, _p1, _p2, next) * color;
	}
	int point;
	int winner = CHeuristicBase::evaluateBoard(board, _p1, _p2, next, point);
	if (winner == PLAYER_1){
		return (MAX_POINTS / 2 + point)* color;
	}
	else if (winner == PLAYER_2){
		return (MIN_POINTS / 2 - point)* color;
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
			value = -negaMaxWithMemory(board, _p1.move(*m), _p2, PLAYER_2, depth - 1, -b, -a);
			bestValue = max(bestValue, value);
			a = max(a, value);
			bOk = move(board, _p1.move(*m), getOpositeDirection(*m), true); assert(bOk);
			if (a >= b)
				break;
		}
		else {
			bOk = move(board, _p2, *m, false); assert(bOk);
			value = -negaMaxWithMemory(board, _p1, _p2.move(*m), PLAYER_1, depth - 1, -b, -a);
			bestValue = max(bestValue, value);
			a = max(a, value);
			bOk = move(board, _p2.move(*m), getOpositeDirection(*m), true); assert(bOk);
			if (a >= b)
				break;
		}
	}

	// Transposition Table Store; node is the lookup key for ttEntry
	gameState.value = bestValue;
	if (bestValue <= alphaOrig)
		gameState.flag = CGameState::F_UPPERBOUND;
	else if (bestValue >= b)
		gameState.flag = CGameState::F_LOWERBOUND;
	else
		gameState.flag = CGameState::F_EXACT;
	gameState.depth = depth;
	if (ttEntry)
		*ttEntry = gameState;
	else
		table->put(gameState);
	return bestValue;
}

