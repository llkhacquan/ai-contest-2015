#include "BiconnectedComponentsOutput.h"
#include "StaticFunctions.h"
#include "FastPos1DDeque.h"


int CBiconnectedComponentsOutput::estimateLengthOfPath(const CFastPos1DDeque &path, const int &startPos) const
{
	assert(path.size() > 0);
	assert(nVertices[path[0]] == 1);

	int result = 0; // skip the first area (areas[path[0]])
	CFastPos1DDeque newPath;
	newPath.push_back(startPos);

	// for the last area)
	int pathBack = path[path.size() - 1];
	if (path.size() >= 2) {
		Pos1D connection = aXa[pathBack][path[path.size() - 2]];
		int delta = calculateLengthBetween2ArticulationPointsIn1Area(connection, -1, pathBack);
		result += delta - 1;
	}

	// for the other areas (path[1] -> path[size - 2]
	if (path.size() >= 3)
		for (int iArea = 1; iArea < (int)path.size() - 1; iArea++){
			Pos1D start = aXa[path[iArea]][path[iArea - 1]];
			assert(start >= 0 && start < BOARD_SIZE);
			Pos1D end = aXa[path[iArea]][path[iArea + 1]];;
			assert(end >= 0 && end < BOARD_SIZE);
			int delta = calculateLengthBetween2ArticulationPointsIn1Area(start, end, path[iArea]);
			assert(delta > 0);
			result += delta - 1;
		}
	return result;
}

