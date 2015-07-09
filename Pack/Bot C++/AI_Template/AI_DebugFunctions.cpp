#include "MyAI.h"
#include "mydefine.h"


//////////////////////////////////////////////////////////////////////////
//			DEBUG FUNCTIONS
//////////////////////////////////////////////////////////////////////////
void CMyAI::printBoard(const int board[], const bool special)
{
#if SHOW_DEBUG_INFORMATION
	static std::string line;

	for (int y = 0; y < MAP_SIZE; y++) {
		line = "| ";
		for (int x = 0; x < MAP_SIZE; x++){
			int block = board[CC(x, y)];
			if (special)
				if (block >= SPECIAL_BLOCK)
					cout << setfill(' ') << setw(5) << block - SPECIAL_BLOCK << "|";
				else
					cout << setfill(' ') << setw(5) << "-----" << "|";
			else
				cout << setfill(' ') << setw(5) << block << "|";
		}
		cout << endl;
	}
	cout << endl;
#endif
}

// this method create a random new game in the board.
// this method does not allocate memory for board
// TODO: implement the noRandomMove
void CMyAI::createNewBoard(int board[], int nObstacle)
{
	memset(board, 0, BOARD_SIZE*sizeof(int));

	int nObject = 0;
	while (nObject < 5){
		int random;
		do {
			random = rand() % 121;
		} while (random == 0 || random == 120 || random == 60 || board[random] != BLOCK_EMPTY);
		nObject++;
		board[random] = BLOCK_OBSTACLE;
		board[120 - random] = BLOCK_OBSTACLE;
	}
	board[0] = BLOCK_PLAYER_1;
	board[BOARD_SIZE - 1] = BLOCK_PLAYER_2;

	int count = 0;
	while (count < nObstacle){
		int random = rand() % 121;
		if (board[random] == BLOCK_EMPTY){
			board[random] = BLOCK_OBSTACLE;
			count++;
		}
	}
}
#ifdef OPENCV

cv::Mat CMyAI::toImage(int board[], bool special)
{
	static cv::Mat result;
	result = cv::Mat(11 * PIXEL_PER_BLOCK, 11 * PIXEL_PER_BLOCK, CV_8UC3);
	static cv::Mat *target;

	for (int i = 0; i < BOARD_SIZE; i++) {
		cv::Mat newMat = Mat(PIXEL_PER_BLOCK - HALF_PIXEL_PER_LINE * 2, PIXEL_PER_BLOCK - HALF_PIXEL_PER_LINE * 2,
			CV_8UC3, Scalar(50, 50, 50));
		int x = i % MAP_SIZE;
		int y = i / MAP_SIZE;
		int block = board[i];
		switch (block) {
		case BLOCK_EMPTY:
			target = &empty;
			break;
		case BLOCK_OBSTACLE:
			target = &obstacle;
			break;
		case BLOCK_PLAYER_1:
			target = &player_1;
			break;
		case BLOCK_PLAYER_1_TRAIL:
			target = &player_1_trail;
			break;
		case BLOCK_PLAYER_2:
			target = &player_2;
			break;
		case BLOCK_PLAYER_2_TRAIL:
			target = &player_2_trail;
			break;
		default:
			target = &newMat;
			putText(result, std::to_string(block - SPECIAL_BLOCK), cvPoint(3, 3),
				FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
		}
		target->copyTo(result.rowRange(PIXEL_PER_BLOCK*y, PIXEL_PER_BLOCK*(y + 1)).
			colRange(PIXEL_PER_BLOCK*x, PIXEL_PER_BLOCK*(x + 1)));
	}
	return result;
}
#endif // OPENCV
