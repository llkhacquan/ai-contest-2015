#include "BiconnectedComponents.h"
#include "MyAI.h"
#include "Pos.h"

CBiconnectedComponents::CBiconnectedComponents(){}

CBiconnectedComponents::~CBiconnectedComponents(){}

// return the number of components
vector<Area> CBiconnectedComponents::biconnectedComponents(int const board[], const CPos &playerPos, int outBoard[])
{
	// setting up
	CBiconnectedComponents bc;
	memcpy(outBoard, board, BOARD_SIZE*sizeof(int));
	bc.nComponents = 0;
	bc.oBoard = outBoard;
	bc.playerPos = playerPos;
	bc.areas.clear();

	bc.iCount = 0;
	{// clear stack
		stack<Edge> t;
		bc.myStack.swap(t);
	}

	for (int iArea = 0; iArea < BOARD_SIZE; iArea++){
		bc.visited[iArea] = false;
		bc.parrent[iArea] = -1;
	}

	bc.dfsVisit(playerPos.to1D());

		// take care the area contains the playerPos
		for (int iArea = 0; iArea < (int)bc.areas.size(); iArea++)
			bc.areas[iArea].erase(playerPos.to1D());

		bc.areas.push_back(Area());
		bc.areas.back().insert(playerPos.to1D());
		bc.areas.back().code = bc.areas.size() - 1;
		outBoard[playerPos.to1D()] = SPECIAL_BLOCK | ipowBase2(bc.areas.size() - 1);

	// re-build areas so the larger areas will have more and more vertice
	for (Vertex v = 0; v < BOARD_SIZE; v++){
		if (bc.oBoard[v] < SPECIAL_BLOCK) // if v is not special then continue
			continue;
		int block = bc.oBoard[v];
		int iMax = -1;
		for (unsigned int iArea = 0; iArea < bc.areas.size(); iArea++){
			if ((block & ipowBase2(bc.areas[iArea].code)) != 0){
				if (iMax == -1)
					iMax = iArea;
				else if (bc.areas[iMax] > bc.areas[iArea]){
					// create the vertex v in the areas[iArea]
					bc.areas[iArea].erase(v);
					clearBit(bc.oBoard[v], bc.areas[iArea].code);
				}
				else {
					// create the vertex v in the areas[iMax] and set iMax to iArea
					bc.areas[iMax].erase(v);
					clearBit(bc.oBoard[v], bc.areas[iMax].code);
					iMax = iArea;
				}
			}
		}
	}
	assert(bc.areas.size() <= MAXIMUM_NUMBER_OF_AREAS);
	// remove area with 0 position
	sort(bc.areas.begin(), bc.areas.end());
	vector<Area>::iterator it = bc.areas.begin();
	while (bc.areas.size() > 0 && it->nVertices == 0)
		it++;
	bc.areas.erase(bc.areas.begin(), it);

#ifdef _DEBUG	
	// make sure 1 vertex is in only one vertex
	for (Vertex v = 0; v < BOARD_SIZE; v++){
		int block = bc.oBoard[v];
		if (block < SPECIAL_BLOCK)
			continue;
		for (unsigned int iArea = 0; iArea < bc.areas.size(); iArea++){
			if (bc.areas[iArea].code == findCode(block))
				assert(bc.areas[iArea].inTheAreas[v]);
			else
				assert(!bc.areas[iArea].inTheAreas[v]);
		}
		clearBit(block, findCode(block));
		assert(block == SPECIAL_BLOCK);
	}

	// ensure the areas[].nVertices
	for (unsigned int iArea = 0; iArea < bc.areas.size(); iArea++){
		int iCount = 0;
		for (Vertex v = 0; v < BOARD_SIZE; v++){
			if (bc.areas[iArea].inTheAreas[v])
				iCount++;
		}
		assert(iCount == bc.areas[iArea].nVertices);
	}
#endif

	for (unsigned int iArea = 0; iArea < bc.areas.size(); iArea++){
		bc.areas[iArea].code = iArea;
		for (Vertex v = 0; v < BOARD_SIZE; v++){
			if (bc.areas[iArea].inTheAreas[v])
				outBoard[v] = SPECIAL_BLOCK | ipowBase2(iArea);
		}
	}

	return bc.areas;
}

void CBiconnectedComponents::dfsVisit(const Vertex &u){
	assert(u >= 0 && u < BOARD_SIZE);
	visited[u] = true;
	iCount++;
	d[u] = iCount;
	low[u] = d[u];

	bool bAdj[4];
	adjection(bAdj, u);
	for (int i = 0; i < 4; i++){
		if (!bAdj[i])
			continue;
		Vertex v = CPos(u).move(i + 1).to1D();
		if (!visited[v]){
			myStack.push(Edge(u, v));
			parrent[v] = u;
			dfsVisit(v);
			if (low[v] >= d[u])
				createNewArea(u, v);
			low[u] = min(low[u], low[v]);
		}
		else if (!(parrent[u] == v) && (d[v] < d[u])){
			myStack.push(Edge(u, v));
			low[u] = min(low[u], d[v]);
		}
	}
}

