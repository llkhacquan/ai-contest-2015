#include "MyAI.h"
#include "mydefine.h"
#include "BiconnectedComponents.h"

int CMyAI::rateBoard(int board[121], const CPos &_p1, const CPos &_p2, const Player next)
{
#ifdef _DEBUG
	int backup[121];
	memcpy(backup, board, BOARD_SIZE*sizeof(int));
#endif // _DEBUG

	assert(next == PLAYER_1 || next == PLAYER_2);
	assert(getBlock(board, _p1) == BLOCK_PLAYER_1 && getBlock(board, _p2) == BLOCK_PLAYER_2);
	int winner = evaluateBoard(board, _p1, _p2, next);
	int result;
	switch (winner){
	case PLAYER_1:
		result = MAX_POINTS;
		break;
	case PLAYER_2:
		result = MIN_POINTS;
		break;
	case OBSERVER:
	{
		static int board1[121], board2[121];
		memcpy(board1, board, BOARD_SIZE*sizeof(int));
		float f1 = fillDistance(board1, _p1);
		memcpy(board2, board, BOARD_SIZE*sizeof(int));
		float f2 = fillDistance(board2, _p2);
		int n1 = 0, n2 = 0, e1 = 0, e2 = 0;
		for (int i = 0; i < BOARD_SIZE; i++){
			if (board1[i] < board2[i])
			{
				n1++;
				e1 += getAvailableMoves(board, CPos(i)).size();
			}
			else if (board1[i] > board2[i])
			{
				n2++;
				e2 += getAvailableMoves(board, CPos(i)).size();
			}
		}

		int k1 = 55, k2 = 194;
		n1 = k1*n1 + k2*e1;
		n2 = k1*n2 + k2*e2;
		result = n1  * (MAX_POINTS - MIN_POINTS) / (n1 + n2) + MIN_POINTS;
	}
	break;
	default:
		assert(false);
		result = (MAX_POINTS + MIN_POINTS) / 2;
		break;
	}
#ifdef _DEBUG
	assert(memcmp(board, backup, BOARD_SIZE*sizeof(int)) == 0);
#endif // _DEBUG

	return result;
}

int CMyAI::rateBoard2(int board[], const CPos &_p1, const CPos &_p2, const Player next){
#ifdef _DEBUG
	int backup[BOARD_SIZE];
	memcpy(backup, board, BOARD_SIZE*sizeof(int));
#endif // _DEBUG

	// not isolated mode, not a leaf node
	assert(next == PLAYER_1 || next == PLAYER_2);
	assert(getBlock(board, _p1) == BLOCK_PLAYER_1 && getBlock(board, _p2) == BLOCK_PLAYER_2);
	int result = 0;
	int n1 = 0, n2 = 0;

	static int board1[BOARD_SIZE], board2[BOARD_SIZE];
	static int oBoard1[BOARD_SIZE], oBoard2[BOARD_SIZE];
	memcpy(board1, board, BOARD_SIZE*sizeof(int));
	memcpy(oBoard1, board, BOARD_SIZE*sizeof(int));
	memcpy(board2, board, BOARD_SIZE*sizeof(int));
	memcpy(oBoard2, board, BOARD_SIZE*sizeof(int));
	fillDistance(board1, _p1);
	fillDistance(board2, _p2);

	for (int i = 0; i < BOARD_SIZE; i++){
		if (board1[i] > SPECIAL_BLOCK && board2[i] > SPECIAL_BLOCK){
			if (board1[i] > board2[i])
			{
				oBoard1[i] = -1;
			}
			else if (board1[i] < board2[i]){
				oBoard2[i] = -1;
			}
			else if (next == PLAYER_1){
				oBoard1[i] = -1;
			}
			else
				oBoard2[i] = -1;
		}
	}

	n1 = CBiconnectedComponents::getEstimatedLength(oBoard1, _p1);
	n2 = CBiconnectedComponents::getEstimatedLength(oBoard2, _p2);

	result = n1 *(MAX_POINTS - MIN_POINTS) / (n1 + n2) + MIN_POINTS;
#ifdef _DEBUG
	assert(memcmp(board, backup, BOARD_SIZE*sizeof(int)) == 0);
#endif // _DEBUG

	return result;
}