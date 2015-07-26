#include "HeuristicBase.h"
#include "BiconnectedComponents.h"
#include "..\AI_Template\MyAI.h"
#include "MyTimer.h"
#include "GameState.h"
#include "TranspositionTable.h"


CHeuristicBase::CHeuristicBase()
{
}

CHeuristicBase::~CHeuristicBase()
{
}

void CHeuristicBase::exploreToPathLongestPath(TBlock board[], Pos2D &pos, vector<TMove> &cPath, vector<TMove> &oldPath, int &length, int depth)
{
	assert(getBlock(board, pos) == BLOCK_PLAYER_1 || getBlock(board, pos) == BLOCK_PLAYER_2);
	assert(depth >= 0);
	auto availMoves = getAvailableMoves(board, pos);
	bool bOk;
	if (depth == 0 || availMoves.size() == 0 || CMyTimer::getInstance()->timeUp()) {
		int l = CHeuristicBase::getEstimatedLengthOfTheLongestPath(board, pos);
		if (cPath.size() + l > oldPath.size() + length){
			length = l;
			oldPath = vector<TMove>(cPath);
		}
	}
	else
		for (unsigned int i = 0; i < availMoves.size(); i++){
			bOk = move(board, pos, availMoves[i]); assert(bOk);
			pos = pos.move(availMoves[i]);
			cPath.push_back(availMoves[i]);

			exploreToPathLongestPath(board, pos, cPath, oldPath, length, depth - 1);

			TMove back = getOpositeDirection(availMoves[i]);
			bOk = move(board, pos, back, true); assert(bOk);
			pos = pos.move(back);
			cPath.pop_back();
		}
}

TMove CHeuristicBase::getFirstMoveOfTheLongestPath(const TBlock boardData[], const Pos2D &pos, const int nDepth /*= 0*/)
{
	TBlock board[BOARD_SIZE];
	memcpy(board, boardData, BOARD_SIZE*sizeof(TBlock));
	vector<TMove> c(nDepth), l(nDepth);
	c.clear();
	l.clear();
	int length = 0;
	Pos2D p = pos;
	exploreToPathLongestPath(board, p, c, l, length, nDepth);
	assert(l.size() > 0);
	return l[0];
}

vector<TMove> CHeuristicBase::getFirstMovesOfTheLongestPath(const TBlock boardData[], const Pos2D &pos)
{
	TBlock board[BOARD_SIZE];
	memcpy(board, boardData, BOARD_SIZE*sizeof(TBlock));
	int iMax = 0;
	vector<TMove> result(4);
	result.clear();

	bool bMoves[4];
	{
		vector<TMove> availMoves = getAvailableMoves(boardData, pos, bMoves);
		if (availMoves.size() == 1) {
			result.push_back(availMoves[0]);
		}
	}

	int lengths[4] = { -1 };
	int maxLength = -1;
	for (unsigned int iMove = 1; iMove <= 4; iMove++){
		if (!bMoves[iMove - 1])
			continue;

		bool bOk = move(board, pos, iMove); assert(bOk);
		lengths[iMove - 1] = getEstimatedLengthOfTheLongestPath(board, pos.move(iMove));
		if (lengths[iMove - 1] > maxLength){
			iMax = iMove;
			maxLength = lengths[iMove - 1];
		}
		bOk = move(board, pos.move(iMove), getOpositeDirection(iMove), true);
		assert(bOk);
	}
	DEBUG(cout << "estimated length = " << maxLength << endl);
	for (unsigned int i = 0; i < 4; i++){
		if (lengths[i] == maxLength)
			result.push_back(i + 1);
	}
	return result;
}

int CHeuristicBase::getEstimatedLengthOfTheLongestPath(TBlock const board[], const Pos2D &playerPos)
{
	return CBiconnectedComponents::getEstimatedLength(board, playerPos);
}

