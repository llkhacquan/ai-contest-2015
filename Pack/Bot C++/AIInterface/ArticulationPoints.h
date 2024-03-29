#pragma once
#include "mydefine.h"
#include "StaticFunctions.h"
class CArticulationPoints
{
private:
	CArticulationPoints();
	~CArticulationPoints();

	// A recursive function that find articulation points using DFS traversal
	// u --> The vertex to be visited next
	// visited[] --> keeps tract of visited vertices
	// disc[] --> Stores discovery times of visited vertices
	// parent[] --> Stores parent vertices in DFS tree
	static void APUtil(const TBlock board[], TBlock oBoard[], int u, bool visited[], int disc[], int low[], int parent[]);

public:
	static void getArticulationPoints(const TBlock board[], const TPos& _p1, const TPos&_p2, TBlock oBoard[]);
};

