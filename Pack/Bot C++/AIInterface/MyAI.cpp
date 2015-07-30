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

// calculate and give an optimal move for ourselves 
TMove CMyAI::newTurn()
{
	// update board first

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

	auto newBoard = p_ai->GetBoard();

	// update newBoard to boardData and players' positions
	for (int i = 0; i < BOARD_SIZE; i++){
		boardData[i] = newBoard[i];
	}

	if (!firstMoveIsOver) // assume this only run 1
	{
		assert(boardData[0] == BLOCK_PLAYER_1 || boardData[120] == BLOCK_PLAYER_2);
		newGame();
	}
	else{
		// calculate history
		if (next == PLAYER_1)
		{
			for (int i = 1; i <= 4; i++){
				if (oP2.move(i) == p2)
				{
					history.push_back(i);
					break;
				}
			}
		}
		else {
			for (int i = 1; i <= 4; i++){
				if (oP1.move(i) == p1)
				{
					history.push_back(i);
					break;
				}
			}
		}
	}
	oP1 = p1; oP2 = p2;

	if (first == next)
		assert(history.size() % 2 == 0);
	else
		assert(history.size() % 2 == 1);

	if (we == next)
		return ourNewTurn();
	else
		return enemyNewTurn();

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

void CMyAI::newGame()
{
	memcpy(originalBoard, p_ai->GetBoard(), sizeof(int)*BOARD_SIZE);
	history.clear();
	firstMoveIsOver = true;
	cout << "===================start a new game=====================" << endl;

	if (we == PLAYER_1)
	{
		cout << "\t\twe are player 1" << endl;
	}
	else
	{
		cout << "\t\twe are player 2" << endl;
	}
	if (next == we)
	{
		cout << "\tYeah! We go first!!!" << endl;
		first = next;
	}
	else
	{
		cout << "\tOh shit! We do not go first!" << endl;
		first = next;
	}
}

TMove CMyAI::ourNewTurn()
{
	assert(!inEnemyTurn);
	TMove result;
	static CMyTimer *timer = CMyTimer::getInstance();

	if (activeIsolatedMode || isIsolated(this->boardData, this->p1, this->p2)){
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
			cout << "\t => It seems that we will fucking WIN  :)\n";
		else
			cout << "\t => It seems that we will fucking LOST :(\n";
		pos = p_ai->GetMyPosition();
		TMove t = CHeuristicBase::getFirstMoveOfTheLongestPath(boardData, pos, ISOLATED_DEPTH);
		result = t;
	}
	else
	{
		result = searcher.optimalMove(boardData, p1, p2, next);
	}
	cout << "We take " << timer->getTimeInMs() << " ms\n";
	return result;
}

TMove CMyAI::enemyNewTurn()
{
	assert(inEnemyTurn);
	TMove result;
	static CMyTimer *timer = CMyTimer::getInstance();

	if (activeIsolatedMode || isIsolated(this->boardData, this->p1, this->p2)){
		activeIsolatedMode = true;
		Pos2D pos;
		pos = p_ai->GetEnemyPosition();
		result = CHeuristicBase::getFirstMoveOfTheLongestPath(boardData, pos, ISOLATED_DEPTH);
	}
	else
	{
		result = searcher.optimalMove(boardData, p1, p2, next);
	}
	return result;
}

bool CMyAI::shouldEndMoveNow()
{
	static CMyTimer* timer = CMyTimer::getInstance();

	if (timer->timeUp())
		return true;
	if (isCalculatingInEnemyTurn && !inEnemyTurn)
		return true;

	return false;
}

