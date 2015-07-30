#include "MyAI.h"
#include "mydefine.h"
#include "StaticFunctions.h"
#include "HeuristicBase.h"
#include "SearchEngine.h"
#include "TranspositionTable.h"
#include "MyTimer.h"

CMyAI* CMyAI::instance;

CMyAI::CMyAI()
{
	p_ai = AI::GetInstance();
	CTranspositionTable::getInstance();

	searcher.usingTT = true;
	searcher.flag = CSearchEngine::ALPHA_BETA_ITERATIVE_DEEPENING;
	searcher.heuristic.rateBoard = &CHeuristicBase::simpleRateBoard;
	searcher.heuristic.quickRateBoard = &CHeuristicBase::voronoiRateBoard;

	printInformation();
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

	if (!newGame) // assume this only run 1
		if (boardData[0] == BLOCK_PLAYER_1 || boardData[120] == BLOCK_PLAYER_2)
		{
			newGame = true;
			// it is our first move
			// new game
			cout << "===================start a new game=====================" << endl;

			if (we == PLAYER_1)
			{
				cout << "\t\twe are player 1 \n";
				if (boardData[120] == BLOCK_PLAYER_2)
					cout << "\tYeah! We go first!\n";
				else
				{
					cout << "\tOh shit! We do not go first!\n";
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
}

// calculate and give an optimal move for ourselves 
TMove CMyAI::newTurn()
{
	static CMyTimer *timer = CMyTimer::getInstance();
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
	if (activeIsolatedMode || isIsolated(this->boardData, this->posPlayer1, this->posPlayer2)){
		activeIsolatedMode = true;
		cout << "Isolated mode!" << endl;
		Pos2D pos;
		pos = p_ai->GetEnemyPosition();
		int n2 = CHeuristicBase::getLowerLengthOfTheLongestPath(this->boardData, pos);

		pos = p_ai->GetMyPosition();
		int n1 = CHeuristicBase::getLowerLengthOfTheLongestPath(this->boardData, pos);
		cout << "\tEstimated length of our path : " << n1 << endl;
		cout << "\tEstimated length of enemy path : " << n2 << endl;
		if (n1 > n2)
			cout << "\t => It seems that we will fucking win  :)\n";
		else
			cout << "\t => It seems that we will fucking lost :(\n";
		pos = p_ai->GetMyPosition();
		TMove t = CHeuristicBase::getFirstMoveOfTheLongestPath(boardData, pos, ISOLATED_DEPTH);
		cout << "Time of this turn: " << timer->getTimeInMs() << " ms\n";
		return t;
	}
	// end ISOLATED MODE

	// start NORMAL_MODE

	TMove move = searcher.optimalMove(boardData, posPlayer1, posPlayer2, next);
	if (inEnemyTurn)
		cout << "Enemy turn: ";
	else
		cout << "Our   turn: ";
	cout << timer->getTimeInMs() << " ms\n";
	return move;
	// end NORMAL_MODE
}

CMyAI* CMyAI::getInstance()
{
	if (instance == NULL)
		instance = new CMyAI();
	return instance;
}

void CMyAI::printInformation()
{
	cout << "LouisLzcute's bot:";
	if (searcher.usingTT)
		cout << "\t  UseTT";
	else
		cout << "\tNoUseTT";
	cout << "\n\n\n";
}

