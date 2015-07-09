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

struct Area{
	int odd, even;
	int code;

	vector<Vertex> vertices;
	vector<int> neiboorAreas;

	Area() :odd(0), even(0){}
	Area(int _v1, int _v2) : odd(_v1), even(_v2) {}
	Area operator =(const Area &e) {
		odd = e.odd; even = e.even; code = e.code;
		vertices = vector<Vertex>(e.vertices);
		neiboorAreas = vector<int>(e.neiboorAreas);
		return *this;
	}

	bool operator==(const Area &e) const {
		return odd + even == e.odd + e.even;
	}

	bool operator<(const Area &e) const {
		return odd + even < e.odd + e.even;
	}

	bool operator!=(const Area &e) const {
		return odd + even != e.odd + e.even;
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
	static int findLengthOfLongestPath(const int _oBoard[], const vector<Area> &areas, const vector<Edge> &edges, const vector<Edge> &edgesOfCode);
	static int calculateLengthOfPath(const int _oBoard[], const vector<Area> &areas, const vector<Edge> &edges, const vector<Edge> &edgesOfCode, const vector<int> &path);
	static void visitNode(const int _oBoard[], const vector<Area> &areas, const vector<Edge> &edges, const vector<Edge> &edgesOfCode, vector<int> &cPath, int &cLength, vector<int> &lPath, int &lLength, bool visitted[], const int cCode);

	vector<Area> biconnectedComponents(int const board[], const CPos &playerPos, int outBoard[]);
	void dfsVisit(const Vertex & u);
	void createNewArea(const Vertex &v1, const Vertex &v2);
	vector<Vertex> adjection(Vertex const &u);
};