void CBiconnectedComponents::createNewArea(const Vertex &u, const Vertex &v){
	assert(u >= 0 && u < BOARD_SIZE);
	assert(v >= 0 && v < BOARD_SIZE);
	// cout << "new biconnected found" << endl;
	Edge e;
	Area a;
	a.code = nComponents;
	do {
		e = myStack.top();
		myStack.pop();
		oBoard[e.u] |= SPECIAL_BLOCK | ipowBase2(nComponents);
		oBoard[e.v] |= SPECIAL_BLOCK | ipowBase2(nComponents);
		a.insert(e.u);
		a.insert(e.v);
	} while (e != Edge(u, v));
	areas.push_back(a);
	nComponents++;
}

void CBiconnectedComponents::adjection(bool out[], Vertex const &u){
	assert(u >= 0 && u < BOARD_SIZE);
	CPos pos(u);
	for (int i = 1; i <= 4; i++){
		static int block;
		block = CMyAI::getBlock(oBoard, pos.move(i));
		if (block == BLOCK_EMPTY || block > SPECIAL_BLOCK)
			out[i - 1] = true;
		else
			out[i - 1] = false;
	}
}

int CBiconnectedComponents::getEstimatedLength(int const board[], const CPos &playerPos)
{
	int oldBlock = board[playerPos.to1D()];
	assert(oldBlock == BLOCK_PLAYER_1 || oldBlock == BLOCK_PLAYER_2);
	static int outBoard[BOARD_SIZE];
	vector<Area> areas = biconnectedComponents(board, playerPos, outBoard);
	set<Edge> edgesOfCode;

	int startArea = findCode(outBoard[playerPos.to1D()]);

	// construct the new graph
	constructNewGraph(playerPos, outBoard, edgesOfCode, areas);

	return findLengthOfLongestPath(outBoard, areas, edgesOfCode, startArea, playerPos.to1D());
}

int CBiconnectedComponents::findLengthOfLongestPath(const int _oBoard[], const vector<Area> &areas,
	const set<Edge> &edgesOfCode, int startArea, const int &startPos)
{
	vector<int> lPath;
	int lLength = 0;
	vector<int> cPath;
	int cLength = 0;
	vector<bool> visittedAreas(areas.size(), false);
	visitNode(_oBoard, areas, edgesOfCode, cPath, cLength, lPath, lLength, visittedAreas, startArea, startPos);

#if SHOW_DEBUG_INFORMATION
	for (int i = 0; i < (int)lPath.size(); i++){
		cout << lPath[i] << "\t";
	}
	cout << "estimated length = " << lLength << endl;
#endif
	return lLength;
}

int CBiconnectedComponents::calculateLengthOfPath(const int _oBoard[], const vector<Area> &areas,
	const set<Edge> &edgesOfCode, const vector<int> &path, const int &startPos)
{
	assert(path.size() > 0);
	assert(areas[path[0]].nVertices == 1);

	int result = 1; // for the first area (areas[path[0]])

	// for the last area (areas[path.back()])
	int pathBach = path.back();
	if (path.size() >= 2) {
		int delta = -1;
		int even = 0, odd = 0;
		for (Vertex j = 0; j < BOARD_SIZE; j++){
			if (!areas[pathBach].inTheAreas[j])
				continue;
			if (j % 2 == 1)
				odd++;
			else
				even++;
		}
		if (odd == even)
			delta = odd * 2;
		else
		{
			bool seen = false;
			for (set<AdjArea>::iterator it = areas[pathBach].adjAreas.begin(); it != areas[pathBach].adjAreas.end(); it++){
				if (it->codeOfAdjArea == path[path.size() - 2]){
					seen = true;
					int delta2;
					if (odd > even) // always end with odd
						if (it->connections % 2 == 1) //start with odd
							delta2 = even * 2 + 1;
						else delta2 = even * 2; // start with even
					else // odd < even, end with even
						if (it->connections % 2 == 1) //start with odd
							delta2 = odd * 2;
						else
							delta2 = odd * 2 + 1; // start with even, end with odd
					if (delta2 > delta)
						delta = delta2;
				}
				else if (seen)
					break;
			}
		}
		assert(delta >= 0);
		result += delta;
	}

	// for the other areas (path[1] -> path[size - 2]
	if (path.size() >= 3)
		for (int i = 1; i < (int)path.size() - 1; i++){
			Area const *area = &(areas[path[i]]);
			int even = 0, odd = 0;
			for (Vertex j = 0; j < BOARD_SIZE; j++){
				if (!area->inTheAreas[j])
					continue;
				if (j % 2 == 1)
					odd++;
				else
					even++;
			}

			// calculate it1, it2, it3, it4
			set<AdjArea>::iterator i1, i2; int n1, n2;
			i1 = i2 = area->adjAreas.end(); n1 = n2 = 0;
			for (set<AdjArea>::iterator it = area->adjAreas.begin(); it != area->adjAreas.end(); it++){
				if (it->codeOfAdjArea == path[i - 1]){
					if (i1 == area->adjAreas.end()){
						i1 = it;
						n1 = 1;
					}
					else
						n1++;
				}
				else if (it->codeOfAdjArea == path[i + 1]){
					if (i2 == area->adjAreas.end()){
						i2 = it;
						n2 = 1;
					}
					else
						n2++;
				}
			}
			assert(i1 != area->adjAreas.end());
			assert(i2 != area->adjAreas.end());
			assert(n1 > 0 && n2 > 0);

			int a = -1;
			set<AdjArea>::iterator i2_ = i2; // backup 
			for (int i = 0; i < n1; i++, i1++){
				i2 = i2_;
				for (int j = 0; j < n2; j++, i2++){
					int b = -1; // find the area with start and end positions is it1+i, it3+j
					if (i1->connections == i2->connections) // start and end in the same position
						b = 1;
					else if (i1->connections % 2 != i2->connections % 2) // start and end in odd/even position
						b = min(odd, even) * 2;
					else if (i1->connections % 2 == 1) // start with odd, end with odd
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

						if (a < b)
							a = b;
				}
			}

			assert(a > 0);
			result += a;
		}
	return result;
}

