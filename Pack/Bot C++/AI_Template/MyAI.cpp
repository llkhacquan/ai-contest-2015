#include "MyAI.h"
#include "mydefine.h"
#include "..\AIInterface\StaticFunctions.h"
#include "..\AIInterface\HeuristicBase.h"
#include "..\AIInterface\SearchEngine.h"
#include "..\AIInterface\TranspositionTable.h"
#include "..\AIInterface\MyTimer.h"

CMyAI* CMyAI::instance;

CMyAI::CMyAI()
{
	p_ai = AI::GetInstance();
	CTranspositionTable::getInstance();

	// bugs in ab with memory, mtdf iterative deepening
	searcher.flag = CSearchEngine::ALPHA_BETA;
	searcher.heuristic.rateBoard = &CHeuristicBase::simpleRateBoard;
	searcher.heuristic.quickRateBoard = &CHeuristicBase::voronoiRateBoard;
}

CMyAI::~CMyAI()
{
	instance = NULL;
}


void CMyAI::updateBoard(const int* newBoard, const Pos2D &_p1, const Pos2D &_p2, const TPlayer next, const TPlayer we)
{
	// update newBoard to boardData and players' positions
	for (int i = 0; i < BOARD_SIZE; i++){
		boardData[i] = newBoard[i];
	}
	iTurn++;

	if (boardData[0] == BLOCK_PLAYER_1 || boardData[120] == BLOCK_PLAYER_2) // it is our first move
	{
		// new game
		cout << "===================start a new game=====================" << endl;
		iTurn = 0;
		nObjectsIn5x5 = 0;
		for (int i = 3; i <= 7; i++)
			for (int j = 3; j <= 7; j++)
				if (getBlock(boardData, i, j) == BLOCK_OBSTACLE)
					nObjectsIn5x5++;


		if (we == PLAYER_1)
		{
			cout << "\t\twe are player 1 \n";
			if (boardData[120] == BLOCK_PLAYER_2)
				cout << "\tYeah! We go first!\n";
			else
			{
				cout << "\tOh shit! We do not go first!\n";
				iTurn++;
			}
		}
		else
		{
			cout << "\t\twe are player 2 \n";
			if (boardData[0] == BLOCK_PLAYER_1)
				cout << "\tYeah! We go first!\n";
			else
			{
				cout << "\tOh shit! We do not go first!\n";
				iTurn++;
			}
		}
	}

	if (we != next)
		return;
	// we = next player
	posPlayer1 = _p1;
	posPlayer2 = _p2;
	myPos = we == PLAYER_1 ? &posPlayer1 : &posPlayer2;
	myPos = we != PLAYER_1 ? &posPlayer1 : &posPlayer2;
	CMyAI::getInstance()->doneDepth = 0;
}

// calculate and give an optimal move for ourselves 
TMove CMyAI::newTurn()
{
	static TBlock tempBoard[BOARD_SIZE];
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
	if (isIsolated(this->boardData, this->posPlayer1, this->posPlayer2)){
		cout << "Isolated mode!" << endl;
		CMyTimer::getInstance()->reset();
		Pos2D pos;
		pos = p_ai->GetEnemyPosition();
		int n2 = CHeuristicBase::getEstimatedLengthOfTheLongestPath(this->boardData, pos);

		pos = p_ai->GetMyPosition();
		int n1 = CHeuristicBase::getEstimatedLengthOfTheLongestPath(this->boardData, pos);
		cout << "\tEstimated length of our path : " << n1 << endl;
		cout << "\tEstimated length of enemy path : " << n2 << endl;
		if (n1 > n2)
			cout << "\t => It seems that we will fucking win  :)\n";
		else
			cout << "\t => It seems that we will fucking lost :(\n";
		pos = p_ai->GetMyPosition();
		TMove t = CHeuristicBase::getFirstMoveOfTheLongestPath(boardData, pos, ISOLATED_DEPTH);
		cout << "Time of this turn: " << double(clock() - startTime) / (double)CLOCKS_PER_SEC
			<< " seconds. Reached depth : " << doneDepth << endl;
		return t;
	}
	// end ISOLATED MODE

	// start NORMAL_MODE

	TMove move;
	if (we == PLAYER_1)
		move = searcher.optimalMove(boardData, posPlayer1, posPlayer2, PLAYER_1);
	else
		move = searcher.optimalMove(boardData, posPlayer1, posPlayer2, PLAYER_2);
	cout << "Time of this turn: " << double(clock() - startTime) / (double)CLOCKS_PER_SEC
		<< " seconds. Reached depth : " << doneDepth << endl;
	return move;
	// end NORMAL_MODE
}

CMyAI* CMyAI::getInstance()
{
	if (instance == NULL)
		instance = new CMyAI();
	return instance;
}

