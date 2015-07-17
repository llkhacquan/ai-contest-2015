#include "MyAI.h"
#include "mydefine.h"
#include "BiconnectedComponents.h"


Direction CMyAI::getNextMoveOfLongestPath(const int boardData[], const CPos &pos, const int depth){
	static int board[BOARD_SIZE];
	memcpy(board, boardData, BOARD_SIZE*sizeof(int));
	int maxLength = -1;
	int iMax = 0;

	vector<Direction> avalableMoves = CMyAI::getAvailableMoves(boardData, pos);
	if (avalableMoves.size() == 1)
		return avalableMoves[0];

	for (unsigned int iMove = 0; iMove < avalableMoves.size(); iMove++){
		Direction i = avalableMoves[iMove];
		bool bOk = move(board, pos, i); assert(bOk);
		int length = CBiconnectedComponents::getEstimatedLength(board, pos.move(i));
		if (length > maxLength){
			iMax = i;
			maxLength = length;
		}
		bOk = move(board, pos.move(i), getOpositeDirection(i), true);
		assert(bOk);
	}
	cout << "estimated length = " << maxLength << endl;
	if (maxLength < 15)
		return getALongestPath(boardData, pos)[0];
	else
		return iMax;
}

vector<int> CMyAI::getALongestPath(int const boardData[], const CPos &pos)
{
	assert(pos.x >= 0 && pos.x < MAP_SIZE && pos.y >= 0 && pos.y < MAP_SIZE);
	static int board[121];
	memcpy(board, boardData, BOARD_SIZE*sizeof(int));

	CPos p = pos;

	static vector<int> c, l; // store path of one of the longest paths
	c.clear();
	l.clear();
	findLongestPath(board, p, c, l);
	return l;
}

vector<Direction> CMyAI::findShortestPath(const int _board[], const CPos& start, CPos& end)
{
	int board[BOARD_SIZE];
	memcpy(board, _board, BOARD_SIZE*sizeof(int));
	vector<Direction> path;

	// we move from end point to every point
	fillDistance(board, end);

	// we scan the 4 blocks near the start point, find the block with minimum distance from end point (if there is no such point, start and end are seperated
	CPos cPos, mPos(-1, -1);
	for (Direction i = 1; i <= 4; i++){
		int block = getBlock(board, cPos = start.move(i));
		if (block >= SPECIAL_BLOCK){
			if (mPos.x == -1){
				mPos = cPos;
				path.push_back(i);
				continue;
			}
			if (block < getBlock(board, cPos)){
				path[0] = i;
				mPos = cPos;
			}
		}
	}
	if (mPos.x == -1){
		// cant reach end pos from start pos
		return path;
	}

	// current point = mPoint (minPoint)
	cPos = mPos;
	while (true){
		int block = getBlock(board, cPos);
		if (block == SPECIAL_BLOCK){
			// we reach the end point
			assert(cPos == end);
			break;
		}
		for (Direction i = 1; i <= 4; i++){
			if (getBlock(board, (mPos = cPos.move(i))) == block - 1){
				path.push_back(i);
				cPos = mPos;
				break;
			}
		}
	}
	return path;
}

// this method use a special method to get the length of the longest path when player move form the input position in boardData
int CMyAI::getLengthOfLongestPath(const int boardData[121], const CPos &pos)
{
	return CBiconnectedComponents::getEstimatedLength(boardData, pos);
}

// this method does modify board but restore the original board in the end
void CMyAI::findLongestPath(int board[], CPos& pos, vector<Direction> &c, vector<Direction> &l){

	static bool bOk;
	vector<Direction> allMoves = getAvailableMoves(board, pos);
	for (int i = 0; i < (int)allMoves.size(); i++){
		bOk = move(board, pos, allMoves[i]);
		assert(bOk);
		c.push_back(allMoves[i]);
		if (c.size()>l.size())
			l = c;
		pos = pos.move(allMoves[i]);
		findLongestPath(board, pos, c, l);

		bOk = move(board, pos, getOpositeDirection(allMoves[i]), true); assert(bOk);
		pos = pos.move(getOpositeDirection(allMoves[i]));
		c.pop_back();
	}
}
