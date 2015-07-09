#pragma once
#include "ai\defines.h"
#include "ai\AI.h"
#include "mydefine.h"
#include <vector>
#include "Pos.h"

using namespace std;

class CMyAI
{
public:
	AI *p_ai;
	static int DEPTH;

	int boardData[BOARD_SIZE];
	CPos posPlayer1, posPlayer2, *myPos = NULL, *enemyPos = NULL;
	Player we;

	// basic operators
	int getBlock(const int x, const int y);
	static int getBlock(int const board[], const Vertex pos);
	static int getBlock(int const board[], const int x, const int y);
	static int getBlock(int const board[], const CPos &pos);

	bool setBlock(const int x, const int y, const int value);
	static bool setBlock(int board[], const Vertex pos, const int value);
	static bool setBlock(int board[], const int x, const int y, const int value);
	static bool setBlock(int board[], const CPos &pos, const int value);

	static int getOpositeDirection(const int direction);

	static bool move(int _board[], const CPos &player, const int direction, const bool backMode = false);
	void move(int direction);

	static vector<Direction> getAvailableMoves(const int board[], const CPos &pos);
	static vector<CPos> getNearEmptyBlock(const int board[], const CPos &pos);

	// minimax algorithm
	static int ab(int board[], const CPos&_p1, const CPos&_p2, Player next, int depth, int a, int b);
	static int evaluateBoard(const int _board[], const CPos &_player1, const CPos &_player2, const Player nextPlayer);
	static int rateBoard(int _board[], const CPos &_player1, const CPos &_player2, const int nextPlayer);
	static int rateBoard2(int _board[], const CPos &_player1, const CPos &_player2, const int nextPlayer);
	static void sortMoves(vector<Direction> moves, const CPos &_player1, const CPos &_player2, const Player nextPlayer);
	static int isIsolated(const int _boardData[], const CPos &pos1, const CPos &pos2);
	bool isIsolated();
	static int fillDistance(int board[], const CPos &pos);
	
	// longest path
	static Direction getNextMoveOfLongestPath(const int boardData[], const CPos &pos);
	static vector<int> getALongestPath(const int boardData[], const CPos &pos);
	static vector<Direction> findShortestPath(const int board[], const CPos& start, CPos& end);
	static int getLengthOfLongestPath(const int boardData[], const CPos &pos);
	static void findLongestPath(int board[], CPos& pos, vector<Direction> &v, vector<Direction> &l);
	
	// DEBUG FUNCTIONS
	static void printBoard(const int board[], const bool special = false);
	static void createNewBoard(int board[], int noRandomMove = 0);
	static cv::Mat toImage(int board[], bool special = false);
	// MAIN 
	void updateBoard(const int* newBoard, const CPos &_p1, const CPos &_p2, const Player next, const Player we);
	Direction newTurn();
	static Direction optimalMove(int board[], const CPos &_pos1, const CPos &_pos2, const Player nextPlayer);

	CMyAI();
	~CMyAI();


	static cv::Mat empty;
	static cv::Mat obstacle;
	static cv::Mat player_1;
	static cv::Mat player_1_trail;
	static cv::Mat player_2;
	static cv::Mat player_2_trail;
};

