#include "BiconnectedComponentsOutput.h"
#include "StaticFunctions.h"
#include "FastPos1DDeque.h"


int CBiconnectedComponentsOutput::estimateLengthOfPath(const CFastPos1DDeque &path, const int &startPos) const
{
	assert(path.size() > 0);
	assert(nVertices[path[0]] == 1);

	int result = 0; // skip the first area (areas[path[0]])

	// for the last area)
	int pathBack = path[path.size() - 1];
	if (path.size() >= 2) {
		int delta = -1;
		int odd = nOddVertices[pathBack];
		int even = nEvenVertices[pathBack];
		if (odd == even)
			delta = odd * 2;
		else
		{
			assert(aXa[pathBack][path[path.size() - 2]][0] >= 0);
			for (auto i = 0; i < 4; i++){
				Pos1D connection = aXa[pathBack][path[path.size() - 2]][i];
				if (connection < 0)
					continue;

				int delta2;
				if (odd > even) // always end with odd
					if (connection % 2 == 1) //start with odd
						delta2 = even * 2 + 1;
					else delta2 = even * 2; // start with even
				else // odd < even, end with even
					if (connection % 2 == 1) //start with odd
						delta2 = odd * 2;
					else
						delta2 = odd * 2 + 1; // start with even, end with odd
				if (delta2 > delta)
					delta = delta2;
			}
		}
		assert(delta >= 0);
		result += delta;
	}

	// for the other areas (path[1] -> path[size - 2]
	if (path.size() >= 3)
		for (int iArea = 1; iArea < (int)path.size() - 1; iArea++){
			//Area const *area = &(areas[path[i]]);
			int even = nEvenVertices[path[iArea]];
			int odd = nOddVertices[path[iArea]];

			int a = -1;
			for (auto i = 0; i < 4; i++){
				for (auto j = 0; j < 4; j++){
					int b = -1;
					Pos1D start = aXa[path[iArea]][path[iArea - 1]][i];
					if (start < 0)
						break;
					assert(aXa[path[iArea - 1]][path[iArea]][i] >= 0);
					Pos1D end = aXa[path[iArea]][path[iArea + 1]][j];;
					if (end < 0)
						break;
					assert(aXa[path[iArea + 1]][path[iArea]][j] >= 0);
					{
						if (start == end) // start and end in the same position
							b = 1;
						else if (start % 2 != end % 2) // start and end in odd/even position
							b = min(odd, even) * 2;
						else if (start % 2 == 1) // start with odd, end with odd
							if (odd > even)
								b = even * 2 + 1;
							else if (even > odd)
								b = odd * 2 - 1;
							else // even == odd
								b = odd * 2 - 1;
						else // if (it1->connections % 2 == 0) // start with even, end with even
							if (odd > even)
								b = even * 2 - 1;
							else if (odd < even)
								b = odd * 2 + 1;
							else // even == odd
								b = even * 2 - 1;
					}
					if (a < b)
						a = b;
				}
			}
			assert(a > 0);
			result += a;
		}
	return result;
}

