#include "BiconnectedComponentsOutput.h"
#include "StaticFunctions.h"
#include "SmallDeque.h"
#include "MyAI.h"

CMyAI *pAI;

// calculate the max length lines in pathOfArea
int CBCO::getLengthInPath(const CSmallDeque &pathOfArea, const TPos &startPos)
{
	int size = pathOfArea.size();
	assert(size > 0);
	assert(startPos >= 0);
	CSmallDeque pathOfPos;
	pathOfPos.push_back(startPos);
	for (int i = 0; i < size - 1; i++){
		pathOfPos.push_back(aXa[pathOfArea[i]][pathOfArea[i + 1]]);
	}
	assert(size == pathOfPos.size());
	CSmallDeque lengths; // lengths[i] stores the length from startPos[0] to pathOfPos[i]; => lengths[0] = 0
	lengths.push_back(0);

	for (int i = 0; i < size - 1; i++){
		int delta = exactLengthInAnArea(pathOfPos[i], pathOfPos[i + 1], pathOfArea[i]);
		if (delta == TIMEOUT_POINTS)
			return TIMEOUT_POINTS;
		lengths.push_back(lengths.back() + delta - 1);
		assert(lengths.size() == i + 2);
	}
	assert(lengths.size() == size);

	int delta = exactLengthInAnArea(pathOfPos.back(), -1, pathOfArea.back());
	if (delta == TIMEOUT_POINTS)
		return TIMEOUT_POINTS;
	int maxLength = lengths.back() + delta - 1;

	// compare the maxLength with lengths[i] + lengthInAnArea(pathOfPos[i], -1, pathOfArea[i])
	for (int i = size - 2; i >= 0; i--){
		int dEstimate = estimateLengthInArea(pathOfPos[i], -1, pathOfArea[i]);
		if (lengths[i] + dEstimate - 1 > maxLength)
		{

			int dExact = exactLengthInAnArea(pathOfPos[i], -1, pathOfArea[i]);
			if (dExact == TIMEOUT_POINTS)
				return TIMEOUT_POINTS;
			if (lengths[i] + dExact - 1 > maxLength)
				maxLength = lengths[i] + dExact - 1;
		}
	}
	return maxLength;
}

// return the length between u and v (v==-1 for anywhere), when u==v, return 1
int CBCO::estimateLengthInArea(const TPos &u, const TPos &v, const int &areaCode)const
{
	assert(u >= 0 && u < BOARD_SIZE);
	// make sure u and v are in the same area
	assert(v < 0 || (vXa[u][areaCode] && vXa[v][areaCode]));

	int odd = nOddVinA[areaCode];
	int even = nEvenVinA[areaCode];

	if (v < 0 || v >= BOARD_SIZE){
		// end at anywhere
		int delta = -1;
		assert(vXa[u][areaCode]);
		if (odd == even)
			delta = odd * 2;
		else
		{
			int delta2;
			if (odd > even) // always end with odd
				if (u % 2 == 1) //start with odd
					delta2 = even * 2 + 1;
				else delta2 = even * 2; // start with even
			else // odd < even, end with even
				if (u % 2 == 1) //start with odd
					delta2 = odd * 2;
				else
					delta2 = odd * 2 + 1; // start with even, end with odd
			if (delta2 > delta)
				delta = delta2;
		}
		assert(delta >= 0);
		return delta;
	}
	else {
		// start at u, end at v
		assert(vXa[u][areaCode] && vXa[v][areaCode]);

		int delta = -1;
		{
			if (u == v) // start and end in the same position
				delta = 1;
			else if (u % 2 != v % 2) // start and end in odd/even position
				delta = min(odd, even) * 2;
			else if (u % 2 == 1) // start with odd, end with odd
				if (odd > even)
					delta = even * 2 + 1;
				else if (even > odd)
					delta = odd * 2 - 1;
				else // even == odd
					delta = odd * 2 - 1;
			else // if (it1->connections % 2 == 0) // start with even, end with even
				if (odd > even)
					delta = even * 2 - 1;
				else if (odd < even)
					delta = odd * 2 + 1;
				else // even == odd
					delta = even * 2 - 1;
		}
		assert(delta > 0);
		return delta;
	}
}

