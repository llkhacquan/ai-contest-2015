#pragma once
#include "mydefine.h"
#include "Pos2D.h"
#include "StaticFunctions.h"

class GameStateData{
public:

	// the first bit indicate the next player: 0 => PLAYER_1; 1 => PLAYER_2
	// the next 119 bit indicate the state of board[1]->board[119] (since board[0] and board[120] always be occuppied)
	//    0 => Empty; 1 => not empty
	unsigned char data[17]; // the last 2 byte indicates player 1 and player 2 positions
};

class CGameState
{
private:
	GameStateData data;
public:
	CGameState();
	CGameState(const TBlock _board[], const Pos2D& _pos1, const Pos2D &_pos2, const TPlayer next);
	~CGameState();
	TMove bestMove[4];
	char depth;
	int lowerbound;
	int upperBound;
	int value;
	enum Flag{ EXACT, LOWER, UPPER } flag;

	TPlayer getNextPlayer() const;

	bool isBlockEmpty(const Pos1D p) const;

	void set(const TBlock _board[], const Pos2D& _pos1, const Pos2D &_pos2, const TPlayer next);

	void get(TBlock board[], Pos2D& _pos1, Pos2D &_pos2, TPlayer &next) const;

	inline bool get(int iBit) const;

	inline void set(int iBit, bool value = true);

	unsigned int hash() const;

	bool isSet() const;

	void clear();

	inline signed char getPos1() const;

	inline signed char getPos2() const;

	CGameState CGameState::operator=(const CGameState &state);

	bool operator==(const CGameState &state) const;

	bool operator!=(const CGameState &state) const;
};

