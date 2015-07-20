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
	int(*rateBoard)(TBlock _board[], const Pos2D &_player1, const Pos2D &_player2, const int nextPlayer);

	static int treeOfChambersRateBoard(TBlock _board[], const Pos2D &_player1, const Pos2D &_player2, const int nextPlayer);
	static int simpleRateBoard(TBlock _board[], const Pos2D &_player1, const Pos2D &_player2, const int nextPlayer);


	// this method explore the board with nDepth
	static TMove getFirstMoveOfTheLongestPath(const TBlock board[], const Pos2D &p, const int nDepth = 0);

	static int getEstimatedLengthOfTheLongestPath(TBlock const board[], const Pos2D &playerPos);

	static void sortMoves(vector<TMove> &moves, TBlock* board, const Pos2D &_player1, const Pos2D &_player2, const TPlayer next);

	static int evaluateBoard(const TBlock _board[], const Pos2D &_player1, const Pos2D &_player2, const TPlayer nextPlayer, int &point);
	static int calculatePotentialPoint(const TBlock * board, const Pos2D & _p, TBlock * oBoard, TBlock * distanceBoard);
	static void constructNewGraphUsedInRateBoard(const TBlock oBoard1[], const Pos2D &_p1, vector<Area> &areas, int* numberOfConnectionsToEnemyArea, set<Edge> &edgesOfCode);
};

