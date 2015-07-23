#include "TranspositionTable.h"

int CTranspositionTable::nGetMiss;
int CTranspositionTable::nGetOk;
int CTranspositionTable::nObjects;
int CTranspositionTable::nPut;

CTranspositionTable * CTranspositionTable::instance = NULL;

CGameState* CTranspositionTable::get(const CGameState &gameState)const
{
	unsigned long key = gameState.hash();
	int hash = (key % TABLE_SIZE);
	while (table[hash].isSet() && table[hash] != gameState)
		hash = (hash + 1) % TABLE_SIZE;
	if (!table[hash].isSet())
	{
		nGetMiss++;
		return NULL;
	}
	else
	{
		nGetOk++;
		return &table[hash];
	}
}

CGameState* CTranspositionTable::put(const CGameState &gameState)
{
	assert(nObjects <= TABLE_SIZE);
	if (nObjects == TABLE_SIZE){
		return NULL;
	}
	nPut++;
	unsigned long key = gameState.hash();
	int hash = (key % TABLE_SIZE);
	while (table[hash].isSet() && table[hash] != gameState)
		hash = (hash + 1) % TABLE_SIZE;
	table[hash] = gameState;
	return table + hash;
}

CTranspositionTable::~CTranspositionTable()
{
	instance = NULL;
	clock_t startTime = clock();
	delete[] table;
	cout << "release memory of Transposition table : " << double(clock() - startTime) / (double)CLOCKS_PER_SEC << " seconds." << endl;
	cout << "nGetMiss = " << nGetMiss << endl;
	cout << "nGetOk = " << nGetOk << endl;
	cout << "nPut = " << nPut << endl;
}

CTranspositionTable* CTranspositionTable::getInstance()
{
	if (instance == NULL)
		instance = new CTranspositionTable();
	return instance;
}

CTranspositionTable::CTranspositionTable()
{
	nObjects = 0;
	clock_t startTime = clock();
	table = new CGameState[TABLE_SIZE];
	cout << "Creating CTT : " << double(clock() - startTime) / (double)CLOCKS_PER_SEC << " seconds." << endl;
	nGetMiss = nGetOk = nPut = 0;
}

bool CTranspositionTable::remove(const CGameState &gameState)
{
	CGameState *s = get(gameState);
	if (s == NULL)
		return false;
	else
	{
		s->clear();
		return true;
	}
}
