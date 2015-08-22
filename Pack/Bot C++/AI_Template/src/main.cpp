#include <ai/Game.h>
#include <ai/AI.h>
#include "../AIInterface/MyAI.h"
#include "../AIInterface/mydefine.h"
#include "../AIInterface/BiconnectedComponents.h"
#include "../AIInterface/StaticFunctions.h"
#include "../AIInterface/HeuristicBase.h"
#include "../AIInterface/TranspositionTable.h"
#include "../AIInterface/MyTimer.h"
#include "../AIInterface/ArticulationPoints.h"

extern CMyAI* pAI;

thread *t;

void threadInEnemyTurn()
{
	static CMyTimer *timer = CMyTimer::getInstance();
	timer->reset();
	pAI->iTurn++;
	cout << "\n=============================================\nTurn number " << pAI->iTurn << endl;
	pAI->isCalculatingInEnemyTurn = true;
	// newTurn should exit when pAI->inEnemyTurn = false
	pAI->newTurn();
	while (pAI->inEnemyTurn && (!DISABLE_TIMEOUT)){
		this_thread::sleep_for(chrono::milliseconds(5));
	}
	cout << "Enemy take " << timer->getTimeInMs() << " ms" << endl;

	pAI->isCalculatingInEnemyTurn = false;
	timer->reset();
}

void AI_Update()
{
	static CMyTimer *timer = CMyTimer::getInstance();
	assert(pAI != NULL);
	if (pAI->p_ai->IsMyTurn()){
		pAI->inEnemyTurn = false;

		while (pAI->isCalculatingInEnemyTurn){
			this_thread::sleep_for(chrono::milliseconds(5));
		}

		pAI->iTurn++;
		cout << "\n=============================================\nTurn number " << pAI->iTurn << endl;
		TMove direction = pAI->newTurn();
		assert(direction >= 1 && direction <= 4);
		Game::GetInstance()->AI_Move(direction);
	}
	else {
		pAI->inEnemyTurn = true;
		t = new thread(threadInEnemyTurn);
		t->detach();
	}
}

void testFindPath(TBlock *board, const TPos&p);

void setupBoard(TBlock *board, const vector<TPos> *positions, TPos &p1, TPos &p2){
	if (positions == NULL){
		// createNewBoard(board, rand() % 20 + 5);
		createNewBoard(board, rand() % 20 + 15);
		board[0] = board[BOARD_SIZE - 1] = BLOCK_OBSTACLE;
		while (true) {
			int i = rand() % 121;
			if (board[i] == BLOCK_EMPTY){
				board[i] = BLOCK_PLAYER_1;
				p1 = TPos(i);
				break;
			}
		}
		while (true){
			int i = rand() % 121;
			if (board[i] == BLOCK_EMPTY){
				board[i] = BLOCK_PLAYER_2;
				p2 = TPos(i);
				break;
			}
		}
	}
	else {
		memset(board, 0, BOARD_SIZE*sizeof(TBlock));
		for (int j = 0; j < (int)positions->size(); j++){
			SET_BLOCK(board, positions->at(j), BLOCK_OBSTACLE);
		}
		SET_BLOCK(board, p1, BLOCK_PLAYER_1);
		SET_BLOCK(board, p2, BLOCK_PLAYER_2);
	}
}

void createBoardWithOutIsland(TBlock *board, TPos p1 = 0i16, TPos p2 = 120i16){
	memset(board, 0, BOARD_SIZE*sizeof(TBlock));
	int nObject = rand() % 30 + 30;
	vector<TPos> obstacles = { 60 };
	for (int i = 0; i < 121; i++){
		SET_BLOCK(board, TPos(i), BLOCK_OBSTACLE);
	}

	SET_BLOCK(board, TPos(60), BLOCK_EMPTY);
	for (int i = 1; i < nObject;){
		TPos t = TPos(obstacles[rand() % obstacles.size()]);
		TPos newT = MOVE(t, rand() % 4 + 1);

		if (GET_BLOCK(board, newT) == BLOCK_OBSTACLE){
			SET_BLOCK(board, newT, BLOCK_EMPTY);
			obstacles.push_back(newT);
			i++;
		}
	}
	p1 = obstacles[rand() % obstacles.size()];
	SET_BLOCK(board, p1, BLOCK_PLAYER_1);
}

