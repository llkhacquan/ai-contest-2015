#pragma once

#include "..\AI_Template\mydefine.h"

class CBiconnectedComponentsOutput{
private:
	bool checkConsitency()const;
public:
	bool vXa[BOARD_SIZE][MAX_N_AREAS];
	signed char aXa[MAX_N_AREAS][MAX_N_AREAS][4];
	int nAreas;
	signed char nVertices[MAX_N_AREAS];
	signed char nOddVertices[MAX_N_AREAS];
	signed char nEvenVertices[MAX_N_AREAS];
	signed char nAreasOfVertices[BOARD_SIZE];
	signed char iAreaOfVertices[BOARD_SIZE];

	CBiconnectedComponentsOutput();

	void mark(int iArea, int iVertex, bool value = true);

	void clear();

	void manager(const Pos1D &playerPos);

	void buildAreaXArea(const Pos1D &playerPos);

	int findLengthOfLongestPath(const Pos1D &startPos)const;

private:

	void insertEdgeOf2Areas(const int a1, const int a2, const int u, const int v);

	void visitNode(vector<int> &cPath, int &cLength, vector<int> &lPath, int &lLength,
		bool *visitted, const int cCode, const int &startPos) const;

	int calculateLengthOfPath(const vector<int> &path, const int &startPos) const;
};

