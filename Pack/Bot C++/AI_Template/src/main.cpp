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

CMyAI* pAI;

void threadInEnemyTurn()
{
	static CMyTimer *timer = CMyTimer::getInstance();
	timer->reset();
	pAI->calculatingInEnemyTurn = true;

	// run till pAI->inEnemyTurn == false
	// TMove direction = pAI->newTurn();

	while (pAI->inEnemyTurn)
	{

	}
	cout << "Enemy turn: " << timer->getTimeInMs() << " ms" << endl;

	pAI->calculatingInEnemyTurn = false;
	timer->reset();
}

void AI_Update()
{
	static CMyTimer *timer = CMyTimer::getInstance();
	timer->reset();
	thread* t;
	assert(pAI != NULL);
	if (pAI->p_ai->IsMyTurn()){
		pAI->inEnemyTurn = false;

		TMove direction = pAI->newTurn();
		Game::GetInstance()->AI_Move(direction);
	}
	else {
		//pAI->inEnemyTurn = true;
		//t = new thread(threadInEnemyTurn);
		//t->detach();
	}
}

void setupBoard(TBlock *board, const vector<Pos2D> *positions, Pos2D&p1 = Pos2D(0, 0), Pos2D &p2 = Pos2D(10, 10)){
	if (positions == NULL){
		createNewBoard(board, rand() % 20 + 5);
		board[0] = board[BOARD_SIZE - 1] = BLOCK_OBSTACLE;
		while (true) {
			int i = rand() % 121;
			if (board[i] == BLOCK_EMPTY){
				board[i] = BLOCK_PLAYER_1;
				p1 = Pos2D(i);
				break;
			}
		}
		while (true){
			int i = rand() % 121;
			if (board[i] == BLOCK_EMPTY){
				board[i] = BLOCK_PLAYER_2;
				p2 = Pos2D(i);
				break;
			}
		}
	}
	else {
		memset(board, 0, BOARD_SIZE*sizeof(TBlock));
		for (int j = 0; j < (int)positions->size(); j++){
			setBlock(board, positions->at(j), BLOCK_OBSTACLE);
		}
		setBlock(board, p1, BLOCK_PLAYER_1);
		setBlock(board, p2, BLOCK_PLAYER_2);
	}
}

void createBoardWithOutIsland(TBlock *board, Pos2D&p1 = Pos2D(0, 0), Pos2D &p2 = Pos2D(10, 10)){
	memset(board, 0, BOARD_SIZE*sizeof(TBlock));
	int nObject = rand() % 30 + 30;
	vector<int> obstacles = { 60 };
	for (int i = 0; i < 121; i++){
		setBlock(board, Pos2D(i), BLOCK_OBSTACLE);
	}

	setBlock(board, Pos2D(60), BLOCK_EMPTY);
	for (int i = 1; i < nObject;){
		Pos2D t = Pos2D(obstacles[rand() % obstacles.size()]);
		Pos2D newT = t.move(rand() % 4 + 1);

		if (getBlock(board, newT) == BLOCK_OBSTACLE){
			setBlock(board, newT, BLOCK_EMPTY);
			obstacles.push_back(newT.to1D());
			i++;
		}
	}
	p1 = obstacles[rand() % obstacles.size()];
	setBlock(board, p1, BLOCK_PLAYER_1);
}

void testFindPath(TBlock *board, const Pos2D&p = Pos2D(0, 0))
{
	Pos2D pos = p;
	int c = 0;
	int upper = 0, lower = 0;
	bool bOk;
	printBoard(board, false);
#ifdef OPENCV
	imshow("original board", toImage(board));
#endif // OPENCV
	upper = CHeuristicBase::getUpperLengthOfTheLongestPath(board, pos);
	cout << "Upper Estimated length : " << upper << endl;

	lower = CHeuristicBase::getLowerLengthOfTheLongestPath(board, pos);
	cout << "Lower Estimated length : " << lower << endl;
	int iCount = 0;

	while (c != 27){
		if (c == ' ' || c == -1){
			static bool out[4];
			if (getAvailableMoves(board, pos, out).size() == 0)
				break;
			CMyTimer::getInstance()->reset();
			TMove i = CHeuristicBase::getFirstMoveOfTheLongestPath(board, pos, 1);
			iCount++;
			bOk = move(board, pos, i); assert(bOk);
			pos = pos.move(i);
		}
#ifdef OPENCV
		imshow("game", toImage(board));
		c = waitKey(50);
#endif // OPENCV
		}
	cout << "Traveled length : " << iCount << endl << endl;
	if (iCount > upper || iCount < lower)
		system("pause");
	}

