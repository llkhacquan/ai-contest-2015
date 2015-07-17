#include <ai/Game.h>
#include <ai/AI.h>
#include <time.h>
#include <ctime>
#include "MyAI.h"
#include "mydefine.h"
#include "BiconnectedComponents.h"

// ==================== HOW TO RUN THIS =====================
// Call:
// "AI_Template.exe -h [host] -p [port] -k [key]"
//
// If no argument given, it'll be 127.0.0.1:3011
// key is a secret string that authenticate the bot identity
// it is not required when testing
// ===========================================================

//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//                                    GAME RULES                                    //
//                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////
// - Game board is an array of MAP_SIZExMAP_SIZE blocks                             //
// - 2 players starts at 2 corners of the game board                                //
// - Each player will take turn to move                                             //
// - Player can only move left/right/up/down and stay inside the game board         //
// - The game is over when one of 2 players cannot make a valid move                //
// - In a competitive match:                                                        //
//   + A player will lose if they cannot connect to server within 10 seconds        //
//   + A player will lose if they don't make a valid move within 3 seconds          //
//////////////////////////////////////////////////////////////////////////////////////
CMyAI* pAI;
// This function is called automatically each turn.
// If it's your turn, remember to call AI_Move() with a valid move before the time is run out.
// See <ai/Game.h> and <ai/AI.h> for supported APIs.
void AI_Update()
{
	assert(pAI != NULL);
	if (pAI->p_ai->IsMyTurn()){
		Direction direction = pAI->newTurn();
		Game::GetInstance()->AI_Move(direction);
	}
}

void setupBoard(int *board, const vector<CPos> *positions, CPos&p1 = CPos(0, 0), CPos &p2 = CPos(10, 10)){
	if (positions == NULL){
		CMyAI::createNewBoard(board, rand() % 25 + 5);
		board[0] = board[BOARD_SIZE - 1] = BLOCK_OBSTACLE;
		while (true) {
			int i = rand() % 121;
			if (board[i] == BLOCK_EMPTY){
				board[i] = BLOCK_PLAYER_1;
				p1 = CPos(i);
				break;
			}
		}
		while (true){
			int i = rand() % 121;
			if (board[i] == BLOCK_EMPTY){
				board[i] = BLOCK_PLAYER_2;
				p2 = CPos(i);
				break;
			}
		}
	}
	else {
		memset(board, 0, BOARD_SIZE*sizeof(int));
		for (int j = 0; j < (int)positions->size(); j++){
			CMyAI::setBlock(board, positions->at(j), BLOCK_OBSTACLE);
		}
		CMyAI::setBlock(board, p1, BLOCK_PLAYER_1);
		CMyAI::setBlock(board, p2, BLOCK_PLAYER_2);
	}
}

void testIsolatedMode(int *board, const CPos&p = CPos(0, 0))
{
	CPos pos = p;
	int c = 0;
	bool bOk;
	int iCount = 0;
	while (c != 27){
		if (c == ' ' || c == -1){
			if (CMyAI::getAvailableMoves(board, pos).size() == 0)
				break;
			Direction i = CMyAI::getNextMoveOfLongestPath(board, pos);
			bOk = CMyAI::move(board, pos, i); assert(bOk);
			pos = pos.move(i);
		}
#ifdef OPENCV
		imshow("game", CMyAI::toImage(board));
		c = waitKey(50);
#endif // OPENCV
	}
}

void testConnectedComponents(int *board, CPos p = CPos(0, 0))
{
	int board2[BOARD_SIZE];
#ifdef OPENCV
	imshow("test", CMyAI::toImage(board));
#endif // OPENCV
	cout << endl;
	int n = CBiconnectedComponents::biconnectedComponents(board, p, board2).size();
	cout << "n = " << n << endl;
	CMyAI::printBoard(board2, true);
	cout << endl;

	n = CBiconnectedComponents::getEstimatedLength(board, p);
	printf("estimated length = %d\n", n);
#ifdef OPENCV
	waitKey(100);
#endif // OPENCV
}

void testRateBoard(int*board, const CPos &p1 = CPos(0, 0), const CPos &p2 = CPos(10, 10)){
	CMyAI::rateBoard(board, p1, p2, PLAYER_1);
	CMyAI::rateBoard2(board, p1, p2, PLAYER_1);
#ifdef OPENCV
	while (true){
		imshow("test", CMyAI::toImage(board));
		int c = waitKey(200);
		if (c == ' ')
			break;
	}
#endif // OPENCV
}

