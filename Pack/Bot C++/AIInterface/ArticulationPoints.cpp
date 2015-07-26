#include "ArticulationPoints.h"


CArticulationPoints::CArticulationPoints()
{
}


CArticulationPoints::~CArticulationPoints()
{
}

void CArticulationPoints::getArticulationPoints(const TBlock board[], const Pos2D& _p1, const Pos2D&_p2, TBlock oBoard[])
{
	// Mark all the vertices as not visited
	memcpy(oBoard, board, sizeof(TBlock)*BOARD_SIZE);
	bool visited[BOARD_SIZE];
	int disc[BOARD_SIZE];
	int low[BOARD_SIZE];
	int parent[BOARD_SIZE];

	// Initialize parent and visited, and ap(articulation point) arrays
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		parent[i] = -1;
		visited[i] = false;
	}

	// Call the recursive helper function to find articulation points
	// in DFS tree rooted with vertex 'i'
	for (int i = 0; i < BOARD_SIZE; i++)
		if (visited[i] == false && getBlock(board, i) == BLOCK_EMPTY)
			APUtil(board, oBoard, i, visited, disc, low, parent);
}

void CArticulationPoints::APUtil(const TBlock board[], TBlock oBoard[], int u, bool visited[], int disc[], int low[], int parent[])
{
	// A static variable is used for simplicity, we can avoid use of static
	// variable by passing a pointer.
	static int time = 0;

	// Count of children in DFS Tree
	int children = 0;

	// Mark the current node as visited
	visited[u] = true;

	// Initialize discovery time and low value
	disc[u] = low[u] = ++time;

	// Go through all vertices adjacent to this
	for (int iMove = 1; iMove <= 4; iMove++)
	{
		Pos2D v2d = Pos2D(u).move(iMove);
		if (getBlock(board, v2d) != BLOCK_EMPTY)
			continue;
		int v = v2d;  // v is current adjacent of u

		// If v is not visited yet, then make it a child of u
		// in DFS tree and recur for it
		if (!visited[v])
		{
			children++;
			parent[v] = u;
			APUtil(board, oBoard, v, visited, disc, low, parent);

			// Check if the subtree rooted with v has a connection to
			// one of the ancestors of u
			low[u] = min(low[u], low[v]);

			// u is an articulation point in following cases

			// (1) u is root of DFS tree and has two or more children.
			if (parent[u] == -1 && children > 1)
				oBoard[u] = SPECIAL_BLOCK;

			// (2) If u is not root and low value of one of its child is more
			// than discovery value of u.
			if (parent[u] != -1 && low[v] >= disc[u])
				oBoard[u] = SPECIAL_BLOCK;
		}

		// Update low value of u for parent function calls.
		else if (v != parent[u])
			low[u] = min(low[u], disc[v]);
	}
}
