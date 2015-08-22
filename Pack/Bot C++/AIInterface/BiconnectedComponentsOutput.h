#pragma once

#include "mydefine.h"
#include "SmallDeque.h"

enum EXACT_LEVEL {
	ESTIMATE_ALL, // fastest, estimate all
	EXACT_AREA_BELOW_10, // calculate exact area that have estimate <= 10
	EXACT_AREA_BELOW_25, // calculate exact area that have estimate <= 20
	EXACT, // calculate exact everything
};

class CBCO{
public:
	TPos startPos;

	int nAreas;
	bool vXa[BOARD_SIZE][MAX_N_AREAS];
	signed char aXa[MAX_N_AREAS][MAX_N_AREAS];
	signed char nVinA[MAX_N_AREAS];
	signed char nOddVinA[MAX_N_AREAS];
	signed char nEvenVinA[MAX_N_AREAS];
	signed char nAofV[BOARD_SIZE];
	signed char iAofV[BOARD_SIZE][MAX_N_AREAS_PER_BLOCK];
	signed char exactLength[MAX_N_AREAS][BOARD_SIZE][BOARD_SIZE + 1]; // exactLength from a pos to a pos in an area

	EXACT_LEVEL inputExact = ESTIMATE_ALL;
	EXACT_LEVEL outputExact = EXACT;
	int specialResult = -1;

	CBCO();

	void mark(const int iArea, const TPos iVertex);
	void clear();
	void buildAreaXArea();
	void checkConsitency(const bool checkAxA = false) const;
	int findLengthOfLongestPath(const EXACT_LEVEL exact);
private:
	int visitArea(CSmallDeque &cPath, int &cLength, CSmallDeque &lPath, int &lLength,
		bool *visitted, const int &cCode, const int &startPos);
	int getLengthInPath(const CSmallDeque &path, const TPos &startPos);
	int estimateLengthInArea(const TPos &u, const TPos &v, const int &areaCode) const;
	int exactLengthInAnArea(const TPos &u, const TPos &v, const int areaCode);
	int exactLengthInAnAreaWithoutRecursion(const TPos &u, const TPos &v, const int &areaCode);
	int explorer(CSmallDeque &path, bool visitted[], const TPos &v, const int &realMaxLength,
		int &foundMaxLength, const int &areaCode, const int &dUV) const;
};