int main(int argc, char* argv[]){
	// test fillDistance
	pAI = new CMyAI();
	std::srand((int)std::time(0));
	int board[BOARD_SIZE];

	vector<CPos> p1 = {
		CPos(0, 3), CPos(0, 7), CPos(1, 3), CPos(1, 7), CPos(2, 3), CPos(2, 6), CPos(3, 3), CPos(4, 3), CPos(4, 8), CPos(5, 8),
		CPos(5, 9), CPos(6, 5), CPos(6, 7), CPos(6, 10), CPos(7, 1), CPos(7, 2), CPos(7, 3), CPos(7, 4), CPos(8, 7), CPos(8, 8),
		CPos(8, 9), CPos(8, 10), CPos(9, 1), CPos(9, 2), CPos(9, 3), CPos(10, 5) };

	vector<CPos> p2 = {
		CPos(1, 1), CPos(2, 2), CPos(3, 3), CPos(4, 4), CPos(4, 5), CPos(4, 6), CPos(4, 4)
		, CPos(0, 6), CPos(1, 6), CPos(2, 6), CPos(3, 6)
		, CPos(6, 0), CPos(6, 1), CPos(6, 2), CPos(6, 3), CPos(6, 4)
		, CPos(6, 5), CPos(6, 6), CPos(6, 7), CPos(6, 8)
		, CPos(8, 0), CPos(8, 1), CPos(8, 2) };

	vector<CPos> p3 = {
		CPos(2, 0), CPos(2, 1), CPos(2, 2), CPos(2, 3), CPos(2, 4), CPos(2, 5), CPos(2, 6), CPos(2, 7), CPos(1, 3), CPos(1, 4), CPos(1, 2) };
	vector<CPos> p4 = { CPos(0, 2), CPos(2, 2), CPos(3, 0), CPos(3, 1), CPos(1, 2), CPos(1, 3) };
	vector<CPos> p5 = { CPos(3, 0), CPos(3, 1), CPos(2, 2), CPos(2, 3), CPos(3, 3), CPos(4, 3), CPos(5, 3), CPos(6, 3), CPos(7, 3), CPos(8, 3),
		CPos(9, 3), CPos(9, 4), CPos(9, 5), CPos(9, 6), CPos(9, 7), CPos(9, 8),
		CPos(0, 8), CPos(1, 8), CPos(02, 8), CPos(3, 8), CPos(4, 8), CPos(5, 8), CPos(6, 8), CPos(7, 8), CPos(8, 8), CPos(10, 10), CPos(0, 9) };
	vector<CPos> p6 = {
		CPos(0, 2), CPos(1, 2), CPos(2, 2), CPos(3, 2), CPos(3, 3), CPos(4, 3), CPos(5, 3),
		CPos(6, 3), CPos(7, 3), CPos(8, 3), CPos(4, 4), CPos(9, 4), CPos(10, 4),
		CPos(0, 6), CPos(2, 7), CPos(7, 7), CPos(8, 1), CPos(4, 8), CPos(5, 10),
	};

	vector<CPos> p7 = { CPos(0, 5), CPos(1, 5), CPos(1, 6), CPos(2, 5), CPos(2, 7), CPos(2, 9), CPos(2, 10), CPos(3, 4), CPos(3, 9),
		CPos(0, 5), CPos(4, 3), CPos(4, 6), CPos(5, 3), CPos(5, 7), CPos(6, 0), CPos(6, 1), CPos(6, 2), CPos(6, 4), CPos(6, 7), CPos(6, 9), CPos(6, 10),
		CPos(7, 5), CPos(7, 6), CPos(8, 2), CPos(8, 3), CPos(9, 5), CPos(9, 8),
		CPos(10, 1), CPos(10, 2), CPos(10, 3), CPos(10, 8), };
	clock_t tStart = clock();
	for (int i = 0; i < 100; i++){
		/*setupBoard(board, &p5, CPos(1, 9));
		testConnectedComponents(board, CPos(1, 9));

		setupBoard(board, &p5, CPos(0, 10));
		testConnectedComponents(board, CPos(0, 10));*/
		CPos p1, p2;
		setupBoard(board, NULL, p1, p2);
		//testIsolatedMode(board);
		testRateBoard(board, p1, p2);
	}

	printf("Time taken: %.2fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);
	return 0;
}

////////////////////////////////////////////////////////////
//                DON'T TOUCH THIS PART                   //
////////////////////////////////////////////////////////////

int main_(int argc, char* argv[])
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

	pAI = new CMyAI();

	p_Game->PollingFromServer();

	Game::DestroyInstance();

#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
}