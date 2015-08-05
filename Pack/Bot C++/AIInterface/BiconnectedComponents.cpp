#include "BiconnectedComponents.h"
#include "../AIInterface/StaticFunctions.h"

CBiconnectedComponents::CBiconnectedComponents(){}

CBiconnectedComponents::~CBiconnectedComponents(){}

// return the number of components
void CBiconnectedComponents::biconnectedComponents(TBlock const board[], CBiconnectedComponentsOutput *output,
	const Pos1D &playerPos, const Pos1D &endPos, TBlock *oBoard)
{
	assert(GET_BLOCK(board, playerPos) == BLOCK_PLAYER_1 || GET_BLOCK(board, playerPos) == BLOCK_PLAYER_2);
	// setting up
	CBiconnectedComponents bc;
	output->clear();
	memcpy(bc.oBoard, board, sizeof(TBlock)*BOARD_SIZE);

	setBlock(bc.oBoard, playerPos, BLOCK_OBSTACLE);
	setBlock(bc.oBoard, endPos, BLOCK_OBSTACLE);

	bc.iCount = 0;
	bc.output = output;

	{// clear stack
		bc.myStack.clear();
	}

	for (int iVertex = 0; iVertex < BOARD_SIZE; iVertex++){
		bc.visited[iVertex] = false;
		bc.parrent[iVertex] = -1;
	}

	// build the areas by biconnected components algorithm 
	for (int i = 1; i <= 4; i++){
		{
			Pos1D newPos = MOVE(playerPos, i);
			if (GET_BLOCK(bc.oBoard, newPos) == BLOCK_EMPTY && !bc.visited[newPos])
				bc.dfsVisit(newPos);
		}
	}
	output->manager(playerPos, endPos);

	if (oBoard != NULL){
		memcpy(oBoard, board, sizeof(TBlock)*BOARD_SIZE);
		for (int i = 0; i < BOARD_SIZE; i++){
			if (output->iAreaOfVertices[i] >= 0){
				oBoard[i] = SPECIAL_BLOCK + (output->iAreaOfVertices[i]);
			}
		}
	}
	output->buildAreaXArea(playerPos);
}

__forceinline void CBiconnectedComponents::dfsVisit(const Pos1D &u){
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
		Pos1D v = MOVE(u, i + 1);
		if (!visited[v]){
			myStack.push_back(u); myStack.push_back(v);
			parrent[v] = u;
			dfsVisit(v);
			if (low[v] >= d[u])
				createNewArea(u, v);
			if (low[u] > low[v]) low[u] = low[v];
		}
		else if (!(parrent[u] == v) && (d[v] < d[u])){
			myStack.push_back(u); myStack.push_back(v);
			if (low[u] > d[v]) low[u] = d[v];
		}
	}
}

__forceinline void CBiconnectedComponents::createNewArea(const Pos1D &_u, const Pos1D &_v){
	assert(_u >= 0 && _u < BOARD_SIZE);
	assert(_v >= 0 && _v < BOARD_SIZE);
	output->nAreas++;
	Pos1D u, v;
	do {
		assert(myStack.size() % 2 == 0);
		u = myStack.pop_back();
		v = myStack.pop_back();
		oBoard[u] |= SPECIAL_BLOCK;
		oBoard[v] |= SPECIAL_BLOCK;
		output->mark(output->nAreas - 1, u);
		output->mark(output->nAreas - 1, v);
	} while ((u != _u || v != _v) && (v != _u || u != _v));
}

__forceinline void CBiconnectedComponents::adjection(bool out[], Pos1D const &u){
	assert(u >= 0 && u < BOARD_SIZE);
	Pos1D pos(u);
	for (int i = 1; i <= 4; i++){
		TBlock block;
		block = GET_BLOCK(oBoard, MOVE(pos, i));
		if (block == BLOCK_EMPTY || block >= SPECIAL_BLOCK)
			out[i - 1] = true;
		else
			out[i - 1] = false;
	}
}

int CBiconnectedComponents::getEstimatedLength(TBlock const board[], const Pos1D &playerPos, const Pos1D &endPos, const int depth, bool printInfo)
{
	TBlock oldBlock = board[playerPos];
	assert(oldBlock == BLOCK_PLAYER_1 || oldBlock == BLOCK_PLAYER_2);
	CBiconnectedComponentsOutput out;
	out.depth = depth;

	TBlock b[BOARD_SIZE];
	biconnectedComponents(board, &out, playerPos, endPos, b);
	// int result = out.findLengthOfLongestPath(playerPos, endPos);

	CFastPos1DDeque lPath;
	int lLength = 0;
	CFastPos1DDeque cPath;
	int cLength = 0;
	bool visittedAreas[MAX_N_AREAS] = { false };
	int startArea = out.iAreaOfVertices[playerPos];
	if (SHOW_DEBUG_INFORMATION && printInfo)
		printBoard(b, true);
	out.visitNode(cPath, cLength, lPath, lLength, visittedAreas, startArea, playerPos);

	if (SHOW_DEBUG_INFORMATION && printInfo)
	{
		for (int i = 0; i < lPath.size(); i++)
			cout << lPath[i] << " ";
		cout << "\n";
	}

	return lLength;
}