void testEstimateLongestLength(TBlock* board, TPos p = 0){
	createBoardWithOutIsland(board, p);
	testFindPath(board, p);
}

void testRateBoard(TBlock*board, const TPos p1 = 0, const TPos p2 = 120){
	if (isIsolated(board, p1, p2))
		return;
#ifdef OPENCV
	while (true){
		imshow("test", toImage(board));
		int c = waitKey(10);
		// if (c == ' ')
		break;
	}
#endif // OPENCV
	CHeuristicBase::simpleRateBoard(board, p1, p2, PLAYER_1);
}

int bad, good;

void testSearchEngine(TBlock*board, const TPos p1 = 0, const TPos p2 = 120){
	if (isIsolated(board, p1, p2))
		return;

	pAI->searcher.heuristic.rateBoard = CHeuristicBase::simpleRateBoard;
	pAI->searcher.heuristic.quickRateBoard = CHeuristicBase::voronoiRateBoard;
	int ab, abtt, nega;
	int depth = 6;
	CMyTimer::getInstance()->reset();
	//	cout << "\tab  :" << (ab = pAI->searcher.alphaBeta(board, p1, p2, PLAYER_1, depth, -MY_INFINITY, +MY_INFINITY)) << endl;

	CMyTimer::getInstance()->reset();
	//cout << "\tabtt:" << (abtt = pAI->searcher.alphaBetaTT(board, p1, p2, PLAYER_1, depth, -MY_INFINITY, +MY_INFINITY)) << endl;

	CMyTimer::getInstance()->reset();
	//cout << "\tnega:" << (nega = pAI->searcher.negaMaxTT(board, p1, p2, PLAYER_1, depth, -MY_INFINITY, +MY_INFINITY)) << endl;

	CTranspositionTable::getInstance()->printStatic();
	cout << endl;

#ifdef OPENCV
	while (true){
		// imshow("test", toImage(board));
		int c = waitKey(1);
		// if (c == ' ')
		break;
	}
#endif // OPENCV
	// assert(ab == abtt);
	assert(abtt == nega);

	if (ab == abtt && ab == nega)
	{
		cout << "good\n";
		good++;
	}
	else
	{
		cout << "bad!\n";
		// system("pause");
		bad++;
	}
}

void testGetArticulationPoints(TBlock *board, const TPos p1 = 0, const TPos p2 = 120){
#ifdef OPENCV
	imshow("test", toImage(board));
#endif // OPENCV
	TBlock oBoard[BOARD_SIZE];
	CArticulationPoints::getArticulationPoints(board, p1, p2, oBoard);
	printBoard(oBoard, true);

#ifdef OPENCV
	waitKey(100);
#endif // OPENCV
	system("pause");
}

