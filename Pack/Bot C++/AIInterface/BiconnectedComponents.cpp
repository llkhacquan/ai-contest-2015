#include "BiconnectedComponents.h"
#include "..\AIInterface\Pos2D.h" 
#include "../AIInterface/StaticFunctions.h"

CBiconnectedComponents::CBiconnectedComponents(){}

CBiconnectedComponents::~CBiconnectedComponents(){}

// return the number of components
void CBiconnectedComponents::biconnectedComponents(TBlock const board[], CBiconnectedComponentsOutput *output,
	const Pos2D &playerPos, const Pos2D &endPos, TBlock *oBoard)
{
	assert(getBlock(board, playerPos) == BLOCK_PLAYER_1 || getBlock(board, playerPos) == BLOCK_PLAYER_2);
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
		if (getBlock(bc.oBoard, playerPos.move(i)) == BLOCK_EMPTY && !bc.visited[playerPos.move(i)])
			bc.dfsVisit(playerPos.move(i));
	}
	output->manager(playerPos, endPos);

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
			myStack.push_back(u); myStack.push_back(v);
			parrent[v] = u;
			dfsVisit(v);
			if (low[v] >= d[u])
				createNewArea(u, v);
			low[u] = min(low[u], low[v]);
		}
		else if (!(parrent[u] == v) && (d[v] < d[u])){
			myStack.push_back(u); myStack.push_back(v);
			low[u] = min(low[u], d[v]);
		}
	}
}

void CBiconnectedComponents::createNewArea(const Pos1D &_u, const Pos1D &_v){
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

void CBiconnectedComponents::adjection(bool out[], Pos1D const &u){
	assert(u >= 0 && u < BOARD_SIZE);
	Pos2D pos(u);
	for (int i = 1; i <= 4; i++){
		TBlock block;
		block = getBlock(oBoard, pos.move(i));
		if (block == BLOCK_EMPTY || block >= SPECIAL_BLOCK)
			out[i - 1] = true;
		else
			out[i - 1] = false;
	}
}

int CBiconnectedComponents::getEstimatedLength(TBlock const board[], const Pos2D &playerPos, const Pos2D &endPos, const int depth)
{
	TBlock oldBlock = board[playerPos.to1D()];
	assert(oldBlock == BLOCK_PLAYER_1 || oldBlock == BLOCK_PLAYER_2);
	CBiconnectedComponentsOutput out;
	out.depth = depth;
	biconnectedComponents(board, &out, playerPos, endPos);
	return out.findLengthOfLongestPath(playerPos, endPos);
}