// return the player will win : PLAYER_1 or PLAYER_2 or OBSERVER if unknown
int CHeuristicBase::evaluateBoard(const TBlock _board[121], const Pos2D &_player1, const Pos2D &_player2, const TPlayer next, int &point) {
	assert(getBlock(_board, _player1) == BLOCK_PLAYER_1);
	assert(getBlock(_board, _player2) == BLOCK_PLAYER_2);
	if (isIsolated(_board, _player1, _player2)){
		int length1 = CHeuristicBase::getEstimatedLengthOfTheLongestPath(_board, _player1);
		int length2 = CHeuristicBase::getEstimatedLengthOfTheLongestPath(_board, _player2);
		if (length1 > length2) {
			point = (length1 - length2)*(MAX_POINTS - MIN_POINTS) / 4 / (max(length1, length2));
			return PLAYER_1;
		}
		else if (length1 < length2) {
			point = (length2 - length1)*(MAX_POINTS - MIN_POINTS) / 4 / (max(length1, length2));
			return PLAYER_2;
		}
		else if (next == PLAYER_1) {
			point = (MAX_POINTS - MIN_POINTS) / 4 / (length1 + 2);
			return PLAYER_2;
		}
		else {
			point = (MAX_POINTS - MIN_POINTS) / 4 / (length1 + 2);
			return PLAYER_1;
		}
	}
	return OBSERVER;
}

int CHeuristicBase::simpleRateBoard(TBlock board[], const Pos2D &_p1, const Pos2D &_p2, const TPlayer next){
#ifdef _DEBUG
	TBlock backup[BOARD_SIZE];
	memcpy(backup, board, BOARD_SIZE*sizeof(TBlock));
#endif // _DEBUG

	// not isolated mode, not a leaf node
	assert(next == PLAYER_1 || next == PLAYER_2);
	assert(getBlock(board, _p1) == BLOCK_PLAYER_1 && getBlock(board, _p2) == BLOCK_PLAYER_2);
	int result = 0;
	int n1 = 0, n2 = 0;

	static TBlock dBoard1[BOARD_SIZE], dBoard2[BOARD_SIZE];
	static TBlock board1[BOARD_SIZE], board2[BOARD_SIZE];

	memcpy(dBoard1, board, BOARD_SIZE*sizeof(TBlock));
	memcpy(dBoard2, board, BOARD_SIZE*sizeof(TBlock));
	fillDistance(dBoard1, _p1);
	fillDistance(dBoard2, _p2);

	memcpy(board1, board, BOARD_SIZE*sizeof(TBlock));
	memcpy(board2, board, BOARD_SIZE*sizeof(TBlock));

	for (int i = 0; i < BOARD_SIZE; i++){
		if (getBit(dBoard1[i], SPECIAL_BIT) && getBit(dBoard2[i], SPECIAL_BIT)){
			if (dBoard1[i] > dBoard2[i])
			{
				board1[i] = BLOCK_ENEMY_AREA;
			}
			else if (dBoard1[i] < dBoard2[i]){
				board2[i] = BLOCK_ENEMY_AREA;
			}
			else if (next == PLAYER_1){
				board1[i] = BLOCK_ENEMY_AREA;
			}
			else
				board2[i] = BLOCK_ENEMY_AREA;
		}
	}

	dBoard1[_p1] = BLOCK_PLAYER_1;
	dBoard2[_p2] = BLOCK_PLAYER_2;

	n1 = CBiconnectedComponents::getEstimatedLength(board1, _p1);
	n2 = CBiconnectedComponents::getEstimatedLength(board2, _p2);
	if (n1 + n2 == 0){
		if (next == PLAYER_1)
			return (MAX_POINTS + 3 * MIN_POINTS) / 4 - 1;
		else
			return (MAX_POINTS * 3 + MIN_POINTS) / 4 + 1;
	}
	DEBUG(cout << "n1/n2 = " << n1 << "/" << n2 << endl);
	result = n1 *(MAX_POINTS - MIN_POINTS) / (n1 + n2) / 2 + MIN_POINTS / 2;
#ifdef _DEBUG
	assert(memcmp(board, backup, BOARD_SIZE*sizeof(TBlock)) == 0);
#endif // _DEBUG

	return result;
}

