#include "MyAI.h"
#include "mydefine.h"



void CMyAI::updateBoard(const int* newBoard, const CPos &_p1, const CPos &_p2, const Player next, const Player we)
{
	// update newBoard to boardData and players' positions
	memcpy(boardData, newBoard, BOARD_SIZE*sizeof(int));
	if (we != next)
		return;
	// we = next player
	posPlayer1 = _p1;
	posPlayer2 = _p2;
	myPos = we == PLAYER_1 ? &posPlayer1 : &posPlayer2;
	myPos = we != PLAYER_1 ? &posPlayer1 : &posPlayer2;
}

// calculate and give an optimal move for ourselves 
Direction CMyAI::newTurn()
{
	static int tempBoard[BOARD_SIZE];
	// update board first
	Player we, next;
	CPos p1, p2;
	if (p_ai->GetBlock(p_ai->GetMyPosition()) == BLOCK_PLAYER_1){
		we = PLAYER_1;
		p1 = p_ai->GetMyPosition();
		p2 = p_ai->GetEnemyPosition();
		next = p_ai->IsMyTurn() ? PLAYER_1 : PLAYER_2;
	}
	else {
		we = PLAYER_2;
		p2 = p_ai->GetMyPosition();
		p1 = p_ai->GetEnemyPosition();
		next = p_ai->IsMyTurn() ? PLAYER_2 : PLAYER_1;
	}

	updateBoard(p_ai->GetBoard(), p1, p2, next, we);

	// start ISOLATED MODE
	if (isIsolated()){
		cout << "start isolated mode!" << endl;
		CPos pos; pos = p_ai->GetMyPosition();
		return getNextMoveOfLongestPath(boardData, pos);
	}
	// end ISOLATED MODE

	// start NORMAL_MODE

	if (we == PLAYER_1)
		return optimalMove(boardData, posPlayer1, posPlayer2, PLAYER_1);
	else
		return optimalMove(boardData, posPlayer1, posPlayer2, PLAYER_2);
	// end NORMAL_MODE
}

// this method is call to get the optimalMove for the next player
Direction CMyAI::optimalMove(int board[121], const CPos &_p1, const CPos &_p2, const Player next)
{
#ifdef _DEBUG
	int backup[121];
	memcpy(backup, board, BOARD_SIZE*sizeof(int));
#endif // _DEBUG

	bool bOk;
	CPos newPos;
	vector<Direction> allMoves; allMoves.clear();
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
			points[iMove] = ab(board, newPos, _p2, PLAYER_2, DEPTH, MIN_POINTS - 1, MAX_POINTS + 1);
			bOk = move(board, newPos, getOpositeDirection(allMoves[iMove]), true); assert(bOk);
		}
		else {
			newPos = _p2.move(allMoves[iMove]);
			bOk = move(board, _p2, allMoves[iMove]); assert(bOk);
			points[iMove] = ab(board, _p1, newPos, PLAYER_1, DEPTH, MIN_POINTS - 1, MAX_POINTS + 1);
			bOk = move(board, newPos, getOpositeDirection(allMoves[iMove]), true); assert(bOk);
		}
	}
	int iMax = 0;
	for (int i = 0; i < (int)allMoves.size(); i++){
		if (next == PLAYER_1){
			if (points[iMax] <= points[i])
				iMax = i;
		}
		else if (points[iMax] >= points[i])
			iMax = i;
		}

#ifdef _DEBUG
	assert(memcmp(board, backup, BOARD_SIZE*sizeof(int)) == 0);
#endif // _DEBUG
	return allMoves[iMax];
	}