void CBiconnectedComponents::visitNode(const int _oBoard[], const vector<Area> &areas,
	const set<Edge> &edgesOfCode,
	vector<int> &cPath, int &cLength, vector<int> &lPath,
	int &lLength, vector<bool> &visitted, const int cCode, const int &startPos){
	visitted[cCode] = true;
	cPath.push_back(cCode);

	vector<int> adj;
	for (int iCode = 0; iCode < (int)areas.size(); iCode++){
		if (visitted[iCode])
			continue;
		if (edgesOfCode.find(Edge(cCode, iCode)) != edgesOfCode.end())
			adj.push_back(iCode);
	}

	if (adj.size() == 0){
		cLength = CBiconnectedComponents::calculateLengthOfPath(_oBoard, areas, edgesOfCode, cPath, startPos);
		if (cLength > lLength){
			lLength = cLength;
			lPath = vector<int>(cPath);
		}
	}
	else
		for (int i = 0; i < (int)adj.size(); i++)
			visitNode(_oBoard, areas, edgesOfCode, cPath, cLength, lPath, lLength, visitted, adj[i], startPos);
	visitted[cCode] = false;
	assert(cPath.back() == cCode);
	cPath.pop_back();
}

int CBiconnectedComponents::rateBoardForAPlayer(int const board[], const CPos &playerPos)
{
	int oldBlock = board[playerPos.to1D()];
	assert(oldBlock == BLOCK_PLAYER_1 || oldBlock == BLOCK_PLAYER_2);
	static int outBoard[BOARD_SIZE];
	static bool visited[BOARD_SIZE];
	for (int i = 0; i < BOARD_SIZE; i++){
		visited[i] = false;
	}
	vector<Area> areas = biconnectedComponents(board, playerPos, outBoard);
	set<Edge> edgesOfCode;

	return 0;
}

void CBiconnectedComponents::constructNewGraph(const CPos &playerPos, int * outBoard, set<Edge> &edgesOfCode, vector<Area> &areas)
{
	vector<bool> foundAreas(30, false);
	static bool visited[BOARD_SIZE];
	memset(visited, 0, BOARD_SIZE);
	queue<Vertex> queueOfAreas;
	queueOfAreas.push(playerPos.to1D());
	while (!queueOfAreas.empty()){
		Vertex a = queueOfAreas.front();
		queueOfAreas.pop();
		foundAreas[findCode(outBoard[a])] = true;

		queue<Vertex> queueInAArea;
		queueInAArea.push(a);
		while (!queueInAArea.empty()){
			Vertex v = queueInAArea.front();
			queueInAArea.pop();

			for (Direction direction = 1; direction <= 4; direction++){
				// check the specialty of the block
				int block = CMyAI::getBlock(outBoard, CPos(v).move(direction));
				if (block < SPECIAL_BLOCK || block == BLOCK_OUT_OF_BOARD)
					continue;
				Vertex u = CPos(v).move(direction).to1D();
				if (visited[u])
					continue;

				if (outBoard[u] == outBoard[v]){// same area
					queueInAArea.push(u);
					visited[u] = true;
				}
				else { // maybe found a new area?
					int code = findCode(block);
					if (!foundAreas[code]){
						foundAreas[code] = true;
						queueOfAreas.push(u);
					}
					int block1 = outBoard[u];
					int block2 = outBoard[v];
					int code1 = findCode(block1);
					int code2 = findCode(block2);
					edgesOfCode.insert(Edge(code1, code2));
					AdjArea adjArea;
					adjArea.codeOfAdjArea = code2;
					adjArea.connections = u;
					areas[code1].adjAreas.insert(adjArea);

					adjArea.codeOfAdjArea = code1;
					adjArea.connections = v;
					areas[code2].adjAreas.insert(adjArea);
				}
			}
		}
	}
}
