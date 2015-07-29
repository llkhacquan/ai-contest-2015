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
	static int nObjects;
	~CTranspositionTable();
	const int TABLE_SIZE = 1 << 24;
	CGameState *table;

	static CTranspositionTable* getInstance();

	CGameState* put(const CGameState &gameState);

	CGameState* get(const CGameState &gameState) const;

	bool remove(const CGameState &gameState);

	void printStatic(){
		printf("\t\t\t%i/%i/%i\n", nGetOk, nGetMiss, nPut);
	}
};