int traveled_exact = 0;
int lowwer_extract = 0;
int upper_extract = 0;
int nTimes = 0;
void testFindPath(TBlock *board, const TPos&p = 0)
{
	static CMyTimer* timer = CMyTimer::getInstance();
	TPos pos = p;
	int c = 0;
	int upper = 0, lower = 0, exact = 0, calculatedExact = 0;
	bool bOk;

#ifdef OPENCV
	imshow("original board", toImage(board));
	waitKey(1);
#endif // OPENCV
	upper = CBC::calculateLength(board, pos, true, EXACT_AREA_BELOW_10);
	assert(upper != TIMEOUT_POINTS);
	if (upper > 45)
		return;

// 	{
// 		int t1, t2;
// 		static CBCO o;
// 		CBC::calculateBCs(board, &o, pos);
// 		t1 = CBC::calculateLength(board, pos, false, ESTIMATE_ALL);
// 		t2 = o.findLengthOfLongestPath(ESTIMATE_ALL);
// 		assert(t1 == t2);
// 
// 		t1 = CBC::calculateLength(board, pos, false, EXAXT_AREA_BELOW_10);
// 		t2 = o.findLengthOfLongestPath(EXAXT_AREA_BELOW_10);
// 		assert(t1 == t2);
// 
// 		t1 = CBC::calculateLength(board, pos, false, EXAXT_AREA_BELOW_25);
// 		t2 = o.findLengthOfLongestPath(EXAXT_AREA_BELOW_25);
// 		assert(t1 == t2);
// 
// 		t1 = CBC::calculateLength(board, pos, false, EXACT);
// 		t2 = o.findLengthOfLongestPath(EXACT);
// 		assert(t1 == t2);
// 
// 		return;
// 	}

	nTimes++;
	cout << "Upper Estimated length : " << upper << endl;

	timer->reset();
	calculatedExact = CBC::calculateLength(board, pos, false, EXACT);
	assert(calculatedExact != TIMEOUT_POINTS);
	int timeInMs = timer->getTimeInMs();
	cout << "Exact Calculated length : " << calculatedExact << " calculated in " << timeInMs << " ms\n";

	exact = exploreMapWithoutRecursion(board, BOARD_SIZE, pos);
	cout << "Exact length		: " << exact << endl;

	lower = CHeuristicBase::getLowerLengthOfTheLongestPath(board, pos);
	cout << "Lower Estimated length : " << lower << endl;
	int travelled = 0;
	while (true){
		if (getAvailableMoves(board, pos).size() == 0)
			break;
		timer->reset();
		TMove i = CHeuristicBase::getFirstMove(board, pos, EXACT, -1);
		int temp = CBC::calculateLength(board, pos, false, EXACT);
		assert(temp != TIMEOUT_POINTS);
		assert(travelled + temp == exact);
		travelled++;
		bOk = move(board, pos, i); assert(bOk);
		pos = MOVE(pos, i);
#ifdef OPENCV
		imshow("game", toImage(board));
		waitKey(1);
#endif // OPENCV
	}
	cout << "Traveled length : " << travelled << endl << endl;
	if (lower == exact)
		lowwer_extract++;
	if (travelled == exact)
		traveled_exact++;
	if (upper > exact)
		upper_extract++;
	if (calculatedExact != exact || calculatedExact != travelled || upper < exact)
		system("pause");
}

#if BOT_ACTIVE
int main_(int argc, char* argv[])
#else
int main(int argc, char* argv[])
#endif // BOT_ACTIVE
{
	// test fillDistance
	pAI = CMyAI::getInstance();
#ifdef OPENCV
	setupImage();
#endif // OPENCV
	int seed = (int)time(NULL);
	std::srand(seed);
	TBlock board[BOARD_SIZE];
	clock_t tStart = clock();

	vector<TPos> points = { CC(5, 0), CC(5, 1), CC(5, 2), CC(6, 3), CC(6, 4),
		CC(7, 5), CC(6, 6), CC(7, 6), CC(7, 7), CC(8, 7), CC(9, 0), CC(9, 2),
		CC(9, 8), CC(9, 9), CC(10, 10), };

	for (int i = 0; i < 10000; i++){
		TPos p1(CC(6, 0)), p2(120);
		setupBoard(board, NULL, p1, p2);
		// testConnectedComponents(board, p1);
		// cout << CHeuristicBase::getEstimatedLengthOfTheLongestPath(board, p1);
		testFindPath(board, p1);
		// testRateBoard(board, p1, p2);
		// testEstimateLongestLength(board, p1);
		// testSearchEngine(board, p1, p2);
		// testGetArticulationPoints(board, p1, p2);
	}

	cout << "lowwer_extract " << lowwer_extract << endl;
	printf("Time taken: %.2fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);
	system("pause");
	return 0;
}


#if BOT_ACTIVE
int main(int argc, char* argv[])
#else
int main_(int argc, char* argv[])
#endif // BOT_ACTIVE
{
	srand(clock());


#ifdef _WIN32
	INT rc;
	WSADATA wsaData;

	rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (rc) {
		printf("WSAStartup Failed.\n");
		return 1;
	}
#endif

	Game::CreateInstance();
	Game * p_Game = Game::GetInstance();

	// Create connection
	if (p_Game->Connect(argc, argv) == -1)
	{
		LOG("Failed to connect to server!\n");
		return -1;
	}

	// Set up function pointer
	AI::GetInstance()->Update = &AI_Update;

	pAI = CMyAI::getInstance();
	CTranspositionTable::getInstance();

	p_Game->PollingFromServer();

	Game::DestroyInstance();

#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
}