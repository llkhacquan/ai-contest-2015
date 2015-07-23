#include "BiconnectedComponentsOutput.h"
#include "Pos2D.h"



void CBiconnectedComponentsOutput::visitNode(vector<int> &cPath, int &cLength, vector<int> &lPath, int &lLength, bool *visitted, const int cCode, const int &startPos) const
{
	visitted[cCode] = true;
	cPath.push_back(cCode);

	vector<int> adjAreas;
	for (int iCode = 0; iCode < nAreas; iCode++){
		if (visitted[iCode])
			continue;
		if (aXa[cCode][iCode][0] >= 0) // != -1
			adjAreas.push_back(iCode);
	}

	if (adjAreas.size() == 0){
		cLength = calculateLengthOfPath(cPath, startPos);
		if (cLength > lLength){
			lLength = cLength;
			lPath = vector<int>(cPath);
		}
	}
	else
		for (int i = 0; i < (int)adjAreas.size(); i++)
			visitNode(cPath, cLength, lPath, lLength, visitted, adjAreas[i], startPos);

	visitted[cCode] = false;
	assert(cPath.back() == cCode);
	cPath.pop_back();
}

int CBiconnectedComponentsOutput::findLengthOfLongestPath(const Pos1D &startPos) const
{
	vector<int> lPath;
	int lLength = 0;
	vector<int> cPath;
	int cLength = 0;
	bool visittedAreas[MAX_N_AREAS] = {false};
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

void CBiconnectedComponentsOutput::manager(const Pos1D &playerPos)
{
	{
		// take care the area contains the playerPos
		for (int iArea = 0; iArea < nAreas; iArea++)
			mark(iArea, playerPos, false);
		nAreas++;
		mark(nAreas - 1, playerPos, true);
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
	assert(checkConsitency());
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
	memset(iAreaOfVertices, -1, sizeof(iAreaOfVertices[0])*MAX_N_AREAS);
}

void CBiconnectedComponentsOutput::mark(int iArea, int iVertex, bool value /*= true*/)
{
	assert(iArea >= 0 && iArea < nAreas);
	assert(iVertex >= 0 && iVertex < BOARD_SIZE);
	if (vXa[iVertex][iArea] == value)
		return;
	vXa[iVertex][iArea] = value;
	if (value == true){
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
	}
}

CBiconnectedComponentsOutput::CBiconnectedComponentsOutput()
{
	clear();
}

bool CBiconnectedComponentsOutput::checkConsitency() const
{
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
		if (nOdd + nEven != nVertices[iArea])
			return false;
	}

	// check nAreasOfVertices
	for (int iVertex = 0; iVertex < BOARD_SIZE; iVertex++){
		int nAreasOfPos_ = 0;
		for (int iArea = 0; iArea < MAX_N_AREAS; iArea++){
			if (vXa[iVertex][iArea])
				nAreasOfPos_++;
		}
		if (nAreasOfPos_ != nAreasOfVertices[iVertex])
			return false;
	}

	// check iAreasOfVertices
	for (int iVertex = 0; iVertex < BOARD_SIZE; iVertex++){
		if (nAreasOfVertices[iVertex] == 0)
			assert(iAreaOfVertices[iVertex] == -1);
		else
			assert(vXa[iVertex][iAreaOfVertices[iVertex]]);
	}
	return true;
}

void CBiconnectedComponentsOutput::buildAreaXArea(const Pos1D &playerPos)
{
	bool foundAreas[MAX_N_AREAS] = { false };
	bool visited[BOARD_SIZE] = { false };

	queue<Pos1D> qAreas; // this queue store 1 vertex of each unexplored area
	qAreas.push(playerPos);
	foundAreas[iAreaOfVertices[playerPos]] = true;
	while (!qAreas.empty()){
		Pos1D a = qAreas.front();
		qAreas.pop();

		// start explorer from vertex a
		queue<Pos1D> queueInAArea;
		queueInAArea.push(a);
		while (!queueInAArea.empty()){
			Pos1D v = queueInAArea.front();
			queueInAArea.pop();

			visited[v] = true;

			for (TMove direction = 1; direction <= 4; direction++){
				// check the adjection block that is not in any area
				Pos2D u_ = Pos2D(v).move(direction);
				if (u_.x < 0 || u_.x >= MAP_SIZE)
					continue;
				if (u_.y < 0 || u_.y >= MAP_SIZE)
					continue;
				Pos1D u = u_;
				if (nAreasOfVertices[u] == 0)
					continue;
				if (visited[u])
					continue;

				if (iAreaOfVertices[u] == iAreaOfVertices[v]){// same area
					queueInAArea.push(u);
					visited[u] = true;
				}
				else { // maybe found a new area?
					int code = iAreaOfVertices[u];
					if (!foundAreas[code]){
						foundAreas[code] = true;
						qAreas.push(u);
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
