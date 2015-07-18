#include <ai/Game.h>
#include <ai/AI.h>
#include <time.h>
#include <ctime>
#include "MyAI.h"
#include "mydefine.h"
#include "../AIInterface/BiconnectedComponents.h"
#include "../AIInterface/StaticFunctions.h"
#include "../AIInterface/HeuristicBase.h"

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
		TMove direction = pAI->newTurn();
		Game::GetInstance()->AI_Move(direction);
	}
}

void setupBoard(int *board, const vector<Pos2D> *positions, Pos2D&p1 = Pos2D(0, 0), Pos2D &p2 = Pos2D(10, 10)){
	if (positions == NULL){
		createNewBoard(board, rand() % 25 + 5);
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
		memset(board, 0, BOARD_SIZE*sizeof(int));
		for (int j = 0; j < (int)positions->size(); j++){
			setBlock(board, positions->at(j), BLOCK_OBSTACLE);
		}
		setBlock(board, p1, BLOCK_PLAYER_1);
		setBlock(board, p2, BLOCK_PLAYER_2);
	}
}

void testIsolatedMode(int *board, const Pos2D&p = Pos2D(0, 0))
{
	Pos2D pos = p;
	int c = 0;
	bool bOk;
	int iCount = 0;
	while (c != 27){
		if (c == ' ' || c == -1){
			static bool out[4];
			if (getAvailableMoves(board, pos, out).size() == 0)
				break;
			TMove i = CHeuristicBase::getFirstMoveOfTheLongestPath(board, pos);
			bOk = move(board, pos, i); assert(bOk);
			pos = pos.move(i);
		}
#ifdef OPENCV
		imshow("game", toImage(board));
		c = waitKey(50);
#endif // OPENCV
	}
}

void testConnectedComponents(int *board, Pos2D p = Pos2D(0, 0))
{
	int board2[BOARD_SIZE];
#ifdef OPENCV
	imshow("test", toImage(board));
#endif // OPENCV
	cout << endl;
	int n = CBiconnectedComponents::biconnectedComponents(board, p, board2).size();
	cout << "n = " << n << endl;
	printBoard(board2, true);
	cout << endl;

	n = CBiconnectedComponents::getEstimatedLength(board, p);
	printf("estimated length = %d\n", n);
#ifdef OPENCV
	waitKey(100);
#endif // OPENCV
}

void testRateBoard(int*board, const Pos2D &p1 = Pos2D(0, 0), const Pos2D &p2 = Pos2D(10, 10)){
	CHeuristicBase::rateBoard(board, p1, p2, PLAYER_1);
	CHeuristicBase::rateBoard2(board, p1, p2, PLAYER_1);
#ifdef OPENCV
	while (true){
		imshow("test", toImage(board));
		int c = waitKey(200);
		// if (c == ' ')
		break;
	}
#endif // OPENCV
}