int CBiconnectedComponentsOutput::calculateLengthBetween2ArticulationPointsIn1Area(const Pos1D &u, const Pos1D &v, const int areaCode)const{
	// make sure u and v are in the same area
	assert(u >= 0 && u < BOARD_SIZE);

	if (v < 0 || v >= BOARD_SIZE){
		// end at anywhere
		int delta = -1;
		assert(vXa[u][areaCode]);
		int odd = nOddVertices[areaCode];
		int even = nEvenVertices[areaCode];
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
		int even = nEvenVertices[areaCode];
		int odd = nOddVertices[areaCode];

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

void CBiconnectedComponentsOutput::visitNode(CFastPos1DDeque &cPath, int &cLength, CFastPos1DDeque &lPath, int &lLength,
	bool *visitted, const int cCode, const int &startPos) const
{
	visitted[cCode] = true;
	cPath.push_back(cCode);

	CFastPos1DDeque adjAreas;
	for (int iCode = 0; iCode < nAreas; iCode++){
		if (visitted[iCode])
			continue;
		if (aXa[cCode][iCode] >= 0) // != -1
			adjAreas.push_back(iCode);
	}

	if (adjAreas.size() == 0){
		cLength = estimateLengthOfPath(cPath, startPos);
		if (cLength > lLength){
			lLength = cLength;
			lPath = cPath;
		}
	}
	else
		for (int i = 0; i < (int)adjAreas.size(); i++)
			visitNode(cPath, cLength, lPath, lLength, visitted, adjAreas[i], startPos);

	visitted[cCode] = false;
	assert(cPath[cPath.size() - 1] == cCode);
	cPath.pop_back();
}

int CBiconnectedComponentsOutput::findLengthOfLongestPath(const Pos1D &startPos) const
{
	CFastPos1DDeque lPath;
	int lLength = 0;
	CFastPos1DDeque cPath;
	int cLength = 0;
	bool visittedAreas[MAX_N_AREAS] = { false };
	assert(nAreasOfVertices[startPos]>0);
	if (nAreasOfVertices[startPos] == 1)
		return 0;
	int startArea = nAreas - 1;
	visitNode(cPath, cLength, lPath, lLength, visittedAreas, startArea, startPos);

	if (SHOW_DEBUG_INFORMATION)
	{
		for (int i = 0; i < lPath.size(); i++)
			cout << lPath[i] << " ";
		cout << "\n";
	}
	return lLength;
}

void CBiconnectedComponentsOutput::manager(const Pos1D &playerPos)
{
	{
		nAreas++;
		mark(nAreas - 1, playerPos);
	}
}

void CBiconnectedComponentsOutput::clear()
{
	nAreas = 0;

	memset(aXa, -1, sizeof(aXa[0][0])*MAX_N_AREAS*MAX_N_AREAS);
	memset(vXa, 0, sizeof(vXa[0][0])*BOARD_SIZE*MAX_N_AREAS);
	memset(nVertices, 0, sizeof(nVertices[0])*MAX_N_AREAS);
	memset(nOddVertices, 0, sizeof(nVertices[0])*MAX_N_AREAS);
	memset(nEvenVertices, 0, sizeof(nVertices[0])*MAX_N_AREAS);
	memset(nAreasOfVertices, 0, sizeof(nAreasOfVertices[0]) * BOARD_SIZE);
	memset(iAreaOfVertices, -1, sizeof(iAreaOfVertices[0][0])*BOARD_SIZE * MAX_N_AREAS_PER_BLOCK);
}

void CBiconnectedComponentsOutput::mark(int iArea, int iVertex, bool value /*= true*/)
{
	assert(value == true);
	assert(iArea >= 0 && iArea < nAreas);
	assert(iVertex >= 0 && iVertex < BOARD_SIZE);

	if (vXa[iVertex][iArea] == value)
		return;

	vXa[iVertex][iArea] = value;

	if (value){
		nVertices[iArea]++;
		if (iVertex % 2 == 0)
			nEvenVertices[iArea]++;
		else
			nOddVertices[iArea]++;
		assert(nAreasOfVertices[iVertex] >= 0 && nAreasOfVertices[iVertex] < MAX_N_AREAS_PER_BLOCK);
		iAreaOfVertices[iVertex][nAreasOfVertices[iVertex]] = iArea;
		nAreasOfVertices[iVertex]++;
	}
	else {
		//value == false
		nVertices[iArea]--;
		if (iVertex % 2 == 0)
			nEvenVertices[iArea]--;
		else
			nOddVertices[iArea]--;
		nAreasOfVertices[iVertex]--;
		assert(nEvenVertices[iArea] >= 0);
		assert(nOddVertices[iArea] >= 0);
	}
}

void CBiconnectedComponentsOutput::checkConsitency(bool checkAxA) const
{
	if (nAreas > MAX_N_AREAS)
		return;
	// check nAreas
	for (int i = nAreas; i < MAX_N_AREAS; i++){
		assert(nVertices[i] == 0);
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
		assert(nOdd == nOddVertices[iArea]);
		assert(nEven == nEvenVertices[iArea]);
		assert(nOdd + nEven == nVertices[iArea]);
		assert(nOdd + nEven == nVertices[iArea]);
	}

	// check nAreasOfVertices
	for (int iVertex = 0; iVertex < BOARD_SIZE; iVertex++){
		int nAreasOfPos_ = 0;
		for (int iArea = 0; iArea < MAX_N_AREAS; iArea++){
			if (vXa[iVertex][iArea])
				nAreasOfPos_++;
		}
		assert(nAreasOfPos_ == nAreasOfVertices[iVertex]);
	}

	if (!checkAxA)
		return;
	// check iAreasOfVertices
	for (int iVertex = 0; iVertex < BOARD_SIZE; iVertex++){
		assert(0 <= nAreasOfVertices[iVertex] && nAreasOfVertices[iVertex] <= MAX_N_AREAS_PER_BLOCK);
		for (int i = 0; i < MAX_N_AREAS_PER_BLOCK; i++)
		{
			if (i < nAreasOfVertices[iVertex])
			{
				assert(iAreaOfVertices[iVertex][i] >= 0);
				assert(vXa[iVertex][iAreaOfVertices[iVertex][i]]);
			}
			else {
				assert(iAreaOfVertices[iVertex][i] < 0);
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
			Pos1D v = aXa[i][j];
			assert(vXa[v][i] && vXa[v][j]);
		}
	}
}

void CBiconnectedComponentsOutput::buildAreaXArea(const Pos1D &playerPos)
{
	for (int iVertex = 0; iVertex < BOARD_SIZE; iVertex++){
		assert(nAreasOfVertices[iVertex] >= 0 && nAreasOfVertices[iVertex] <= MAX_N_AREAS_PER_BLOCK);
		if (nAreasOfVertices[iVertex] > 1){
			for (int i = 0; i < nAreasOfVertices[iVertex]; i++){
				for (int j = 0; j < nAreasOfVertices[iVertex]; j++){
					if (i == j) continue;
					aXa[iAreaOfVertices[iVertex][i]][iAreaOfVertices[iVertex][j]] = iVertex;
					aXa[iAreaOfVertices[iVertex][j]][iAreaOfVertices[iVertex][i]] = iVertex;
				}

			}
		}
	}
}

CBiconnectedComponentsOutput::CBiconnectedComponentsOutput()
{
	clear();
}