int CBCO::visitArea(CSmallDeque &cPath, int &cLength, CSmallDeque &lPath, int &lLength,
	bool *visitted, const int &cCode, const int &startPos)
{
	assert(cCode >= 0);
	assert(startPos >= 0);
	visitted[cCode] = true;
	cPath.push_back(cCode);

	CSmallDeque adjAreas;
	for (int iCode = 0; iCode < nAreas; iCode++){
		if (visitted[iCode])
			continue;
		if (aXa[cCode][iCode] >= 0) // != -1
			adjAreas.push_back(iCode);
	}

	if (adjAreas.size() == 0){
		cLength = getLengthInPath(cPath, startPos);
		if (cLength == TIMEOUT_POINTS)
			return TIMEOUT_POINTS;
		if (cLength > lLength){
			lLength = cLength;
			lPath = cPath;
		}
	}
	else
		for (int i = 0; i < (int)adjAreas.size(); i++)
		{
			int t = visitArea(cPath, cLength, lPath, lLength, visitted, adjAreas[i], startPos);
			if (t == TIMEOUT_POINTS)
				return t;
		}

	visitted[cCode] = false;
	assert(cPath[cPath.size() - 1] == cCode);
	cPath.pop_back();
	return 0;
}

int CBCO::findLengthOfLongestPath(const EXACT_LEVEL exact)
{
	this->outputExact = EXACT;
	this->inputExact = exact;
	if (nAreas == 0){
		assert(specialResult >= 0);
		return specialResult;
	}
	CSmallDeque lPath;
	int lLength = 0;
	bool visittedAreas[MAX_N_AREAS];
	memset(visittedAreas, false, MAX_N_AREAS);
	assert(nAofV[startPos] == 0);

	for (TMove i = 1; i <= 4; i++){
		CSmallDeque cPath;
		int cLength = 0;

		TPos newPos = MOVE(startPos, i);
		if (newPos < 0 || nAofV[newPos] == 0)
			continue;
		int t = visitArea(cPath, cLength, lPath, lLength, visittedAreas, iAofV[newPos][0], newPos);
		if (t == TIMEOUT_POINTS)
		{
			assert(exact);
			return t;
		}
	}
	return lLength + 1;
}

void CBCO::clear()
{
	nAreas = 0;
	memset(vXa, 0, sizeof(vXa[0][0])*BOARD_SIZE*MAX_N_AREAS);
	memset(aXa, -1, sizeof(aXa[0][0])*MAX_N_AREAS*MAX_N_AREAS);
	memset(nVinA, 0, sizeof(nVinA[0])*MAX_N_AREAS);
	memset(nOddVinA, 0, sizeof(nOddVinA[0])*MAX_N_AREAS);
	memset(nEvenVinA, 0, sizeof(nEvenVinA[0])*MAX_N_AREAS);
	memset(nAofV, 0, sizeof(nAofV[0]) * BOARD_SIZE);
	memset(iAofV, -1, sizeof(iAofV[0][0])*BOARD_SIZE * MAX_N_AREAS_PER_BLOCK);
	memset(exactLength, -1, sizeof(exactLength[0][0][0])*MAX_N_AREAS*BOARD_SIZE*(BOARD_SIZE + 1));
}

