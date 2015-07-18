#pragma once
#include "..\AI_Template\mydefine.h"
#include "Pos2D.h"

class CGameState
{
public:
	CGameState();
	~CGameState();

	int board[BOARD_SIZE];
	Pos2D p1;
	Pos2D p2;
	TPlayer nextPlayer; // indicate the player who will take the next turn PLAYER_1 or PLAYER_2
};