#ifdef BOT_ACTIVE
int main_(int argc, char* argv[])
#else
int main(int argc, char* argv[])
#endif // BOT_ACTIVE
{
	// test fillDistance
	pAI = new CMyAI();
#ifdef OPENCV
	setupImage();
#endif // OPENCV
	std::srand((int)std::time(0));
	int board[BOARD_SIZE];

	vector<Pos2D> p1 = {
		Pos2D(0, 3), Pos2D(0, 7), Pos2D(1, 3), Pos2D(1, 7), Pos2D(2, 3), Pos2D(2, 6), Pos2D(3, 3), Pos2D(4, 3), Pos2D(4, 8), Pos2D(5, 8),
		Pos2D(5, 9), Pos2D(6, 5), Pos2D(6, 7), Pos2D(6, 10), Pos2D(7, 1), Pos2D(7, 2), Pos2D(7, 3), Pos2D(7, 4), Pos2D(8, 7), Pos2D(8, 8),
		Pos2D(8, 9), Pos2D(8, 10), Pos2D(9, 1), Pos2D(9, 2), Pos2D(9, 3), Pos2D(10, 5) };

	vector<Pos2D> p2 = {
		Pos2D(1, 1), Pos2D(2, 2), Pos2D(3, 3), Pos2D(4, 4), Pos2D(4, 5), Pos2D(4, 6), Pos2D(4, 4)
		, Pos2D(0, 6), Pos2D(1, 6), Pos2D(2, 6), Pos2D(3, 6)
		, Pos2D(6, 0), Pos2D(6, 1), Pos2D(6, 2), Pos2D(6, 3), Pos2D(6, 4)
		, Pos2D(6, 5), Pos2D(6, 6), Pos2D(6, 7), Pos2D(6, 8)
		, Pos2D(8, 0), Pos2D(8, 1), Pos2D(8, 2) };

	vector<Pos2D> p3 = {
		Pos2D(2, 0), Pos2D(2, 1), Pos2D(2, 2), Pos2D(2, 3), Pos2D(2, 4), Pos2D(2, 5), Pos2D(2, 6), Pos2D(2, 7), Pos2D(1, 3), Pos2D(1, 4), Pos2D(1, 2) };
	vector<Pos2D> p4 = { Pos2D(0, 2), Pos2D(2, 2), Pos2D(3, 0), Pos2D(3, 1), Pos2D(1, 2), Pos2D(1, 3) };
	vector<Pos2D> p5 = { Pos2D(3, 0), Pos2D(3, 1), Pos2D(2, 2), Pos2D(2, 3), Pos2D(3, 3), Pos2D(4, 3), Pos2D(5, 3), Pos2D(6, 3), Pos2D(7, 3), Pos2D(8, 3),
		Pos2D(9, 3), Pos2D(9, 4), Pos2D(9, 5), Pos2D(9, 6), Pos2D(9, 7), Pos2D(9, 8),
		Pos2D(0, 8), Pos2D(1, 8), Pos2D(02, 8), Pos2D(3, 8), Pos2D(4, 8), Pos2D(5, 8), Pos2D(6, 8), Pos2D(7, 8), Pos2D(8, 8), Pos2D(10, 10), Pos2D(0, 9) };
	vector<Pos2D> p6 = {
		Pos2D(0, 2), Pos2D(1, 2), Pos2D(2, 2), Pos2D(3, 2), Pos2D(3, 3), Pos2D(4, 3), Pos2D(5, 3),
		Pos2D(6, 3), Pos2D(7, 3), Pos2D(8, 3), Pos2D(4, 4), Pos2D(9, 4), Pos2D(10, 4),
		Pos2D(0, 6), Pos2D(2, 7), Pos2D(7, 7), Pos2D(8, 1), Pos2D(4, 8), Pos2D(5, 10),
	};

	vector<Pos2D> p7 = { Pos2D(0, 5), Pos2D(1, 5), Pos2D(1, 6), Pos2D(2, 5), Pos2D(2, 7), Pos2D(2, 9), Pos2D(2, 10), Pos2D(3, 4), Pos2D(3, 9),
		Pos2D(0, 5), Pos2D(4, 3), Pos2D(4, 6), Pos2D(5, 3), Pos2D(5, 7), Pos2D(6, 0), Pos2D(6, 1), Pos2D(6, 2), Pos2D(6, 4), Pos2D(6, 7), Pos2D(6, 9), Pos2D(6, 10),
		Pos2D(7, 5), Pos2D(7, 6), Pos2D(8, 2), Pos2D(8, 3), Pos2D(9, 5), Pos2D(9, 8),
		Pos2D(10, 1), Pos2D(10, 2), Pos2D(10, 3), Pos2D(10, 8), };
	clock_t tStart = clock();
	for (int i = 0; i < 10000; i++){
		/*setupBoard(board, &p5, Pos2D(1, 9));
		testConnectedComponents(board, Pos2D(1, 9));

		setupBoard(board, &p5, Pos2D(0, 10));
		testConnectedComponents(board, Pos2D(0, 10));*/
		Pos2D p1, p2;
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


#ifdef BOT_ACTIVE
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

	pAI = new CMyAI();

	p_Game->PollingFromServer();

	Game::DestroyInstance();

#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
}