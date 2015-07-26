#include "BiconnectedComponents.h"
#include "..\AIInterface\Pos2D.h" 
#include "../AIInterface/StaticFunctions.h"

CBiconnectedComponents::CBiconnectedComponents(){}

CBiconnectedComponents::~CBiconnectedComponents(){}

// return the number of components
void CBiconnectedComponents::biconnectedComponents(TBlock const board[], const Pos2D &playerPos,
	CBiconnectedComponentsOutput *output, TBlock *oBoard)
{
	assert(getBlock(board, playerPos) == BLOCK_PLAYER_1 || getBlock(board, playerPos) == BLOCK_PLAYER_2);
	// setting up
	CBiconnectedComponents bc;
	output->clear();
	memcpy(bc.oBoard, board, sizeof(TBlock)*BOARD_SIZE);

	setBlock(bc.oBoard, playerPos, BLOCK_OBSTACLE);
	bc.iCount = 0;
	bc.output = output;

	{// clear stack
		stack<Edge> t;
		bc.myStack.swap(t);
	}

	for (int iVertex = 0; iVertex < BOARD_SIZE; iVertex++){
		bc.visited[iVertex] = false;
		bc.parrent[iVertex] = -1;
	}

	// build the areas by biconnected components algorithm 
	for (int i = 1; i <= 4; i++){
		if (getBlock(bc.oBoard, playerPos.move(i)) == BLOCK_EMPTY)
			bc.dfsVisit(playerPos.move(i));
	}
	output->manager(playerPos);

	if (oBoard != NULL){
		memcpy(oBoard, board, sizeof(TBlock)*BOARD_SIZE);
		for (int i = 0; i < BOARD_SIZE; i++){
			if (output->iAreaOfVertices[i] >= 0){
				oBoard[i] = SPECIAL_BLOCK + (output->iAreaOfVertices[i]);
			}
		}
		printBoard(oBoard, true);
	}
	output->buildAreaXArea(playerPos);
}

void CBiconnectedComponents::dfsVisit(const Pos1D &u){
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
		Pos1D v = Pos2D(u).move(i + 1).to1D();
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

void CBiconnectedComponents::createNewArea(const Pos1D &u, const Pos1D &v){
	assert(u >= 0 && u < BOARD_SIZE);
	assert(v >= 0 && v < BOARD_SIZE);
	output->nAreas++;
	Edge e;
	do {
		e = myStack.top();
		myStack.pop();
		// e.u and e.v are in the new area
		oBoard[e.u] |= SPECIAL_BLOCK | ipowBase2(output->nAreas - 1);
		oBoard[e.v] |= SPECIAL_BLOCK | ipowBase2(output->nAreas - 1);
		output->mark(output->nAreas - 1, e.u);
		output->mark(output->nAreas - 1, e.v);
	} while (e != Edge(u, v));
}

void CBiconnectedComponents::adjection(bool out[], Pos1D const &u){
	assert(u >= 0 && u < BOARD_SIZE);
	Pos2D pos(u);
	for (int i = 1; i <= 4; i++){
		static TBlock block;
		block = getBlock(oBoard, pos.move(i));
		if (block == BLOCK_EMPTY || block > SPECIAL_BLOCK)
			out[i - 1] = true;
		else
			out[i - 1] = false;
	}
}

int CBiconnectedComponents::getEstimatedLength(TBlock const board[], const Pos2D &playerPos)
{
	TBlock oldBlock = board[playerPos.to1D()];
	assert(oldBlock == BLOCK_PLAYER_1 || oldBlock == BLOCK_PLAYER_2);
	CBiconnectedComponentsOutput out;
	biconnectedComponents(board, playerPos, &out);
	return out.findLengthOfLongestPath(playerPos);
}

int CBiconnectedComponentsOutput::calculateLengthOfPath(const vector<int> &path, const int &startPos) const
{
	assert(path.size() > 0);
	assert(nVertices[path[0]] == 1);

	int result = 0; // skip the first area (areas[path[0]])

	// for the last area (areas[path.back()])
	int pathBack = path.back();
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
