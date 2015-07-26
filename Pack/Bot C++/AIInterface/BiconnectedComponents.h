#pragma once
#include "..\AI_Template\include/ai/defines.h"
#include "..\AI_Template\include/ai/AI.h"
#include "..\AI_Template\mydefine.h"
#include "Pos2D.h"
#include "BiconnectedComponentsOutput.h"

class Edge{
public:
	Pos1D u, v;

	Edge() :u(0), v(0){}
	Edge(Pos1D _v1, Pos1D _v2) : u(_v1), v(_v2) {	}

	Edge operator =(const Edge &e) { u = e.u; v = e.v; return *this; }

	bool operator==(const Edge &e) const {
		return (u == e.u && v == e.v) || (u == e.v && v == e.u);
	}

	bool operator!=(const Edge &e) const {
		return !((u == e.u && v == e.v) || (u == e.v && v == e.u));
	}

	void rearange(){
		if (v < u)
			std::swap(v, u);
	}

	int hash() const{
		if (v < u)
			return (u << 15) + v;
		else
			return (v << 15) + u;
	}

	bool operator < (const Edge& e) const
	{
		return (hash() < e.hash());
	}
};

class AdjArea{
public:
	int codeOfAdjArea;
	Pos1D connections;
	AdjArea operator=(const AdjArea &a){
		codeOfAdjArea = a.codeOfAdjArea;
		connections = a.connections;
	}

	bool operator==(const AdjArea &e) const {
		return codeOfAdjArea == e.codeOfAdjArea && connections == e.connections;
	}

	bool operator<(const AdjArea &e) const {
		return codeOfAdjArea < e.codeOfAdjArea || (codeOfAdjArea == e.codeOfAdjArea && connections < e.connections);
	}

	bool operator>(const AdjArea &e) const {
		return codeOfAdjArea > e.codeOfAdjArea || (codeOfAdjArea == e.codeOfAdjArea && connections > e.connections);
	}
};

class Area{
public:
	int code;
	bool inTheAreas[BOARD_SIZE];
	int odd, even;
	int nVertices;
	set<AdjArea> adjAreas;

	Area(){
		clear();
	}

	void clear(){
		nVertices = 0;
		odd = even = 0;
		memset(inTheAreas, 0, BOARD_SIZE);
	}

	void insert(const Pos1D &u){
		if (!inTheAreas[u]){
			inTheAreas[u] = true;
			nVertices++;
			if (u % 2 == 1)
				odd++;
			else
				even++;
		}
	}

	void erase(const Pos1D &u){
		if (inTheAreas[u]){
			inTheAreas[u] = false;
			nVertices--;
			if (u % 2 == 1)
				odd--;
			else
				even--;
		}
	}

	Area operator =(const Area &e) {
		odd = e.odd;
		even = e.even;
		code = e.code;
		nVertices = e.nVertices;
		memcpy(inTheAreas, e.inTheAreas, BOARD_SIZE);
		adjAreas = set<AdjArea>(e.adjAreas);
		return *this;
	}

	bool operator==(const Area &e) const {
		return nVertices == e.nVertices;
	}

	bool operator>(const Area &e) const {
		return nVertices > e.nVertices;
	}

	bool operator<(const Area &e) const {
		return nVertices < e.nVertices;
	}

	bool operator!=(const Area &e) const {
		return nVertices > e.nVertices;
	}
};

class CBiconnectedComponents
{
public:
	CBiconnectedComponents();
	~CBiconnectedComponents();

	// input
	TBlock oBoard[BOARD_SIZE];
	CBiconnectedComponentsOutput *output;
	// data

	bool visited[BOARD_SIZE];
	int parrent[BOARD_SIZE];
	int d[BOARD_SIZE];
	int low[BOARD_SIZE];
	int iCount;
	stack<Edge> myStack;

	static int getEstimatedLength(TBlock const board[], const Pos2D &playerPos);
	static void biconnectedComponents(TBlock const board[], const Pos2D &playerPos, CBiconnectedComponentsOutput *output, TBlock *oBoard = NULL);
private:
	void dfsVisit(const Pos1D & u);
	void createNewArea(const Pos1D &v1, const Pos1D &v2);
	void adjection(bool out[], Pos1D const &u);
};


