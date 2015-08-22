#include "BiconnectedComponents.h"
#include "../AIInterface/StaticFunctions.h"

CBC::CBC(){ output = NULL; }

CBC::~CBC(){}

void CBC::calculateBCs(TBlock const board[], CBCO *output, const TPos &playerPos, TBlock *oBoard)
{
	assert(GET_BLOCK(board, playerPos) == BLOCK_PLAYER_1 || GET_BLOCK(board, playerPos) == BLOCK_PLAYER_2);
	// setting up
	CBC bc;
	output->clear();
	output->startPos = playerPos;
	memcpy(bc.oBoard, board, sizeof(TBlock)*BOARD_SIZE);

	bc.oBoard[playerPos] = BLOCK_OBSTACLE;
	bc.iCount = 0;
	bc.output = output;
	bc.myStack.clear();

	memset(bc.visited, false, BOARD_SIZE*sizeof(bool));

	for (int iVertex = 0; iVertex < BOARD_SIZE; iVertex++){
		bc.parrent[iVertex] = -1;
	}

	// build the areas by biconnected components algorithm 
	for (int i = 1; i <= 4; i++){
		TPos newPos = MOVE(playerPos, i);
		if (GET_BLOCK(bc.oBoard, newPos) == BLOCK_EMPTY)
		{
			assert(!bc.visited[newPos]);
			bc.dfsVisit(newPos);
		}
	}

	if (oBoard != NULL){
		memcpy(oBoard, board, sizeof(TBlock)*BOARD_SIZE);
		for (TPos iVertex = 0; iVertex < BOARD_SIZE; iVertex++){
			for (int j = 0; j < output->nAofV[iVertex]; j++)
				oBoard[iVertex] |= SPECIAL_BLOCK | iPow2(output->iAofV[iVertex][j]);
		}
	}
	output->buildAreaXArea();

	if (output->nAreas == 0){
		TMove i;
		for (i = 1; i <= 4; i++)
		{
			TPos newPos = MOVE(playerPos, i);
			if (GET_BLOCK(board, newPos) == BLOCK_EMPTY)
			{
				output->specialResult = 1;
				break;
			}
		}
		if (i == 5)
			output->specialResult = 0;
	}

#ifdef _DEBUG
	output->checkConsitency(true);
#endif // _DEBUG
}

void CBC::calculateBCs(TBlock const board[], CBCO *output, const TPos &playerPos)
{
	calculateBCs(board, output, playerPos, NULL);
}

void CBC::dfsVisit(const TPos &u){
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
		TPos v = MOVE(u, i + 1);
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

void CBC::createNewArea(const TPos &_u, const TPos &_v){
	assert(_u >= 0 && _u < BOARD_SIZE);
	assert(_v >= 0 && _v < BOARD_SIZE);
	output->nAreas++;
	TPos u, v;
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

void CBC::adjection(bool out[], TPos const &u) const {
	assert(u >= 0 && u < BOARD_SIZE);
	TPos pos = u;
	for (TMove m = 1; m <= 4; m++){
		TBlock block = GET_BLOCK(oBoard, MOVE(pos, m));
		if (block == BLOCK_EMPTY || block >= SPECIAL_BLOCK)
			out[m - 1] = true;
		else
			out[m - 1] = false;
	}
}

int CBC::calculateLength(TBlock const board[], const TPos &playerPos, const bool printInfo, const EXACT_LEVEL exact)
{
	TBlock oldBlock = board[playerPos];
	assert(oldBlock == BLOCK_PLAYER_1 || oldBlock == BLOCK_PLAYER_2);
	CBCO out;

	TBlock b[BOARD_SIZE];
	calculateBCs(board, &out, playerPos, b);

	if (printInfo && SHOW_DEBUG_INFORMATION) {
		for (int y = 0; y < MAP_SIZE; y++) {
			for (int x = 0; x < MAP_SIZE; x++){
				TBlock block = b[CC(x, y)];
				if (block >= SPECIAL_BLOCK)
				{
					cout << setw(2) << (int)out.iAofV[CC(x, y)][0] << ".";
					cout << setw(2) << (int)out.iAofV[CC(x, y)][1] << "|";
				}
				else if (block >= 0)
					cout << setfill(' ') << setw(5) << "-----" << "|";
				else
					cout << setfill(' ') << setw(5) << block << "|";
			}
			cout << endl;
		}
		cout << endl;
	}
	int result = out.findLengthOfLongestPath(exact);
	return result;
}

