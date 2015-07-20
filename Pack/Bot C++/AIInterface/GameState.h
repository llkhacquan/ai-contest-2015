#pragma once
#include "..\AI_Template\mydefine.h"
#include "Pos2D.h"
#include "StaticFunctions.h"

class CGameState
{
private:
	// the first bit indicate the next player: 0 => PLAYER_1; 1 => PLAYER_2
	// the next 119 bit indicate the state of board[1]->board[119] (since board[0] and board[120] always be occuppied)
	//    0 => Empty; 1 => not empty
	bitset<120> boardData; // 15bytes
public:
	static const char F_EXACT = 0;
	static const char F_LOWERBOUND = -1;
	static const char F_UPPERBOUND = 1;
	CGameState();
	CGameState(const TBlock _board[], const Pos2D& _pos1, const Pos2D &_pos2, const TPlayer next);
	~CGameState();
	signed char pos1;
	signed char pos2;
	int value;
	char depth;
	char flag;

	TPlayer getNextPlayer() const;

	bool isEmpty(const Pos1D p) const;

	void set(const TBlock _board[], const Pos2D& _pos1, const Pos2D &_pos2, const TPlayer next);

	void get(TBlock board[], Pos2D& _pos1, Pos2D &_pos2, TPlayer &next) const;

	unsigned long key() const;

	bool isSet();

	void clear();
};