void CBCO::mark(const int iArea, const TPos iVertex)
{
	assert(iArea >= 0 && iArea < nAreas);
	assert(iVertex >= 0 && iVertex < BOARD_SIZE);

	if (vXa[iVertex][iArea])
		return;

	vXa[iVertex][iArea] = true;
	nVinA[iArea]++;
	if (iVertex % 2 == 0)
		nEvenVinA[iArea]++;
	else
		nOddVinA[iArea]++;
	assert(nAofV[iVertex] >= 0 && nAofV[iVertex] < MAX_N_AREAS_PER_BLOCK);
	iAofV[iVertex][nAofV[iVertex]] = iArea;
	nAofV[iVertex]++;
}

void CBCO::checkConsitency(const bool checkAxA) const
{
	if (nAreas > MAX_N_AREAS)
		return;
	// check nAreas
	for (int i = nAreas; i < MAX_N_AREAS; i++){
		assert(nVinA[i] == 0);
	}

	// check nVertices[], odd, even
	for (int iArea = 0; iArea < MAX_N_AREAS; iArea++){
		int nOdd = 0;
		int nEven = 0;
		for (int iVertex = 0; iVertex < BOARD_SIZE; iVertex++){
			if (vXa[iVertex][iArea]){
				if (iVertex % 2 == 0)
					nEven++;
				else
					nOdd++;
			}
		}
		assert(nOdd == nOddVinA[iArea]);
		assert(nEven == nEvenVinA[iArea]);
		assert(nOdd + nEven == nVinA[iArea]);
		assert(nOdd + nEven == nVinA[iArea]);
	}

	// check nAreasOfVertices
	for (int iVertex = 0; iVertex < BOARD_SIZE; iVertex++){
		int nAreasOfPos_ = 0;
		for (int iArea = 0; iArea < MAX_N_AREAS; iArea++){
			if (vXa[iVertex][iArea])
				nAreasOfPos_++;
		}
		assert(nAreasOfPos_ == nAofV[iVertex]);
	}

	if (!checkAxA)
		return;
	// check iAreasOfVertices
	for (int iVertex = 0; iVertex < BOARD_SIZE; iVertex++){
		assert(0 <= nAofV[iVertex] && nAofV[iVertex] <= MAX_N_AREAS_PER_BLOCK);
		for (int i = 0; i < MAX_N_AREAS_PER_BLOCK; i++)
		{
			if (i < nAofV[iVertex])
			{
				assert(iAofV[iVertex][i] >= 0);
				assert(vXa[iVertex][iAofV[iVertex][i]]);
			}
			else {
				assert(iAofV[iVertex][i] < 0);
			}
		}
	}

	// check AxA
	for (int i = 0; i < MAX_N_AREAS; i++){
		for (int j = 0; j < MAX_N_AREAS; j++){
			assert(aXa[i][j] == aXa[j][i]);
			if (aXa[i][j] < 0)
			{
				continue;
			}
			TPos v = aXa[i][j];
			assert(vXa[v][i] && vXa[v][j]);
		}
	}
}

void CBCO::buildAreaXArea()
{
	for (TPos u = 0; u < BOARD_SIZE; u++){
		assert(nAofV[u] >= 0 && nAofV[u] <= MAX_N_AREAS_PER_BLOCK);
		if (nAofV[u] > 1){
			for (int i = 0; i < nAofV[u]; i++){
				for (int j = 0; j < nAofV[u]; j++){
					if (i == j) continue;
					aXa[iAofV[u][i]][iAofV[u][j]] = u;
					aXa[iAofV[u][j]][iAofV[u][i]] = u;
				}

			}
		}
	}
}

CBCO::CBCO(){
	clear();
}

