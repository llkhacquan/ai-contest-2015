#include "GameState.h"
#include "StaticFunctions.h"
#include "TranspositionTable.h"

static set<int> hash1, hash2;



CGameState::CGameState()
{
	clear();
}

CGameState::CGameState(const TBlock _board[], const Pos2D& _pos1, const Pos2D &_pos2, const TPlayer next)
{
	clear();
	set(_board, _pos1, _pos2, next);
}

CGameState::~CGameState()
{
}

TPlayer CGameState::getNextPlayer()  const
{
	return getBit(data.data[0], 0) ? PLAYER_2 : PLAYER_1;
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
	return getBit(data.data[iBit / 15], iBit % 8);
}

void CGameState::set(const TBlock _board[], const Pos2D& _pos1, const Pos2D &_pos2, const TPlayer next)
{
	assert(getBlock(_board, _pos1) == BLOCK_PLAYER_1);
	assert(getBlock(_board, _pos2) == BLOCK_PLAYER_2);

	// set all bit to 1
	memset(data.data, 255, 17);

	data.data[15] = _pos1.to1D();
	assert(data.data[15] >= 0 && data.data[15] < BOARD_SIZE);
	data.data[16] = _pos2.to1D();
	assert(data.data[16] >= 0 && data.data[16] < BOARD_SIZE);

	if (next == PLAYER_1)
		set(0, 0);

	for (int i = 1; i <= 119; i++){
		if (_board[i] == BLOCK_EMPTY)
			set(i, 0);
	}
}

void CGameState::set(int iBit, bool value /*= true*/)
{
	changeBit(data.data[iBit / 15], iBit % 15, value);
}

bool CGameState::isBlockEmpty(const Pos1D p) const
{
	assert(p >= 0 && p < BOARD_SIZE);
	if (p == 0 || p == BOARD_SIZE - 1)
		return true;
	return get(p) == 0;
}

unsigned int CGameState::hash() const
{
	assert(getPos1() >= 0 && getPos1() >= 0);
	unsigned int result = 0;
	// 	for (int i = 0; i < 5; i++)
	// 	{
	// 		unsigned int t = *(unsigned int*)(data.data + i * 3);
	// 		t >>= 8;
	// 		result ^= hashFunction(*this) >> 8;
	// 	};
	static bitset< 120 > bitsetData;
	for (int i = 0; i < 8 * 15; i++)
		if (get(i))
			bitsetData[i] = 1;
		else
			bitsetData[i] = 0;

	result = bitsetData.hash();
	hash1.insert(result);
	result = result % (CTranspositionTable::TABLE_SIZE);
	hash2.insert(result);
	return result;
}

void CGameState::clear()
{
	signed char t = -1;
	memcpy(data.data + 15, &t, 1);
	memcpy(data.data + 16, &t, 1);
	upperBound = MY_INFINITY;
	lowerbound = -MY_INFINITY;
	depth = -1;
	value = TIMEOUT_POINTS;
}

bool CGameState::isSet() const
{
	return getPos1() >= 0;
}

signed char CGameState::getPos2() const
{
	char c;
	memcpy(&c, data.data + 16, 1);
	return c;
}

signed char CGameState::getPos1() const
{
	char c;
	memcpy(&c, data.data + 15, 1);
	return c;
}

CGameState CGameState::operator=(const CGameState &s)
{
	memcpy(data.data, s.data.data, 17);
	upperBound = s.upperBound;
	lowerbound = s.lowerbound;
	return *this;
}

bool CGameState::operator==(const CGameState &state) const
{
	return memcmp(data.data, state.data.data, 17) == 0;
}

bool CGameState::operator!=(const CGameState &state) const
{
	return memcmp(data.data, state.data.data, 17) != 0;
}
