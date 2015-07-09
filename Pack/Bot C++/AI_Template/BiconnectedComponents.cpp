#include "BiconnectedComponents.h"
#include "MyAI.h"
#include "Pos.h"

CBiconnectedComponents::CBiconnectedComponents(){}

CBiconnectedComponents::~CBiconnectedComponents(){}

void erase_v4(std::vector<int> &vec, int value)
{
	// get the range in 2*log2(N), N=vec.size()
	auto bounds = std::equal_range(vec.begin(), vec.end(), value);

	// calculate the index of the first to be deleted O(1)
	auto last = vec.end() - std::distance(bounds.first, bounds.second);

	// swap the 2 ranges O(equals) , equal = std::distance(bounds.first, bounds.last)
	std::swap_ranges(bounds.first, bounds.second, last);

	// erase the victims O(equals)
	vec.erase(last, vec.end());
}

int findCode(int block){
	int t = block - SPECIAL_BLOCK;
	int code = 0;
	while (((t & 1) == 0) && (t > 0)){
		t >>= 1;
		code++;
	}
	return code;
}

int ipowBase2(int exp)
{
	int base = 2;
	int result = 1;
	while (exp)
	{
		if (exp & 1)
			result *= base;
		exp >>= 1;
		base *= base;
	}
	return result;
}

// return the number of components
vector<Area> CBiconnectedComponents::biconnectedComponents(int const board[], const CPos &playerPos, int outBoard[])
{
	// setting up
	memcpy(outBoard, board, BOARD_SIZE*sizeof(int));
	nComponents = 0;
	oBoard = outBoard;
	this->playerPos = playerPos;
	areas.clear();

	iCount = 0;
	{// clear stack
		stack<Edge> t;
		myStack.swap(t);
	}

	for (int iArea = 0; iArea < BOARD_SIZE; iArea++){
		visited[iArea] = false;
		parrent[iArea] = -1;
	}

	dfsVisit(playerPos.to1D());

	// take care the area contains the playerPos
	for (int iArea = 0; iArea < (int)areas.size(); iArea++)
		areas[iArea].vertices.erase(playerPos.to1D());

	areas.push_back(Area());
	areas.back().vertices.insert(playerPos.to1D());
	areas.back().code = areas.size() - 1;
	outBoard[playerPos.to1D()] = SPECIAL_BLOCK + ipowBase2(areas.size() - 1);

	// re-build areas so we remove the 
	sort(areas.begin(), areas.end());
	bool changed = false;
	for (int iArea = 0; iArea < (int)areas.size(); iArea++){
		for (set<Vertex>::iterator iVertex = areas[iArea].vertices.begin(); iVertex != areas[iArea].vertices.end();){
			Vertex v = *iVertex;
			for (int iArea2 = areas.size() - 1; iArea2 > iArea; iArea2--){
				if (binary_search(areas[iArea2].vertices.begin(), areas[iArea2].vertices.end(), v)){
					// insert v to the areas[iArea2].vertices
					areas[iArea].vertices.erase(v);
					changed = true;
					// skip the iArea2 for loop
					break;
				}

			}
			if (changed){
				sort(areas.begin(), areas.end());
				iArea = -1;
				changed = false;
				break;
			}
			else
				iVertex++;
		}
	}

	sort(areas.begin(), areas.end());
	// remove area with 0 position
	while (areas.size() > 0 && areas[0].vertices.size() == 0)
		areas.erase(areas.begin());

	for (int iArea = 0; iArea < (int)areas.size(); iArea++){
		areas[iArea].code = iArea;
		for (set<Vertex>::iterator iVertex = areas[iArea].vertices.begin(); iVertex != areas[iArea].vertices.end(); iVertex++){
			outBoard[*iVertex] = SPECIAL_BLOCK + ipowBase2(iArea);
		}
	}

	return areas;
}

