#include "BiconnectedComponents.h"
#include "../AIInterface/StaticFunctions.h"

CBiconnectedComponents::CBiconnectedComponents(){}

CBiconnectedComponents::~CBiconnectedComponents(){}

// return the number of components
void CBiconnectedComponents::biconnectedComponents(TBlock const board[], CBiconnectedComponentsOutput *output,
	const Pos1D &playerPos, TBlock *oBoard)
{
	assert(GET_BLOCK(board, playerPos) == BLOCK_PLAYER_1 || GET_BLOCK(board, playerPos) == BLOCK_PLAYER_2);
	// setting up
	CBiconnectedComponents bc;
	output->clear();
	memcpy(bc.oBoard, board, sizeof(TBlock)*BOARD_SIZE);

	setBlock(bc.oBoard, playerPos, BLOCK_EMPTY);

	bc.iCount = 0;
	bc.output = output;

	{// clear stack
		bc.myStack.clear();
	}

	memset(bc.visited, false, BOARD_SIZE*sizeof(bc.visited[0]));

	for (int iVertex = 0; iVertex < BOARD_SIZE; iVertex++){
		bc.parrent[iVertex] = -1;
	}

	// build the areas by biconnected components algorithm 
// 	for (int i = 1; i <= 4; i++){
// 		{
// 			Pos1D newPos = MOVE(playerPos, i);
// 			if (GET_BLOCK(bc.oBoard, newPos) == BLOCK_EMPTY && !bc.visited[newPos])
// 				bc.dfsVisit(newPos);
// 		}
// 	}
	bc.dfsVisit(playerPos);
	output->manager(playerPos);

	if (oBoard != NULL){
		memcpy(oBoard, board, sizeof(TBlock)*BOARD_SIZE);
		for (int iVertex = 0; iVertex < BOARD_SIZE; iVertex++){
			for (int j = 0; j < output->nAreasOfVertices[iVertex]; j++)
				oBoard[iVertex] |= SPECIAL_BLOCK | iPow2(output->iAreaOfVertices[iVertex][j]);
		}
	}
	output->buildAreaXArea(playerPos);
	output->checkConsitency(true);
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

int CBiconnectedComponents::getEstimatedLength(TBlock const board[], const Pos1D &playerPos, bool printInfo)
{
	TBlock oldBlock = board[playerPos];
	assert(oldBlock == BLOCK_PLAYER_1 || oldBlock == BLOCK_PLAYER_2);
	CBiconnectedComponentsOutput out;

	TBlock b[BOARD_SIZE];
	biconnectedComponents(board, &out, playerPos, b);
	int result = out.findLengthOfLongestPath(playerPos);

	return result;
}