int CHeuristicBase::voronoiRateBoard(TBlock board[], const Pos2D &_p1, const Pos2D &_p2, const int next)
{
#ifdef _DEBUG
	TBlock backup[BOARD_SIZE];
	memcpy(backup, board, BOARD_SIZE*sizeof(TBlock));
#endif // _DEBUG

	// not a leaf node
	assert(next == PLAYER_1 || next == PLAYER_2);
	assert(getBlock(board, _p1) == BLOCK_PLAYER_1 && getBlock(board, _p2) == BLOCK_PLAYER_2);
	int result = 0;
	int n1 = 0, n2 = 0;

	static TBlock dBoard1[BOARD_SIZE], dBoard2[BOARD_SIZE];
	memcpy(dBoard1, board, BOARD_SIZE*sizeof(TBlock));
	memcpy(dBoard2, board, BOARD_SIZE*sizeof(TBlock));

	bool visited[BOARD_SIZE] = { false };
	queue<Pos1D> q1, q2, *q;
	int *n;
	q1.push(_p1); q2.push(_p2);
	visited[_p1] = visited[_p2] = true;
	TPlayer currentPlayer = next;

	while (!q1.empty() && !q2.empty()){
		if (currentPlayer == PLAYER_1){
			q = &q1;
			n = &n1;
			currentPlayer = PLAYER_2;
		}
		else{
			q = &q2;
			n = &n2;
			currentPlayer = PLAYER_1;
		}
		if (!q->empty()){
			Pos2D  u(q->front());
			q->pop();

			*n = *n + 1;

			for (int m = 1; m <= 4; m++){
				Pos2D v = u.move(m);
				int iV = v;
				if (visited[iV])
					continue;

				if (getBlock(board, v) == BLOCK_EMPTY){
					visited[iV] = true;
					q->push(iV);
				}
			}
		}
	}

	fillDistance(dBoard1, _p1);
	fillDistance(dBoard2, _p2);

	for (int i = 0; i < BOARD_SIZE; i++){
		if (getBit(dBoard1[i], SPECIAL_BIT) && getBit(dBoard2[i], SPECIAL_BIT)){
			if (dBoard1[i] > dBoard2[i])
			{
				n2++;
			}
			else if (dBoard1[i] < dBoard2[i]){
				n1++;
			}
			else if (next == PLAYER_1){
				n1++;
			}
			else
			{
				n2++;
			}
		}
	}
	DEBUG(cout << "n1/n2 = " << n1 << "/" << n2 << endl);
	if (n1 + n2 != 0)
		result = n1 *(MAX_POINTS - MIN_POINTS) / (n1 + n2) / 2 + MIN_POINTS / 2;
	else if (next == PLAYER_2)
		result = (MAX_POINTS * 3 + MIN_POINTS) / 4 + 1;
	else
		result = (MAX_POINTS + 3 * MIN_POINTS) / 4 - 1;
#ifdef _DEBUG
	assert(memcmp(board, backup, BOARD_SIZE*sizeof(TBlock)) == 0);
#endif // _DEBUG

	return result;
}

void CHeuristicBase::sortMoves(vector<TMove> &moves, TBlock* board, const Pos2D &_p1, const Pos2D &_p2, const TPlayer next)
{
	static CMyAI* ai = CMyAI::getInstance();
	assert(moves.size() <= 4);
	int points[4];
	bool bOk;

	int i = 0;
	for (vector<TMove>::iterator m = moves.begin(); m != moves.end(); m++){
		if (next == PLAYER_1){
			bOk = move(board, _p1, *m, false); assert(bOk);
			points[i] = ai->searcher.heuristic.quickRateBoard(board, _p1.move(*m), _p2, PLAYER_2);
			bOk = move(board, _p1.move(*m), getOpositeDirection(*m), true); assert(bOk);
		}
		else{
			bOk = move(board, _p2, *m, false); assert(bOk);
			points[i] = ai->searcher.heuristic.quickRateBoard(board, _p1, _p2.move(*m), PLAYER_1);
			bOk = move(board, _p2.move(*m), getOpositeDirection(*m), true); assert(bOk);
		}

		i++;
	}
	for (unsigned int i = 0; i < moves.size(); i++){
		for (unsigned int j = i + 1; j < moves.size(); j++){
			if (next == PLAYER_1){
				if (points[j] > points[i]){
					int temp = points[i];
					points[i] = points[j];
					points[j] = temp;

					temp = moves[i];
					moves[i] = moves[j];
					moves[j] = temp;
				}
			}
			else {
				if (points[j] < points[i]){
					int temp = points[i];
					points[i] = points[j];
					points[j] = temp;

					temp = moves[i];
					moves[i] = moves[j];
					moves[j] = temp;
				}
			}
		}
	}
}

