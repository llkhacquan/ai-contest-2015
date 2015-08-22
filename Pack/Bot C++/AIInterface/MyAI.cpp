#include "MyAI.h"
#include "mydefine.h"
#include "StaticFunctions.h"
#include "HeuristicBase.h"
#include "SearchEngine.h"
#include "TranspositionTable.h"
#include "MyTimer.h"

CMyAI* CMyAI::instance;

TPos moveTable[BOARD_SIZE][4];

CMyAI::CMyAI()
{
	p_ai = AI::GetInstance();
	CTranspositionTable::getInstance();

	searcher.flag = CSearchEngine::ALPHA_BETA_ITERATIVE_DEEPENING;
	searcher.heuristic.rateBoard = NULL;
	searcher.heuristic.quickRateBoard = &CHeuristicBase::voronoiRateBoard;

	history.reserve(BOARD_SIZE);
	printInformation();

	for (TPos p = 0; p < BOARD_SIZE; p++)
		for (TMove m = 1; m <= 4; m++){
			int y = p / MAP_SIZE;
			int x = p - y * MAP_SIZE;
			switch (m){
			case DIRECTION_DOWN:
				if (y + 1 < MAP_SIZE)
					moveTable[p][m - 1] = p + MAP_SIZE;
				else
					moveTable[p][m - 1] = BLOCK_OUT_OF_BOARD;
				break;
			case DIRECTION_UP:
				if (y - 1 >= 0)
					moveTable[p][m - 1] = p - MAP_SIZE;
				else
					moveTable[p][m - 1] = BLOCK_OUT_OF_BOARD;
				break;
			case DIRECTION_LEFT:
				if (x - 1 >= 0)
					moveTable[p][m - 1] = p - 1;
				else
					moveTable[p][m - 1] = BLOCK_OUT_OF_BOARD;
				break;
			case DIRECTION_RIGHT:
				if (x + 1 < MAP_SIZE)
					moveTable[p][m - 1] = p + 1;
				else
					moveTable[p][m - 1] = BLOCK_OUT_OF_BOARD;
				break;
			}
		}
}

CMyAI::~CMyAI()
{
	instance = NULL;
}

TMove CMyAI::newTurn()
{
	// update board first

	if (p_ai->GetBlock(p_ai->GetMyPosition()) == BLOCK_PLAYER_1){
		we = PLAYER_1;
		p1 = p_ai->GetMyPosition().x + p_ai->GetMyPosition().y*MAP_SIZE;
		p2 = p_ai->GetEnemyPosition().x + p_ai->GetEnemyPosition().y*MAP_SIZE;
		next = p_ai->IsMyTurn() ? PLAYER_1 : PLAYER_2;
	}
	else {
		we = PLAYER_2;
		p2 = p_ai->GetMyPosition().x + p_ai->GetMyPosition().y*MAP_SIZE;
		p1 = p_ai->GetEnemyPosition().x + p_ai->GetEnemyPosition().y*MAP_SIZE;
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
				if (MOVE(oP2, i) == p2)
				{
					history.push_back(i);
					break;
				}
			}
		}
		else {
			for (int i = 1; i <= 4; i++){
				if (MOVE(oP1, i) == p1)
				{
					history.push_back(i);
					break;
				}
			}
		}
	}
	oP1 = p1; oP2 = p2;

	TMove m;
	if (we == next)
	{
		m = ourNewTurn();
		if (m < 1 || m>4){
			cout << "calculated move: " << (int)m << endl;
			system("pause");
		}
	}
	else
		m = enemyNewTurn();

	return m;
}

CMyAI* CMyAI::getInstance()
{
	if (instance == NULL)
		instance = new CMyAI();
	return instance;
}

