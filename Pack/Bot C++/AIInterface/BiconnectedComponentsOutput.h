#pragma once

#include "mydefine.h"
#include "FastPos1DDeque.h"

class CBiconnectedComponentsOutput{
public:
	int depth;
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

	void manager(const Pos1D &playerPos, const Pos1D &endPos);

	void buildAreaXArea(const Pos1D &playerPos);

	int findLengthOfLongestPath(const Pos1D &startPos, const Pos1D &endPos)const;

	void checkConsitency(bool checkAxA = false)const;

	void insertEdgeOf2Areas(const int a1, const int a2, const int u, const int v);

	void visitNode(CFastPos1DDeque &cPath, int &cLength, CFastPos1DDeque &lPath, int &lLength,
		bool *visitted, const int cCode, const int &startPos, const int &endPos = -1) const;

	int estimateLengthOfPath(const CFastPos1DDeque &path, const int &startPos) const;
	int calculateLengthBetween2NodeIn1Area(const Pos1D &u, const Pos1D &v, int depth = 0) const;
};