int explorerCount;
int CBCO::exactLengthInAnArea(const TPos &u, const TPos &v, const int areaCode)
{
	assert(u >= 0 && u < BOARD_SIZE);
	// check if we calculated this length
	if (v < 0){ // v < 0 means v can be any pos
		if (exactLength[areaCode][u][BOARD_SIZE] >= 0){
			return exactLength[areaCode][u][BOARD_SIZE];
		}
	}
	else { // v is a real pos
		if (exactLength[areaCode][u][v] >= 0){
			assert(exactLength[areaCode][u][v] == exactLength[areaCode][v][u]);
			return exactLength[areaCode][u][v];
		}
	}

	int estimatedMaxLength = estimateLengthInArea(u, v, areaCode);
	if (u == v){
		assert(estimatedMaxLength == 1);
		return 1;
	}

	if (inputExact == ESTIMATE_ALL)
	{
		outputExact = ESTIMATE_ALL;
		return estimatedMaxLength;
	}

	if (inputExact == EXACT_AREA_BELOW_10 && estimatedMaxLength > 10)
	{
		if (outputExact > EXACT_AREA_BELOW_10)
			outputExact = EXACT_AREA_BELOW_10;
		return estimatedMaxLength;
	}

	if (inputExact == EXACT_AREA_BELOW_25 && estimatedMaxLength > 25)
	{
		if (outputExact > EXACT_AREA_BELOW_25)
			outputExact = EXACT_AREA_BELOW_25;
		return estimatedMaxLength;
	}

	bool visitted[BOARD_SIZE];
	memset(visitted, false, BOARD_SIZE);

	int minDistanceFromUtoV = -1;
	if (v >= 0)
	{
		// calculate minDistanceFromUtoV
		signed char d[BOARD_SIZE];
		memset(d, -1, BOARD_SIZE);
		TPos cPos = u;
		d[u] = 0;
		CSmallDeque remainPos;
		remainPos.push_back(cPos);
		bool seen = false;
		while (!remainPos.empty() && !seen){
			cPos = remainPos.pop_front();
			for (TMove m = 1; m < 5; m++){
				TPos newP = MOVE(cPos, m);
				if (newP < 0 || !vXa[newP][areaCode] || (d[newP] >= 0))
					continue;
				d[newP] = d[cPos] + 1;
				if (newP == v)
				{
					minDistanceFromUtoV = d[newP];
					seen = true;
					break;
				}
				remainPos.push_back(newP);
			}
		}
		assert(minDistanceFromUtoV > 0);
	}

	CSmallDeque path;
	path.push_back(u);
	int foundMaxLength = 1;
	visitted[u] = true;
	explorerCount = 0;
	int b = explorer(path, visitted, v, estimatedMaxLength, foundMaxLength, areaCode, minDistanceFromUtoV);
	if (b)
		assert(foundMaxLength == estimatedMaxLength);

	assert(foundMaxLength <= estimatedMaxLength);

	if (v >= 0)
		assert((estimatedMaxLength - foundMaxLength) % 2 == 0);

	if (v < 0){
		exactLength[areaCode][u][BOARD_SIZE] = foundMaxLength;
	}
	else {
		exactLength[areaCode][u][v] = foundMaxLength;
		exactLength[areaCode][v][u] = foundMaxLength;
	}

	return foundMaxLength;
}

