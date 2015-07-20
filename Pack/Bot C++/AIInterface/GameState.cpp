#include "GameState.h"


CGameState::CGameState()
{
	int i = sizeof(CGameState);
	i = sizeof(boardData);
	i = sizeof(pos1);
	i = sizeof(pos2);
	i = sizeof(value);
	pos1 = pos2 = -1;
}

CGameState::CGameState(const TBlock _board[], const Pos2D& _pos1, const Pos2D &_pos2, const TPlayer next)
{
	set(_board, _pos1, _pos2, next);
}

CGameState::~CGameState()
{
}

TPlayer CGameState::getNextPlayer()  const
{
	return boardData[0] == 0 ? PLAYER_1 : PLAYER_2;
}

void CGameState::get(TBlock board[], Pos2D& _pos1, Pos2D &_pos2, TPlayer &next) const
{
	_pos1 = Pos2D(pos1);
	_pos2 = Pos2D(pos2);
	next = boardData[0] ? PLAYER_2 : PLAYER_1;
	board[0] = board[BOARD_SIZE - 1] = BLOCK_OBSTACLE;
	for (int i = 1; i <= 119; i++){
		board[i] = boardData[i] ? BLOCK_OBSTACLE : BLOCK_EMPTY;
	}
}

void CGameState::set(const TBlock _board[], const Pos2D& _pos1, const Pos2D &_pos2, const TPlayer next)
{
	assert(getBlock(_board, _pos1) == BLOCK_PLAYER_1);
	assert(getBlock(_board, _pos2) == BLOCK_PLAYER_2);

	pos1 = _pos1.to1D();
	assert(pos1 >= 0 && pos1 < BOARD_SIZE);
	pos2 = _pos2.to1D();
	assert(pos2 >= 0 && pos2 < BOARD_SIZE);

	TBlock board[BOARD_SIZE];

	boardData.set(true); // set all bit to 1

	if (next == PLAYER_1)
		boardData.set(0, 0);

	memcpy(board, _board, BOARD_SIZE*sizeof(TBlock));
	fillDistance(board, _pos1);
	for (int i = 1; i <= 119; i++){
		if (board[i] > SPECIAL_BLOCK)
			boardData.set(i, 0);
	}

	memcpy(board, _board, BOARD_SIZE*sizeof(TBlock));
	fillDistance(board, _pos2);
	for (int i = 1; i <= 119; i++){
		if (board[i] > SPECIAL_BLOCK)
			boardData.set(i, 0);
	}
}

bool CGameState::isEmpty(const Pos1D p) const
{
	assert(p >= 0 && p < BOARD_SIZE);
	if (p == 0 || p == BOARD_SIZE - 1)
		return true;
	return boardData[p] == 0;
}

unsigned long CGameState::key() const
{
	assert(pos1 >= 0 && pos2 >= 0);
	unsigned long hash = 5381;
	int c;
	for (int i = 0; i < 4; i++)
	{
		c = *(((int*)this) + i);
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}

	char c2;
	c2 = *(((char*)this) + 16);
	hash = ((hash << 5) + hash) + c2; /* hash * 33 + c */
	return hash;
}

void CGameState::clear()
{
	pos1 = pos2 = -1;
}

bool CGameState::isSet()
{
	return pos1 >= 0;
}
