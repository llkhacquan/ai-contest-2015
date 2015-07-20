#include "TranspositionTable.h"

CTranspositionTable * CTranspositionTable::instance = NULL;

CGameState* CTranspositionTable::get(const CGameState &gameState)
{
	unsigned long key = gameState.key();
	int hash = (key % TABLE_SIZE);
	while (table[hash].isSet() && table[hash].key() != key)
		hash = (hash + 1) % TABLE_SIZE;
	if (!table[hash].isSet())
		return NULL;
	else
		return &table[hash];
}

void CTranspositionTable::put(const CGameState &gameState)
{
	unsigned long key = gameState.key();
	int hash = (key % TABLE_SIZE);
	while (table[hash].isSet() && table[hash].key() != key)
		hash = (hash + 1) % TABLE_SIZE;
	if (table[hash].isSet())
		table[hash].clear();
	table[hash] = gameState;
}

CTranspositionTable::~CTranspositionTable()
{
	clock_t startTime = clock();
	delete[] table;
	instance = NULL;
	cout << "release memory of CTT : " << double(clock() - startTime) / (double)CLOCKS_PER_SEC << " seconds." << endl;
}

CTranspositionTable* CTranspositionTable::getInstance()
{
	if (instance == NULL)
		instance = new CTranspositionTable();
	return instance;
}

CTranspositionTable::CTranspositionTable()
{
	clock_t startTime = clock();
	table = new CGameState[TABLE_SIZE];
	cout << "Creating CTT : " << double(clock() - startTime) / (double)CLOCKS_PER_SEC << " seconds." << endl;
}
