#pragma once
#include "GameState.h"
class CTranspositionTable
{
private:
	static CTranspositionTable* instance;
	CTranspositionTable();
	~CTranspositionTable();

public:
	const int TABLE_SIZE = 1 << 25;
	CGameState *table;

	static CTranspositionTable* getInstance();

	void put(const CGameState &gameState);

	CGameState* get(const CGameState &gameState);
};