#define EXPLORER_TIMES 15
int CBCO::explorer(CSmallDeque &path, bool visitted[], const TPos &v, const int &overEstimatedLength, int &currentMaxLength, const int &areaCode, const int &dUV) const
{
	if (inputExact == EXACT && explorerCount >= (EXPLORER_TIMES - 1) && pAI->shouldEndMoveNow())
		return TIMEOUT_POINTS;
	explorerCount++;
	if (explorerCount >= EXPLORER_TIMES)
		explorerCount = 0;

	assert(v < 0 || dUV > 0);
	TPos u = path.back();
	for (TMove m = 1; m <= 4; m++){
		TPos u2 = MOVE(u, m);
		if (u2 < 0
			|| !vXa[u2][areaCode]
			|| visitted[u2])
			continue;

		visitted[u2] = true;
		path.push_back(u2);

		if (u2 == v){ // finish path here
			if (currentMaxLength < (int)path.size())
				currentMaxLength = path.size();
			assert(currentMaxLength <= overEstimatedLength);
			if (v != -1 && ((overEstimatedLength - currentMaxLength) % 2 == 1))
				currentMaxLength++;
			if (currentMaxLength == overEstimatedLength)
			{
				currentMaxLength = overEstimatedLength;
				return true;
			}
		}
		else { // continue explorer
			bool canMeet = true;
			if (v >= 0 && ((int)path.size()) > dUV){ // v != u2 now
				// calculate if we can meet v or not; if v<0, of course canMeet = true
				canMeet = false;
				bool visitted2[BOARD_SIZE];
				memcpy(visitted2, visitted, BOARD_SIZE);
				TPos cPos = path.back(); assert(cPos == u2);
				TPos ePos = v;
				CSmallDeque remainingPos;
				remainingPos.push_back(cPos); assert(visitted2[cPos]);
				while (!remainingPos.empty() && !canMeet)
				{
					cPos = remainingPos.pop_back();
					for (TMove m2 = 1; m2 <= 4; m2++){
						TPos u3 = MOVE(cPos, m2);
						if (u3 < 0
							|| !vXa[u3][areaCode]
							|| visitted2[u3])
							continue;

						if (u3 == v){
							// canMeet = truly true, don't need to calculate more, exit this if
							canMeet = true;
							break;
						}
						visitted2[u3] = true;
						remainingPos.push_back(u3);
					}
				}
			}

			if (canMeet)
			{
				int b = explorer(path, visitted, v, overEstimatedLength, currentMaxLength, areaCode, dUV);
				if (b == TIMEOUT_POINTS)
					return TIMEOUT_POINTS;
				if (b)
					return true;
			}
		}
		path.pop_back();
		visitted[u2] = false;
	}
	if (v == -1)
	{
		if (currentMaxLength < (int)path.size())
			currentMaxLength = path.size();
		assert(currentMaxLength <= overEstimatedLength);
		if (v != -1 && ((overEstimatedLength - currentMaxLength) % 2 == 1))
			currentMaxLength++;
		if (currentMaxLength == overEstimatedLength)
		{
			currentMaxLength = overEstimatedLength;
			return true;
		}
	}
	return false;
}

int CBCO::exactLengthInAnAreaWithoutRecursion(const TPos &u, const TPos &v, const int &areaCode)
{
	assert(u >= 0 && u < BOARD_SIZE);
	//check if we calculated this length
	if (v < 0){
		if (exactLength[areaCode][u][BOARD_SIZE] >= 0){
			return exactLength[areaCode][u][BOARD_SIZE];
		}
	}
	else { // v is a real pos
		if (exactLength[areaCode][u][v] >= 0){
			assert(exactLength[areaCode][u][v] == exactLength[areaCode][v][u]);
			return exactLength[areaCode][u][v];
		}
	}

	int estimatedMaxLength = estimateLengthInArea(u, v, areaCode);
	if (u == v){
		assert(estimatedMaxLength == 1);
		return 1;
	}

	int foundMaxLength = 0;

	// create the map
	bool visited[BOARD_SIZE];
	for (TPos i = 0; i < BOARD_SIZE; i++){
		if (!vXa[i][areaCode])
		{
			visited[i] = true;
		}
		else
		{
			visited[i] = false;
		}
	}
	visited[u] = true;
	if (v < 0)
		foundMaxLength = exploreMapWithoutRecursion(visited, estimatedMaxLength - 1, u) + 1;
	else
		foundMaxLength = exploreMapWithoutRecursion(visited, estimatedMaxLength - 1, u, v) + 1;

	assert(foundMaxLength <= estimatedMaxLength);

	if (v != -1)
		assert((estimatedMaxLength - foundMaxLength) % 2 == 0);

	if (v < 0){
		exactLength[areaCode][u][BOARD_SIZE] = foundMaxLength;
	}
	else {
		exactLength[areaCode][u][v] = foundMaxLength;
		exactLength[areaCode][v][u] = foundMaxLength;
	}

	return foundMaxLength;
}