int CHeuristicBase::pureTreeOfChamber(TBlock board[], const Pos2D &_p1, const Pos2D &_p2, const int next)
{
#ifdef _DEBUG
	TBlock backup[BOARD_SIZE];
	memcpy(backup, board, BOARD_SIZE*sizeof(TBlock));
#endif // _DEBUG
	TBlock gatesBoard[BOARD_SIZE];
	CBiconnectedComponents::getArticulationPoints(board, _p1, _p2, gatesBoard);

	// not isolated mode, not a leaf node
	assert(next == PLAYER_1 || next == PLAYER_2);
	assert(getBlock(board, _p1) == BLOCK_PLAYER_1 && getBlock(board, _p2) == BLOCK_PLAYER_2);
	int result = 0;
	int n1 = 0, n2 = 0;

	static TBlock board1[BOARD_SIZE], board2[BOARD_SIZE];
	memcpy(board1, board, BOARD_SIZE*sizeof(TBlock));
	memcpy(board2, board, BOARD_SIZE*sizeof(TBlock));
	vector<Pos1D> enemyOfP1, enemyOfP2;
	enemyOfP1.reserve(BOARD_SIZE);
	enemyOfP2.reserve(BOARD_SIZE);

	static TBlock dBoard1[BOARD_SIZE], dBoard2[BOARD_SIZE];
	memcpy(dBoard1, board, BOARD_SIZE*sizeof(TBlock));
	memcpy(dBoard2, board, BOARD_SIZE*sizeof(TBlock));
	fillDistance(dBoard1, _p1);
	fillDistance(dBoard2, _p2);
	for (int i = 0; i < BOARD_SIZE; i++){
		if (dBoard1[i] > SPECIAL_BLOCK && dBoard2[i] > SPECIAL_BLOCK){
			if (dBoard1[i] > dBoard2[i]) {
				board1[i] = BLOCK_ENEMY_AREA;
				enemyOfP1.push_back(i);
			}
			else if (dBoard1[i] < dBoard2[i]) {
				board2[i] = BLOCK_ENEMY_AREA;
				enemyOfP2.push_back(i);
			}
			else if (next == PLAYER_1) {
				board2[i] = BLOCK_ENEMY_AREA;
				enemyOfP2.push_back(i);
			}
			else {
				board1[i] = BLOCK_ENEMY_AREA;
				enemyOfP1.push_back(i);
			}
		}
	}
	board1[_p2] = BLOCK_ENEMY_AREA; enemyOfP1.push_back(_p2);
	board2[_p1] = BLOCK_ENEMY_AREA; enemyOfP2.push_back(_p1);

	static TBlock filledBoard1[BOARD_SIZE];
	fillChamberWithBattleFields(gatesBoard, board1, enemyOfP1, filledBoard1);
	int k1 = CBiconnectedComponents::getEstimatedLength(filledBoard1, _p1);

	static TBlock filledBoard2[BOARD_SIZE];
	fillChamberWithBattleFields(gatesBoard, board2, enemyOfP2, filledBoard2);
	int k2 = CBiconnectedComponents::getEstimatedLength(filledBoard2, _p2);

	int l1 = -1, l2 = -1;

	bool p1isInChamberWithBattleField = false;
	for (int i = 1; i <= 4; i++){
		if (getBlock(filledBoard1, _p1.move(i)) == BLOCK_ENEMY_AREA)
		{
			p1isInChamberWithBattleField = true;
			l1 = CBiconnectedComponents::getEstimatedLength(board1, _p1);
			break;
		}
	}

	bool p2isInChamberWithBattleField = false;
	for (int i = 1; i <= 4; i++){
		if (getBlock(filledBoard1, _p2.move(i)) == BLOCK_ENEMY_AREA)
		{
			p2isInChamberWithBattleField = true;
			l1 = CBiconnectedComponents::getEstimatedLength(board2, _p2);
			break;
		}
	}

	n1 = max(k1, l1);
	n2 = max(k2, l2);

	if (n1 + n2 == 0)
		return 0;

	DEBUG(cout << "n1/n2 = " << n1 << "/" << n2 << endl);
	result = n1 *(MAX_POINTS - MIN_POINTS) / (n1 + n2) / 2 + MIN_POINTS / 2;
#ifdef _DEBUG
	assert(memcmp(board, backup, BOARD_SIZE*sizeof(TBlock)) == 0);
#endif // _DEBUG

	return result;
}