void CMyAI::printInformation()
{
	cout << "\n\nLOUISLZCUTE'S BOT:";
	if (USING_MEMORY)
		cout << "\tActiveTT";
	else
		cout << "\tDeactiveTT";
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

	int count7x7 = 0;
	for (int x = 2; x <= 8; x++){
		for (int y = 2; y <= 8; y++){
			if (originalBoard[CC(x, y)] == BLOCK_OBSTACLE)
				count7x7++;
		}
	}

	int count3x3 = 0;
	for (int x = 4; x <= 6; x++){
		for (int y = 4; y <= 6; y++){
			if (originalBoard[CC(x, y)] == BLOCK_OBSTACLE)
				count3x3++;
		}
	}

	if (count7x7 >= 8 || (count3x3 >= 2 && count7x7 >= 6)){
		searcher.heuristic.rateBoard = &CHeuristicBase::simpleRateBoard;
	}
	else
	{
		searcher.heuristic.rateBoard = &CHeuristicBase::voronoiRateBoard;
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
		TPos p1, p2;
		p1 = CC(p_ai->GetMyPosition().x, p_ai->GetMyPosition().y);
		p2 = CC(p_ai->GetEnemyPosition().x, p_ai->GetEnemyPosition().y);

		static CBCO o1, o2;
		CBC::calculateBCs(this->boardData, &o1, p1);
		CBC::calculateBCs(this->boardData, &o2, p2);
		int n1, n2;
		n1 = o1.findLengthOfLongestPath(EXACT_AREA_BELOW_10);
		n2 = o2.findLengthOfLongestPath(EXACT_AREA_BELOW_10);
		if (abs(n1 - n2) < 4)
		{
			int _n1 = o1.findLengthOfLongestPath(EXACT);
			if (_n1 != TIMEOUT_POINTS)
			{
				n1 = _n1;
				int _n2 = o2.findLengthOfLongestPath(EXACT);
				if (_n2 != TIMEOUT_POINTS)
					n2 = _n2;
			}
		}

		cout << "\tEstimated length of our path	   " << n1 << endl;
		cout << "\tEstimated length of enemy path  " << n2 << endl;
		if (n1 > n2)
			cout << "\t => It seems that we will fucking WIN  ";
		else
			cout << "\t => It seems that we will fucking LOSE ";
		if (first == we)
			cout << "this easy game " << n1 - n2 << endl;
		else
			cout << "this hard game " << n1 - n2 << endl;
		p1 = CC(p_ai->GetMyPosition().x, p_ai->GetMyPosition().y);
		TMove estimateMove = CHeuristicBase::getFirstMove(boardData, p1, EXACT_AREA_BELOW_25, n1);
		TMove exactMove = CHeuristicBase::getFirstMove(boardData, p1, EXACT, n1);
		if (exactMove < 1 || exactMove > 4)
			result = estimateMove;
		else
			result = exactMove;

		result = estimateMove;
	}
	else
	{
		static bool followingMode = TRY_FOLLOWING && we != first;
		if (followingMode){
			TPos u = MOVE(we == PLAYER_1 ? p1 : p2, getOpositeDirection(history.back())) != BLOCK_EMPTY;
			if (GET_BLOCK(boardData, u))
				followingMode = false;
		}

		if (followingMode) {
			assert(history.size() % 2 == 1);
			for (unsigned int i = 0; i < history.size() - 1; i = i + 2){
				if (history[i] != getOpositeDirection(history[i + 1]))
				{
					followingMode = false;
					break;
				}
			}

		}
		else{
			followingMode = false;
		}

		pair<TMove, int> re = searcher.optimalMove(boardData, p1, p2, next, history);
		if (we == PLAYER_2)
			re.second = -re.second;

		if (!followingMode || re.first == getOpositeDirection(history.back()))
		{
			result = re.first;
		}
		else {
			if (re.second >= 0)
				result = re.first;
			else
			{
				result = getOpositeDirection(history.back());
				cout << "\t===FOLLOWING ENEMY===\n";
			}
		}
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
		TPos pos;
		pos = CC(p_ai->GetEnemyPosition().x, p_ai->GetEnemyPosition().y);
		result = 0;
	}
	else
	{
		int before = history.size();
		result = searcher.optimalMove(boardData, p1, p2, next, history).first;
		assert(history.size() == before);
	}
	return result;
}

