#include "GameState.h"
#include "StaticFunctions.h"
#include "TranspositionTable.h"


CGameState::CGameState()
{
	clear();
}

CGameState::CGameState(const vector<TMove> &_history)
{
	clear();
	set(_history);
}

CGameState::~CGameState()
{
}

void CGameState::set(const vector<TMove> &_history)
{
	// set all bit to 1
	historyLength = _history.size();
	history.reset();
	for (int i = 0; i < historyLength; i++){
		switch (_history[i]){
		case DIRECTION_LEFT: // 00
			history.set(i * 2, 0); history.set(i * 2 + 1, 0);
			break;
		case DIRECTION_UP: // 01
			history.set(i * 2, 0); history.set(i * 2 + 1, 1);
			break;
		case DIRECTION_RIGHT: //10
			history.set(i * 2, 1); history.set(i * 2 + 1, 0);
			break;
		case DIRECTION_DOWN: //11
			history.set(i * 2, 1); history.set(i * 2 + 1, 1);
			break;
		default:
			assert(false);
		}
	}
}

unsigned int CGameState::hash() const
{
	assert(isSet());
	static const int n = 20; // (n>=12)
	static bitset< n * 2 > bitsetData;
	bitsetData.reset();

	for (int i = 0; i < n * 2; i++){
		int iBit = historyLength * 2 - n * 2 + i;
		if (iBit >= 0)
			bitsetData.set(i, history[iBit]);
		else
			bitsetData.set(i, 0);
	}
	return bitsetData.hash() % (CTranspositionTable::TABLE_SIZE);
}

void CGameState::clear()
{
	signed char t = -1;
	upper = MY_INFINITY;
	lower = -MY_INFINITY;
	vono = simple = MY_INFINITY;
	length1 = length2 = -1;
	depth = -1;
	historyLength = -1;
}

bool CGameState::isSet() const
{
	return historyLength >= 0;
}

bool CGameState::operator==(const CGameState &state) const
{
	return historyLength == state.historyLength && history == state.history;
}

bool CGameState::operator!=(const CGameState &state) const
{
	return !(historyLength == state.historyLength && history == state.history);
}

CGameState & CGameState::operator=(const CGameState &state)
{
	historyLength = state.historyLength;
	history = state.history;
	depth = state.depth;
	lower = state.lower;
	upper = state.upper;
	return *this;
}
