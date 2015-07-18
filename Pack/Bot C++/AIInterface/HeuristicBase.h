#pragma once
#include "..\AI_Template\mydefine.h"
#include "Pos2D.h"
#include "StaticFunctions.h"
#include "BiconnectedComponents.h"
class CHeuristicBase
{
public:
	CHeuristicBase();
	~CHeuristicBase();

	// this method explore the board with nDepth
	static TMove getFirstMoveOfTheLongestPath(const int board[], const Pos2D &p, const int nDepth = 0);

	static int getEstimatedLengthOfTheLongestPath(int const board[], const Pos2D &playerPos);

	static void sortMoves(vector<TMove> moves, const Pos2D &_player1, const Pos2D &_player2, const TPlayer nextPlayer);

	static int evaluateBoard(const int _board[], const Pos2D &_player1, const Pos2D &_player2, const TPlayer nextPlayer);
	static int rateBoard(int _board[], const Pos2D &_player1, const Pos2D &_player2, const int nextPlayer);
	static int rateBoard2(int _board[], const Pos2D &_player1, const Pos2D &_player2, const int nextPlayer);
	static int calculatePotentialPoint(const int * board, const Pos2D & _p, int * oBoard, int * distanceBoard);
	static void constructNewGraphUsedInRateBoard(const int oBoard1[], const Pos2D &_p1, vector<Area> &areas, int* numberOfConnectionsToEnemyArea, set<Edge> &edgesOfCode);
};