int CBiconnectedComponentsOutput::calculateLengthBetween2NodeIn1Area(const Pos1D &u, const Pos1D &v, int depth){
	// make sure u and v are in the same area
	assert(iAreaOfVertices[u] == iAreaOfVertices[v]);
	int cArea = iAreaOfVertices[u];

	if (depth == 0){ // we have to estimate
		if (v < -1){
			int delta = -1;
			int odd = nOddVertices[cArea];
			int even = nEvenVertices[cArea];
			if (odd == even)
				delta = odd * 2;
			else
			{
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
		}
	}

	// create a map with only block of cArea
	TBlock tBoard[BOARD_SIZE];
	for (int iVertex = 0; iVertex < BOARD_SIZE; iVertex++){
		if (iAreaOfVertices[iVertex] == cArea)
			tBoard[iVertex] = BLOCK_EMPTY;
		else
			tBoard[iVertex] = BLOCK_OBSTACLE;
	}

	assert(!isIsolated(tBoard, Pos1D(u), Pos1D(v)));
	tBoard[u] = BLOCK_OBSTACLE;
	tBoard[v] = BLOCK_OBSTACLE;

	return 0;
}

void CBiconnectedComponentsOutput::visitNode(CFastPos1DDeque &cPath, int &cLength, CFastPos1DDeque &lPath, int &lLength,
	bool *visitted, const int cCode, const int &startPos, const int &endPos) const
{
	visitted[cCode] = true;
	cPath.push_back(cCode);

	CFastPos1DDeque adjAreas;
	for (int iCode = 0; iCode < nAreas; iCode++){
		if (visitted[iCode])
			continue;
		if (aXa[cCode][iCode][0] >= 0) // != -1
			adjAreas.push_back(iCode);
	}

	if (adjAreas.size() == 0){
		if (endPos < 0 || cPath[cPath.size() - 1] == iAreaOfVertices[endPos])
		{
			cLength = estimateLengthOfPath(cPath, startPos);
			if (cLength > lLength){
				lLength = cLength;
				lPath = cPath;
			}
		}
	}
	else
		for (int i = 0; i < (int)adjAreas.size(); i++)
			visitNode(cPath, cLength, lPath, lLength, visitted, adjAreas[i], startPos, endPos);

	visitted[cCode] = false;
	assert(cPath[cPath.size() - 1] == cCode);
	cPath.pop_back();
}

int CBiconnectedComponentsOutput::findLengthOfLongestPath(const Pos1D &startPos, const Pos1D &endPos) const
{
	CFastPos1DDeque lPath;
	int lLength = 0;
	CFastPos1DDeque cPath;
	int cLength = 0;
	bool visittedAreas[MAX_N_AREAS] = { false };
	int startArea = iAreaOfVertices[startPos];
	visitNode(cPath, cLength, lPath, lLength, visittedAreas, startArea, startPos);

#if SHOW_DEBUG_INFORMATION
	for (int i = 0; i < (int)lPath.size(); i++){
		cout << lPath[i] << "\t";
	}
	cout << "estimated length = " << lLength << endl;
#endif
	return lLength;
}

void CBiconnectedComponentsOutput::manager(const Pos1D &playerPos, const Pos1D &endPos)
{
	{
		// take care the area contains the playerPos and endPos
		nAreas++;
		mark(nAreas - 1, playerPos, true);
		if (endPos >= 0)
		{
			nAreas++;
			mark(nAreas - 1, endPos, true);
		}
	}
	// re-build areas so the larger areas will have more and more vertice
	for (Pos1D v = 0; v < BOARD_SIZE; v++){
		if (nAreasOfVertices[v] == 0)
			iAreaOfVertices[v] = -1;
		else if (nAreasOfVertices[v] == 1)
		{
			for (int iArea = 0; iArea < nAreas; iArea++){
				if (vXa[v][iArea])
				{
					iAreaOfVertices[v] = iArea;
					break;
				}
			}
			continue;
		}
		else if (nAreasOfVertices[v] >= 3)
		{
			// move the v to a new area
			for (int iArea = 0; iArea < nAreas; iArea++)
				mark(iArea, v, false);
			nAreas++;
			mark(nAreas - 1, v, true);
			iAreaOfVertices[v] = nAreas - 1;
			continue;
		}
		else {
			assert(nAreasOfVertices[v] == 2);

			int a1 = -1, a2 = -2;
			for (int iArea = 0; iArea < nAreas; iArea++){
				if (vXa[v][iArea])
					if (a1 == -1)
						a1 = iArea;
					else
					{
						a2 = iArea;
						break;
					}
			}
			assert(a1 >= 0 && a2>a1);
			if (nVertices[a1] > nVertices[a2]){
				mark(a2, v, false);
				iAreaOfVertices[v] = a1;
			}
			else {
				mark(a1, v, false);
				iAreaOfVertices[v] = a2;
			}
		}
	}
}

void CBiconnectedComponentsOutput::clear()
{
	nAreas = 0;

	memset(aXa, -1, sizeof(aXa[0][0][0])*MAX_N_AREAS*MAX_N_AREAS * 4);
	memset(vXa, 0, sizeof(vXa[0][0])*BOARD_SIZE*MAX_N_AREAS);
	memset(nVertices, 0, sizeof(nVertices[0])*MAX_N_AREAS);
	memset(nOddVertices, 0, sizeof(nVertices[0])*MAX_N_AREAS);
	memset(nEvenVertices, 0, sizeof(nVertices[0])*MAX_N_AREAS);
	memset(nAreasOfVertices, 0, sizeof(nAreasOfVertices[0]) * BOARD_SIZE);
	memset(iAreaOfVertices, -1, sizeof(iAreaOfVertices[0])*BOARD_SIZE);
}

void CBiconnectedComponentsOutput::mark(int iArea, int iVertex, bool value /*= true*/)
{
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

CBiconnectedComponentsOutput::CBiconnectedComponentsOutput()
{
	clear();
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
		if (nAreasOfVertices[iVertex] == 0)
			assert(iAreaOfVertices[iVertex] == -1);
		else
			assert(vXa[iVertex][iAreaOfVertices[iVertex]]);
	}
}

void CBiconnectedComponentsOutput::buildAreaXArea(const Pos1D &playerPos)
{
	bool foundAreas[MAX_N_AREAS] = { false };
	bool visited[BOARD_SIZE] = { false };

	CFastPos1DDeque qAreas; // this queue store 1 vertex of each unexplored area
	qAreas.push_back(playerPos);
	foundAreas[iAreaOfVertices[playerPos]] = true;
	while (!qAreas.empty()){
		Pos1D a = qAreas.pop_front();

		// start explorer from vertex a
		CFastPos1DDeque queueInAArea;
		queueInAArea.push_back(a);
		while (!queueInAArea.empty()){
			Pos1D v = queueInAArea.pop_front();

			visited[v] = true;

			for (TMove direction = 1; direction <= 4; direction++){
				// check the adjection block that is not in any area
				Pos1D u_ = MOVE(v, direction);
				if (u_ < 0 || u_ >= BOARD_SIZE)
					continue;
				Pos1D u = u_;
				if (nAreasOfVertices[u] == 0)
					continue;
				if (visited[u])
					continue;

				if (iAreaOfVertices[u] == iAreaOfVertices[v]){// same area
					queueInAArea.push_back(u);
					visited[u] = true;
				}
				else { // maybe found a new area?
					int code = iAreaOfVertices[u];
					if (!foundAreas[code]){
						foundAreas[code] = true;
						qAreas.push_back(u);
					}
					int code1 = iAreaOfVertices[u];
					int code2 = iAreaOfVertices[v];

					int t = 0;
					insertEdgeOf2Areas(code1, code2, u, v);
				}
			}
		}
	}
}

void CBiconnectedComponentsOutput::insertEdgeOf2Areas(const int a1, const int a2, const int u, const int v)
{
	int i = 0;
	for (i = 0; i < 4; i++){
		if (aXa[a1][a2][i] == -1)
			break;
		if (aXa[a1][a2][i] == u && aXa[a2][a1][i] == v){
			return;
		}
	}
	assert(i < 4);
	assert(aXa[a2][a1][i] == -1);
	aXa[a1][a2][i] = u;
	aXa[a2][a1][i] = v;
}
