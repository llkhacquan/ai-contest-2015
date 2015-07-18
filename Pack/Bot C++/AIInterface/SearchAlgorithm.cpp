#include "SearchAlgorithm.h"
#include "HeuristicBase.h"


CSearchAlgorithm::CSearchAlgorithm()
{
}


CSearchAlgorithm::~CSearchAlgorithm()
{
}


// this alpha beta method help player_1 win
int CSearchAlgorithm::ab(int board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int a, int b)
{
	int v;
	assert((v = getBlock(board, _p1)) == BLOCK_PLAYER_1);
	assert((v = getBlock(board, _p2)) == BLOCK_PLAYER_2);
	bool bOk;

	int winner = CHeuristicBase::evaluateBoard(board, _p1, _p2, next);
	if (depth == 0){
		return CHeuristicBase::rateBoard(board, _p1, _p2, next);
	}
	else if (winner == PLAYER_1){
		return MAX_POINTS;
	}
	else if (winner == PLAYER_2){
		return MIN_POINTS;
	}
	else { // winner == OBSERVER
		// do nothing :v
	}

	Pos2D newPos;
	vector<TMove> moves;
	moves = next == PLAYER_1 ? getAvailableMoves(board, _p1) : getAvailableMoves(board, _p2);

	// sortMoves

	if (next == PLAYER_1){
		v = MIN_POINTS;
		for (int iMove = 0; iMove < (int)moves.size(); iMove++){
			newPos = _p1.move(moves[iMove]);
			bOk = move(board, _p1, moves[iMove]); assert(bOk);
			v = max(v, ab(board, newPos, _p2, PLAYER_2, depth - 1, a, b));
			bOk = move(board, newPos, getOpositeDirection(moves[iMove]), true); assert(bOk);
			a = max(a, v);
			if (b <= a)
				break;
		}
	}
	else {
		v = MAX_POINTS;
		for (int iMove = 0; iMove < (int)moves.size(); iMove++){
			newPos = _p2.move(moves[iMove]);
			bOk = move(board, _p2, moves[iMove]); assert(bOk);
			v = min(v, ab(board, _p1, newPos, PLAYER_1, depth - 1, a, b));
			bOk = move(board, newPos, getOpositeDirection(moves[iMove]), true); assert(bOk);
			b = min(b, v);
			if (b <= a)
				break;
		}
	}
	return v;
}