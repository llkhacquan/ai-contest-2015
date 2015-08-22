#pragma once
#include "GameState.h"
class CTranspositionTable
{
private:
	static CTranspositionTable* instance;
	CTranspositionTable();

public:
	static int nGetOk;
	static int nGetMiss;
	static int nPut;
	static int nBadHash;
	static int nObjects;
	~CTranspositionTable();
	static const int TABLE_SIZE = 16777991; // a prime number
	CGameState *table;

	static CTranspositionTable* getInstance();

	CGameState* put(const CGameState &gameState);

	CGameState* get(const CGameState &gameState) const;

	bool remove(const CGameState &gameState);

	void printStatic();
};

