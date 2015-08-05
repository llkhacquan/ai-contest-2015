#pragma once

#include "mydefine.h"
#include "FastPos1DDeque.h"

class CBiconnectedComponentsOutput{
public:
	bool vXa[BOARD_SIZE][MAX_N_AREAS];
	signed char aXa[MAX_N_AREAS][MAX_N_AREAS];
	int nAreas;
	signed char nVertices[MAX_N_AREAS];
	signed char nOddVertices[MAX_N_AREAS];
	signed char nEvenVertices[MAX_N_AREAS];
	signed char nAreasOfVertices[BOARD_SIZE];
	signed char iAreaOfVertices[BOARD_SIZE][MAX_N_AREAS_PER_BLOCK];

	CBiconnectedComponentsOutput();

	void mark(int iArea, int iVertex, bool value = true);

	void clear();

	void manager(const Pos1D &playerPos);

	void buildAreaXArea(const Pos1D &playerPos);

	int findLengthOfLongestPath(const Pos1D &startPos)const;

	void checkConsitency(bool checkAxA = false)const;

	// void insertEdgeOf2Areas(const int a1, const int a2, const int u, const int v);

	void visitNode(CFastPos1DDeque &cPath, int &cLength, CFastPos1DDeque &lPath, int &lLength,
		bool *visitted, const int cCode, const int &startPos) const;

	int estimateLengthOfPath(const CFastPos1DDeque &path, const int &startPos) const;
	int calculateLengthBetween2ArticulationPointsIn1Area(const Pos1D &u, const Pos1D &v, const int areaCode) const;
};