void CBiconnectedComponents::dfsVisit(const Vertex &u){
	assert(u >= 0 && u < BOARD_SIZE);
	visited[u] = true;
	iCount++;
	d[u] = iCount;
	low[u] = d[u];

	vector<Vertex> adj = adjection(u);
	for (int i = 0; i < (int)adj.size(); i++){
		Vertex v = adj[i];
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
	areas.push_back(Area());
	areas.back().code = nComponents;
	do {
		e = myStack.top();
		myStack.pop();
		// printf("(%d %d - %d %d)\t", e.u%MAP_SIZE, e.u / MAP_SIZE, e.v%MAP_SIZE, e.v / MAP_SIZE);
		areas.back().vertices.insert(e.u);
		areas.back().vertices.insert(e.v);
	} while (e != Edge(u, v));
	// cout << endl;
	nComponents++;
}

vector<Vertex> CBiconnectedComponents::adjection(Vertex const &u){
	assert(u >= 0 && u < BOARD_SIZE);
	vector<Vertex> result;
	result.reserve(4);
	CPos pos(u);
	for (int i = 1; i <= 4; i++){
		static int block;
		block = CMyAI::getBlock(oBoard, pos.move(i));
		if (block == BLOCK_EMPTY || block > SPECIAL_BLOCK)
			result.push_back(pos.move(i).to1D());
	}
	return result;
}

int CBiconnectedComponents::getEstimatedLength(int const board[], const CPos &playerPos)
{
	CBiconnectedComponents bc;
	int oldBlock = board[playerPos.to1D()];
	assert(oldBlock == BLOCK_PLAYER_1 || oldBlock == BLOCK_PLAYER_2);
	static int outBoard[BOARD_SIZE];
	static bool visited[BOARD_SIZE];
	for (int i = 0; i < BOARD_SIZE; i++){
		visited[i] = false;
	}
	vector<Area> areas = bc.biconnectedComponents(board, playerPos, outBoard);
	set<Edge> edgesOfCode;

	int startArea = findCode(outBoard[playerPos.to1D()]);

	vector<bool> foundAreas(30, false);

	// construct the new graph
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
	CMyAI::printBoard(_oBoard, true);

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
	assert(path.size()>0);
	assert(areas[path[0]].vertices.size() == 1);
	assert(*areas[path[0]].vertices.begin() == startPos);

	int result = 1; // for the first area (areas[path[0]])

	// for the last area (areas[path.back()])
	if (path.size() >= 2) {
		int delta = -1;
		int even = 0, odd = 0;
		for (set<Vertex>::iterator j = areas[path.back()].vertices.begin(); j != areas[path.back()].vertices.end(); j++){
			if ((*j) % 2 == 1)
				odd++;
			else
				even++;
		}
		if (odd == even)
			delta = odd * 2;
		else
		{
			bool seen = false;
			for (set<AdjArea>::iterator it = areas[path.back()].adjAreas.begin(); it != areas[path.back()].adjAreas.end(); it++){
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
		result += delta;
	}

	// for the other areas (path[1] -> path[size - 2]
	if (path.size() >= 3)
		for (int i = 1; i < (int)path.size() - 1; i++){
			Area area = areas[path[i]];
			int even = 0, odd = 0;
			for (set<Vertex>::iterator j = area.vertices.begin(); j != area.vertices.end(); j++){
				if ((*j) % 2 == 1)
					odd++;
				else
					even++;
			}

			// calculate it1, it2, it3, it4
			set<AdjArea>::iterator i1, i2; int n1, n2;
			i1 = i2 = area.adjAreas.end(); n1 = n2 = 0;
			for (set<AdjArea>::iterator it = area.adjAreas.begin(); it != area.adjAreas.end(); it++){
				if (it->codeOfAdjArea == path[i - 1]){
					if (i1 == area.adjAreas.end()){
						i1 = it;
						n1 = 1;
					}
					else
						n1++;
				}
				else if (it->codeOfAdjArea == path[i + 1]){
					if (i2 == area.adjAreas.end()){
						i2 = it;
						n2 = 1;
					}
					else
						n2++;
				}
			}
			assert(i1 != area.adjAreas.end());
			assert(i2 != area.adjAreas.end());
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
		if (!binary_search(edgesOfCode.begin(), edgesOfCode.end(), Edge(cCode, iCode)))
			continue;
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