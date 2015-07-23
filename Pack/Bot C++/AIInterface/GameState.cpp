#include "GameState.h"
#include "StaticFunctions.h"


CGameState::CGameState()
{
	clear();
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
	return getBit(data[0], 0) ? PLAYER_2 : PLAYER_1;
}

void CGameState::get(TBlock board[], Pos2D& _pos1, Pos2D &_pos2, TPlayer &next) const
{
	_pos1 = Pos2D(getPos1());
	_pos2 = Pos2D(getPos2());
	next = getNextPlayer();
	board[0] = board[BOARD_SIZE - 1] = BLOCK_OBSTACLE;
	for (int i = 1; i <= 119; i++){
		board[i] = get(i) ? BLOCK_OBSTACLE : BLOCK_EMPTY;
	}
	board[_pos1] = BLOCK_PLAYER_1;
	board[_pos2] = BLOCK_PLAYER_1;
}

bool CGameState::get(int iBit) const
{
	return getBit(data[iBit / 15], iBit % 8);
}

void CGameState::set(const TBlock _board[], const Pos2D& _pos1, const Pos2D &_pos2, const TPlayer next)
{
	assert(getBlock(_board, _pos1) == BLOCK_PLAYER_1);
	assert(getBlock(_board, _pos2) == BLOCK_PLAYER_2);

	// set all bit to 1
	memset(data, 255, 17);

	data[15] = _pos1.to1D();
	assert(data[15] >= 0 && data[15] < BOARD_SIZE);
	data[16] = _pos2.to1D();
	assert(data[16] >= 0 && data[16] < BOARD_SIZE);

	TBlock board[BOARD_SIZE];

	if (next == PLAYER_1)
		set(0, 0);


	memcpy(board, _board, BOARD_SIZE*sizeof(TBlock));
	fillDistance(board, _pos1);
	for (int i = 1; i <= 119; i++){
		if (board[i] > SPECIAL_BLOCK)
			set(i, 0);
	}

	memcpy(board, _board, BOARD_SIZE*sizeof(TBlock));
	fillDistance(board, _pos2);
	for (int i = 1; i <= 119; i++){
		if (board[i] > SPECIAL_BLOCK)
			set(i, 0);
	}
}

void CGameState::set(int iBit, bool value /*= true*/)
{
	changeBit(data[iBit / 15], iBit % 15, value);
}

bool CGameState::isBlockEmpty(const Pos1D p) const
{
	assert(p >= 0 && p < BOARD_SIZE);
	if (p == 0 || p == BOARD_SIZE - 1)
		return true;
	return get(p) == 0;
}

unsigned long CGameState::hash() const
{
	assert(getPos1() >= 0 && getPos1() >= 0);
	int result = 0;
	for (int i = 0; i < 5; i++)
	{
		int t = *(int*)(data + i * 3);
		t >>= 8;
		result ^= t;
	};

	int t = *(int*)data + 15;
	t = (t << 16) + t;
	result ^= t;
	return result;
}

void CGameState::clear()
{
	signed char t = -1;
	memcpy(data + 15, &t, 1);
	memcpy(data + 16, &t, 1);
}

bool CGameState::isSet() const
{
	return getPos1() >= 0;
}

signed char CGameState::getPos2() const
{
	char c;
	memcpy(&c, data + 16, 1);
	return c;
}

signed char CGameState::getPos1() const
{
	char c;
	memcpy(&c, data + 15, 1);
	return c;
}

CGameState CGameState::operator=(const CGameState &state)
{
	memcpy(data, state.data, 17);
	flag = state.flag;
	depth = state.depth;
	value = state.value;
	return *this;
}

bool CGameState::operator==(const CGameState &state) const
{
	return memcmp(data, state.data, 17) == 0;
}

bool CGameState::operator!=(const CGameState &state) const
{
	return memcmp(data, state.data, 17) != 0;
}
