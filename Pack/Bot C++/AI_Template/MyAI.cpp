#include "MyAI.h"
#include "mydefine.h"
#include "..\AIInterface\StaticFunctions.h"
#include "..\AIInterface\HeuristicBase.h"
#include "..\AIInterface\SearchAlgorithm.h"

int CMyAI::DEPTH;

CMyAI::CMyAI()
{
	ifstream f;
	int d;
	f.open("config.txt");
	if (f.is_open()){
		f >> d;
		f.close();
	}
	else
		d = DEFAULT_DEPTH;
	DEPTH = d;
	p_ai = AI::GetInstance();
}

CMyAI::~CMyAI()
{

}


void CMyAI::updateBoard(const int* newBoard, const Pos2D &_p1, const Pos2D &_p2, const TPlayer next, const TPlayer we)
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
TMove CMyAI::newTurn()
{
	static int tempBoard[BOARD_SIZE];
	clock_t startTime = clock();
	// update board first
	TPlayer we, next;
	Pos2D p1, p2;
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
	if (isIsolated(this->boardData, this->posPlayer1, this->posPlayer1) < 0){
		cout << "start isolated mode!" << endl;
		Pos2D pos; pos = p_ai->GetMyPosition();
		return CHeuristicBase::getFirstMoveOfTheLongestPath(boardData, pos, 10);
	}
	// end ISOLATED MODE

	// start NORMAL_MODE

	int move;
	if (we == PLAYER_1)
		move = optimalMove(boardData, posPlayer1, posPlayer2, PLAYER_1);
	else
		move = optimalMove(boardData, posPlayer1, posPlayer2, PLAYER_2);
	cout << double(clock() - startTime) / (double)CLOCKS_PER_SEC << " seconds." << endl;
	return move;
	// end NORMAL_MODE
}

// this method is call to get the optimalMove for the next player
TMove CMyAI::optimalMove(int board[121], const Pos2D &_p1, const Pos2D &_p2, const TPlayer next)
{
#ifdef _DEBUG
	int backup[121];
	memcpy(backup, board, BOARD_SIZE*sizeof(int));
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
			points[iMove] = CSearchAlgorithm::ab(board, newPos, _p2, PLAYER_2, DEPTH, MIN_POINTS - 1, MAX_POINTS + 1);
			bOk = move(board, newPos, getOpositeDirection(allMoves[iMove]), true); assert(bOk);
		}
		else {
			newPos = _p2.move(allMoves[iMove]);
			bOk = move(board, _p2, allMoves[iMove]); assert(bOk);
			points[iMove] = CSearchAlgorithm::ab(board, _p1, newPos, PLAYER_1, DEPTH, MIN_POINTS - 1, MAX_POINTS + 1);
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

