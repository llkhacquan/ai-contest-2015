#include <ai/Game.h>
#include <ai/AI.h>
#include <time.h>
#include <ctime>
#include "MyAI.h"
#include "mydefine.h"
#include "../AIInterface/BiconnectedComponents.h"
#include "../AIInterface/StaticFunctions.h"
#include "../AIInterface/HeuristicBase.h"
#include "../AIInterface/TranspositionTable.h"
#include "../AIInterface/MyTimer.h"

CMyAI* pAI;
// This function is called automatically each turn.
// If it's your turn, remember to call AI_Move() with a valid move before the time is run out.
// See <ai/Game.h> and <ai/AI.h> for supported APIs.
void AI_Update()
{
	assert(pAI != NULL);
	if (pAI->p_ai->IsMyTurn()){
		TMove direction = pAI->newTurn();
		Game::GetInstance()->AI_Move(direction);
	}
}

void setupBoard(TBlock *board, const vector<Pos2D> *positions, Pos2D&p1 = Pos2D(0, 0), Pos2D &p2 = Pos2D(10, 10)){
	if (positions == NULL){
		createNewBoard(board, rand() % 30 + 5);
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
	bool bOk;
	printBoard(board, false);
	cout << "Estimated length : " << CHeuristicBase::getEstimatedLengthOfTheLongestPath(board, pos) << endl;
	int iCount = 0;

	while (c != 27){
		if (c == ' ' || c == -1){
			static bool out[4];
			if (getAvailableMoves(board, pos, out).size() == 0)
				break;
			CMyTimer::getInstance()->reset();
			TMove i = CHeuristicBase::getFirstMoveOfTheLongestPath(board, pos, 3);
			iCount++;
			bOk = move(board, pos, i); assert(bOk);
			pos = pos.move(i);
		}
#ifdef OPENCV
		imshow("game", toImage(board));
		c = waitKey(50);
#endif // OPENCV
	}
	cout << "Traveled length : " << iCount << endl;
	// system("pause");
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
	CBiconnectedComponents::biconnectedComponents(board, p, &output, board2);
	cout << "n = " << output.nAreas << endl;

	n = CBiconnectedComponents::getEstimatedLength(board, p);
	printf("estimated length = %d\n", n);
#ifdef OPENCV
	waitKey(100);
#endif // OPENCV
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
	CHeuristicBase::treeOfChambersRateBoard(board, p1, p2, PLAYER_1);
	CHeuristicBase::simpleRateBoard(board, p1, p2, PLAYER_1);
}

void testOptimalMove(TBlock*board, const Pos2D &p1 = Pos2D(0, 0), const Pos2D &p2 = Pos2D(10, 10)){
	if (isIsolated(board, p1, p2))
		return;

	int ab, negawm, negaMax, negaScout, mtdf;
	int depth = 5;
	CMyTimer::getInstance()->reset();
	cout << "\tab:" << (ab = pAI->searcher.alphaBeta(board, p1, p2, PLAYER_1, depth, -MY_INFINITY, +MY_INFINITY));
	CMyTimer::getInstance()->reset();
	cout << "\tnegaScout:" << (negaScout = pAI->searcher.negaScout(board, p1, p2, PLAYER_1, depth, -MY_INFINITY, +MY_INFINITY));
	CMyTimer::getInstance()->reset();
	cout << "\tnega:" << (negaMax = pAI->searcher.negaMax(board, p1, p2, PLAYER_1, depth, -MY_INFINITY, +MY_INFINITY));
	// CMyTimer::getInstance()->reset();
	// cout << "\tabwm:" << (negawm = pAI->searcher.negaMaxWithMemory(board, p1, p2, PLAYER_1, depth, -MY_INFINITY, +MY_INFINITY));
	// CMyTimer::getInstance()->reset();
	// cout << "\tmtdf:" << (mtdf = pAI->searcher.iterative_deepening(board, p1, p2, PLAYER_1, depth));
	cout << endl;

#ifdef OPENCV
	while (true){
		imshow("test", toImage(board));
		int c = waitKey(100);
		// if (c == ' ')
		break;
	}
#endif // OPENCV
	assert(ab == negaScout);
	assert(ab == negaMax);

	// 	if (ab == negawm)
	// 		cout << "good\n";
	// 	else {
	// 		cout << "bad!\n";
	// 		system("pause");
	// 		return;
	// 	}
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
	std::srand(time(0));
	TBlock board[BOARD_SIZE];

	vector<Pos2D> _p1 = { Pos2D(2, 0), Pos2D(2, 0), Pos2D(3, 0), Pos2D(4, 1), Pos2D(4, 2), Pos2D(4, 3), Pos2D(4, 4), Pos2D(4, 5),
		Pos2D(3, 5), Pos2D(2, 5), Pos2D(1, 5), Pos2D(0, 5), Pos2D(0, 2), Pos2D(0, 3), Pos2D(0, 4) };
	clock_t tStart = clock();
	for (int i = 0; i < 1000; i++){
		/*setupBoard(board, &p5, Pos2D(1, 9));
		testConnectedComponents(board, Pos2D(1, 9));

		setupBoard(board, &p5, Pos2D(0, 10));
		testConnectedComponents(board, Pos2D(0, 10));*/
		Pos2D p1(0, 0), p2;
		setupBoard(board, NULL, p1, p2);
		// testConnectedComponents(board, p1);
		// cout << CHeuristicBase::getEstimatedLengthOfTheLongestPath(board, p1);
		// testFindPath(board, p1);
		// testRateBoard(board, p1, p2);
		// testEstimateLongestLength(board, p1);
		testOptimalMove(board, p1, p2);
	}

	printf("Time taken: %.2fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);
	return 0;
}

////////////////////////////////////////////////////////////
//                DON'T TOUCH THIS PART                   //
////////////////////////////////////////////////////////////


#if BOT_ACTIVE
int main(int argc, char* argv[])
#else
int main_(int argc, char* argv[])
#endif // BOT_ACTIVE

{
	cout << "LouisLzcute's bot" << endl;
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