void testConnectedComponents(TBlock *board, Pos2D p = Pos2D(0, 0))
{
#ifdef OPENCV
	imshow("test", toImage(board));
#endif // OPENCV
	int n;
	TBlock board2[BOARD_SIZE];
	cout << endl;
	CBiconnectedComponentsOutput output;
	CBiconnectedComponents::biconnectedComponents(board, &output, p, Pos2D(-1, -1), board2);
	printBoard(board2, true);
	cout << "n = " << output.nAreas << endl;

	n = CBiconnectedComponents::getEstimatedLength(board, p, Pos2D(-1, -1));
	printf("estimated length = %d\n", n);
#ifdef OPENCV
	waitKey(100);
#endif // OPENCV
	system("pause");
}

void testEstimateLongestLength(TBlock* board, Pos2D p = Pos2D(0, 0)){
	createBoardWithOutIsland(board, p);
	testFindPath(board, p);
}

void testRateBoard(TBlock*board, const Pos2D &p1 = Pos2D(0, 0), const Pos2D &p2 = Pos2D(10, 10)){
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
	CHeuristicBase::pureTreeOfChamber(board, p1, p2, PLAYER_1);
	CHeuristicBase::simpleRateBoard(board, p1, p2, PLAYER_1);
}

void testSearchEngine(TBlock*board, const Pos2D &p1 = Pos2D(0, 0), const Pos2D &p2 = Pos2D(10, 10)){
	if (isIsolated(board, p1, p2))
		return;

	pAI->searcher.heuristic.rateBoard = CHeuristicBase::simpleRateBoard;
	pAI->searcher.heuristic.quickRateBoard = CHeuristicBase::simpleRateBoard;
	int depth = 5;
	//cout << "\tab:" << (ab = pAI->searcher.alphaBeta(board, p1, p2, PLAYER_1, depth, -MY_INFINITY, +MY_INFINITY));
	// cout << "\tnegaScout:" << (negaScout = pAI->searcher.negaScout(board, p1, p2, PLAYER_1, depth, -MY_INFINITY, +MY_INFINITY));
	cout << endl;

#ifdef OPENCV
	while (true){
		// imshow("test", toImage(board));
		int c = waitKey(1);
		// if (c == ' ')
		break;
}
#endif // OPENCV
	//assert(ab == negaScout);
	//assert(ab == negaMax);

	// 	if (ab == negawm)
	// 		cout << "good\n";
	// 	else {
	// 		cout << "bad!\n";
	// 		system("pause");
	// 		return;
	// 	}
}

void testGetArticulationPoints(TBlock *board, const Pos2D &p1 = Pos2D(0, 0), const Pos2D &p2 = Pos2D(10, 10)){
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
	std::srand(0);
	TBlock board[BOARD_SIZE];
	clock_t tStart = clock();
	for (int i = 0; i < 1000; i++){
		/*setupBoard(board, &p5, Pos2D(1, 9));
		testConnectedComponents(board, Pos2D(1, 9));

		setupBoard(board, &p5, Pos2D(0, 10));
		testConnectedComponents(board, Pos2D(0, 10));*/
		Pos2D p1(9, 5), p2(10, 10);
		setupBoard(board, NULL, p1, p2);
		// testConnectedComponents(board, p1);
		// cout << CHeuristicBase::getEstimatedLengthOfTheLongestPath(board, p1);
		testFindPath(board, p1);
		// testRateBoard(board, p1, p2);
		// testEstimateLongestLength(board, p1);
		// testSearchEngine(board, p1, p2);
		// testGetArticulationPoints(board, p1, p2);
	}

	printf("Time taken: %.2fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);
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