int CHeuristicBase::pureTreeOfChamber2(TBlock board[], const Pos2D &_p1, const Pos2D &_p2, const int next)
{
#ifdef _DEBUG
	TBlock backup[BOARD_SIZE];
	memcpy(backup, board, BOARD_SIZE*sizeof(TBlock));
#endif // _DEBUG
	TBlock gatesBoard[BOARD_SIZE];
	CBiconnectedComponents::getArticulationPoints(board, _p1, _p2, gatesBoard);

	// not isolated mode, not a leaf node
	assert(next == PLAYER_1 || next == PLAYER_2);
	assert(getBlock(board, _p1) == BLOCK_PLAYER_1 && getBlock(board, _p2) == BLOCK_PLAYER_2);
	int result = 0;
	int n1 = 0, n2 = 0;

	static TBlock board1[BOARD_SIZE], board2[BOARD_SIZE];
	memcpy(board1, board, BOARD_SIZE*sizeof(TBlock));
	memcpy(board2, board, BOARD_SIZE*sizeof(TBlock));
	vector<Pos1D> enemyOfP1, enemyOfP2;
	enemyOfP1.reserve(BOARD_SIZE);
	enemyOfP2.reserve(BOARD_SIZE);

	static TBlock dBoard1[BOARD_SIZE], dBoard2[BOARD_SIZE];
	memcpy(dBoard1, board, BOARD_SIZE*sizeof(TBlock));
	memcpy(dBoard2, board, BOARD_SIZE*sizeof(TBlock));
	fillDistance(dBoard1, _p1);
	fillDistance(dBoard2, _p2);
	for (int i = 0; i < BOARD_SIZE; i++){
		if (dBoard1[i] > SPECIAL_BLOCK && dBoard2[i] > SPECIAL_BLOCK){
			if (dBoard1[i] > dBoard2[i]) {
				board1[i] = BLOCK_ENEMY_AREA;
				enemyOfP1.push_back(i);
			}
			else if (dBoard1[i] < dBoard2[i]) {
				board2[i] = BLOCK_ENEMY_AREA;
				enemyOfP2.push_back(i);
			}
			else if (next == PLAYER_1) {
				board2[i] = BLOCK_ENEMY_AREA;
				enemyOfP2.push_back(i);
			}
			else {
				board1[i] = BLOCK_ENEMY_AREA;
				enemyOfP1.push_back(i);
			}
		}
	}
	board1[_p2] = BLOCK_ENEMY_AREA; enemyOfP1.push_back(_p2);
	board2[_p1] = BLOCK_ENEMY_AREA; enemyOfP2.push_back(_p1);

	static TBlock filledBoard1[BOARD_SIZE];
	fillChamberWithBattleFields(gatesBoard, board1, enemyOfP1, filledBoard1);
	int k1 = CBiconnectedComponents::getEstimatedLength(filledBoard1, _p1);

	static TBlock filledBoard2[BOARD_SIZE];
	fillChamberWithBattleFields(gatesBoard, board2, enemyOfP2, filledBoard2);
	int k2 = CBiconnectedComponents::getEstimatedLength(filledBoard2, _p2);

	int l1 = -1, l2 = -1;

	bool p1isInChamberWithBattleField = false;
	for (int i = 1; i <= 4; i++){
		if (getBlock(filledBoard1, _p1.move(i)) == BLOCK_ENEMY_AREA)
		{
			p1isInChamberWithBattleField = true;
			l1 = CBiconnectedComponents::getEstimatedLength(board1, _p1);
			break;
		}
	}

	bool p2isInChamberWithBattleField = false;
	for (int i = 1; i <= 4; i++){
		if (getBlock(filledBoard1, _p2.move(i)) == BLOCK_ENEMY_AREA)
		{
			p2isInChamberWithBattleField = true;
			l1 = CBiconnectedComponents::getEstimatedLength(board2, _p2);
			break;
		}
	}

	n1 = max(k1, l1);
	n2 = max(k2, l2);

	if (n1 + n2 == 0)
		return 0;

	DEBUG(cout << "n1/n2 = " << n1 << "/" << n2 << endl);
	result = n1 *(MAX_POINTS - MIN_POINTS) / (n1 + n2) / 2 + MIN_POINTS / 2;
#ifdef _DEBUG
	assert(memcmp(board, backup, BOARD_SIZE*sizeof(TBlock)) == 0);
#endif // _DEBUG

	return result;
}