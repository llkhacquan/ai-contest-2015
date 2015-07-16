#pragma once
#include "ai/defines.h"
#include "ai/AI.h"
#include "mydefine.h"
#include "Pos.h"

typedef int Vertex;

struct Edge{
	Vertex u, v;

	Edge() :u(0), v(0){}
	Edge(Vertex _v1, Vertex _v2) : u(_v1), v(_v2) {	}

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
		int v1 = min(u, v);
		int v2 = max(u, v);
		return v2 * 10000 + v1;
	}

	bool operator < (const Edge& e) const
	{
		return (hash() < e.hash());
	}
};

struct AdjArea{
	int codeOfAdjArea;
	Vertex connections;
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

struct Area{
	int code;
	bool inTheAreas[BOARD_SIZE];
	int nVertices;
	set<AdjArea> adjAreas;

	Area(){
		nVertices = 0;
		memset(inTheAreas, 0, BOARD_SIZE);
	}

	void insert(const Vertex &u){
		if (!inTheAreas[u]){
			inTheAreas[u] = true;
			nVertices++;
		}
	}

	void erase(const Vertex &u){
		if (inTheAreas[u]){
			inTheAreas[u] = false;
			nVertices--;
		}
	}

	Area operator =(const Area &e) {
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
	CPos playerPos;
	int* oBoard;
	vector<Area> areas;
	// data

	bool visited[BOARD_SIZE];
	int parrent[BOARD_SIZE];
	int d[BOARD_SIZE];
	int low[BOARD_SIZE];
	int iCount;
	int nComponents;
	stack<Edge> myStack;

	static int getEstimatedLength(int const board[], const CPos &playerPos);
	static int findLengthOfLongestPath(const int _oBoard[], const vector<Area> &areas, const set<Edge> &edgesOfCode, int startArea, const int &startPos);
	static int calculateLengthOfPath(const int _oBoard[], const vector<Area> &areas, const set<Edge> &edgesOfCode, const vector<int> &path, const int &startPos);
	static void visitNode(const int _oBoard[], const vector<Area> &areas, const set<Edge> &edgesOfCode,
		vector<int> &cPath, int &cLength, vector<int> &lPath, int &lLength, vector<bool> &visitted, const int cCode, const int &startPos);

	static vector<Area> biconnectedComponents(int const board[], const CPos &playerPos, int outBoard[]);
	void dfsVisit(const Vertex & u);
	void createNewArea(const Vertex &v1, const Vertex &v2);
	void adjection(bool out[], Vertex const &u);

	static int rateBoardForAPlayer(int const oBoard[], const CPos &playerPos);
};


