#include <iostream>
#include <vector>
#include <iostream>
#include <sstream>
#include <conio.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int* playSpace;

#define MIN_BOARD_SIZE 3

#define RETURN_KEY 13
#define ARROW_BEGIN 224
#define ARROW_UP 72
#define ARROW_DOWN 80
#define ARROW_LEFT 75
#define ARROW_RIGHT 77

//#define PRINT_GRID
#define BEEP

int cursorX = 0, cursorY = 0;
int boardSize = MIN_BOARD_SIZE;

enum MOVE
{
	NONE = 0,
	X,
	O
};

enum class ADJACENT_TYPE
{
	NONE,
	DIAGONAL,
	STRAIGHT
};

struct Vec2
{
public:
	int x;
	int y;

	Vec2(int X, int Y)
	{
		x = X;
		y = Y;
	}
};

size_t BoardIndex(int x, int y)
{
	return x + boardSize * y;
}

std::string DynamicSpace(int input)
{
	char buf[256];

	sprintf_s(buf, "%d", input);
	std::string ret;

	for (int i = 0; i < strlen(buf); i++)
	{
		ret.append(" ");
	}

	return ret;
}

std::string GetSpaceFromHighestVal(int currVal)
{
	char bufA[256];
	char bufB[256];

	sprintf_s(bufA, "%d", boardSize);
	sprintf_s(bufB, "%d", currVal);
	std::string ret;

	for (int i = 0; i < 1 + (strlen(bufA) - strlen(bufB)); i++)
	{
		ret.append(" ");
	}

	if (ret.length() + strlen(bufB) <= 2)
	{
		ret.clear();
		int amount = 3 - strlen(bufB);

		for (int i = 0; i < amount; i++)
		{
			ret.append(" ");
		}
	}

	return ret;
}

void PrintBoard()
{
	system("cls");

#ifdef PRINT_GRID
	std::cout << "  X ";

	//Print the top board values
	for (int x = 0; x < boardSize; x++)
	{
		std::cout << x + 1 << "  ";
	}

	std::cout << std::endl;
	std::cout << "Y ";
#endif

	std::cout << std::endl;

	for (int y = 0; y < boardSize; y++)
	{
		//Print the side board values
#ifdef PRINT_GRID
		std::cout << y + 1 << GetSpaceFromHighestVal(y + 1);
#else
		std::cout << " ";
#endif
		for (int x = 0; x < boardSize; x++)
		{
			MOVE blockState = (MOVE)playSpace[BoardIndex(x, y)];
			bool isCursor = (x == cursorX && y == cursorY);

			std::cout << (isCursor ? "[" : " ");

			switch (blockState)
			{
				case MOVE::NONE:
				{
					std::cout << "-";

					break;
				}
				case MOVE::X:
				{
					std::cout << "X";
					break;
				}
				case MOVE::O:
				{
					std::cout << "O";

					break;
				}
			}

#ifdef PRINT_GRID
			std::string dynamicSpace = DynamicSpace(x + 2);

			if (isCursor)
			{
				dynamicSpace.pop_back(); //Remove a space as we have a ]
				std::cout << "]";

			}

			std::cout << dynamicSpace;
#else
			std::cout << (isCursor ? "]" : " ");
#endif
		}

		std::cout << std::endl;
	}
}

ADJACENT_TYPE GetAdjacentType(Vec2& a, Vec2& b)
{
	if (abs(a.x - b.x) == 1)
	{
		return ADJACENT_TYPE::STRAIGHT;
	}

	return ADJACENT_TYPE::DIAGONAL;
}

//x/x///
////x///
//////x/
////////

bool IsAdjacent(Vec2& a, Vec2& b)
{
	return (abs(a.x - b.x) == 1 || abs(a.y - b.y) == 1)
		&& !(a.x == b.x && a.y == b.y);
}

std::vector<Vec2> GetExistingMoves(MOVE type)
{
	std::vector<Vec2> ret;

	for (int y = 0; y < boardSize; y++)
	{
		for (int x = 0; x < boardSize; x++)
		{
			if (playSpace[BoardIndex(x, y)] == type)
			{
				ret.push_back(Vec2(x, y));
			}
		}
	}

	return ret;
}

void PlayVec2(Vec2* v2, MOVE type)
{
	playSpace[BoardIndex(v2->x, v2->y)] = type;
}

bool IsValid(int pos)
{
	return pos >= 0 && pos < boardSize;
}

bool IsValidVec2(Vec2& v2)
{
	if (!IsValid(v2.x) || !IsValid(v2.y))
	{
		return false;
	}

	MOVE m = (MOVE)playSpace[BoardIndex(v2.x, v2.y)];

	return m == MOVE::NONE;
}

bool TraceAdjacent(Vec2 start, MOVE type, Vec2* dir, int& count)
{
	//No direction, we're starting out
	if (!dir)
	{
		Vec2 allDirs[]
		{
			Vec2(0, 1), //Down
			Vec2(0,-1),	//Up
			Vec2(-1, 0),	//Left
			Vec2(1, 0),	//Right
			Vec2(1, 1),	//Diag right down
			Vec2(1, -1),	//Diag right up
			Vec2(-1, 1), //Diag left down
			Vec2(-1,-1), //Diag left up
		};

		int highestAdj = 0;
		for (int i = 0; i < sizeof(allDirs) / sizeof(allDirs[0]); i++)
		{
			Vec2 next(start.x + allDirs[i].x, start.y + allDirs[i].y);

			if (!IsValid(next.x) || !IsValid(next.y))
			{
				continue;
			}

			if (playSpace[BoardIndex(next.x, next.y)] == type)
			{
				//We're not leaving scope so we can pass a stack ptr
				Vec2 newDir(allDirs[i].x, allDirs[i].y);

				//Trace this route
				count++;
				TraceAdjacent(next, type, &newDir, count);

				if (count >= boardSize - 1)
				{
					return true;
				}

				if (count > highestAdj)
				{
					highestAdj = count;
				}

				count = 0;
			}
		}

		count = highestAdj;
	}
	else
	{
		Vec2 next(start.x + dir->x, start.y + dir->y);
		if (!IsValid(next.x) || !IsValid(next.y))
		{
			return false;
		}

		if (playSpace[BoardIndex(next.x, next.y)] == type)
		{
			//Trace this route
			count++;
			TraceAdjacent(next, type, dir, count);
		}
	}

	return false;
}

bool GameOver(MOVE& winningMove)
{
	//See if BOARD_SIZE amount of same type is adjacent
	bool remainingMoves = false;

	for (int x = 0; x < boardSize; x++)
	{
		for (int y = 0; y < boardSize; y++)
		{
			MOVE move = (MOVE)playSpace[BoardIndex(x, y)];

			if (move == MOVE::NONE)
			{
				remainingMoves = true;
				continue;
			}

			Vec2 movePos = Vec2(x, y);

			int adjCount = 0;
			if (TraceAdjacent(movePos, move, nullptr, adjCount))
			{
				winningMove = move;
				return true;
			}

			//std::cout << "Pos X:" << x + 1 << " Y:" << y + 1 << " has max adjacent count : " << adjCount << std::endl;
		}
	}

	return !remainingMoves;
}

bool RemainingMoves()
{
	for (int x = 0; x < boardSize; x++)
	{
		for (int y = 0; y < boardSize; y++)
		{
			MOVE move = (MOVE)playSpace[BoardIndex(x, y)];

			if (move == MOVE::NONE)
			{
				return true;
			}
		}
	}

	return false;
}

bool PlayAIMove()
{
	//Find free space on the board
	//Place an O
	//Returns false when conceding

	std::vector<Vec2> options;

	for (int y = 0; y < boardSize; y++)
	{
		for (int x = 0; x < boardSize; x++)
		{
			Vec2 v2 = Vec2(x, y);

			if (!IsValidVec2(v2))
			{
				continue;
			}

			options.push_back(v2);
		}
	}

	if (options.size() == 0)
	{
		return false;
	}

	std::vector<Vec2> priority;

	std::vector<Vec2> existing = GetExistingMoves(MOVE::O);

	for (int i = 0; i < options.size(); i++)
	{
		Vec2* choice = &options[i];

		for (int j = 0; j < existing.size(); j++)
		{
			//Prioritse adjacent moves
			if (IsAdjacent(*choice, existing[j]))
			{
				priority.push_back(*choice);
			}
		}
	}

	if (priority.size() == 0)
	{
		//Choose a random one from options

		Vec2* move = options.size() > 1 ? &options[rand() % (options.size())]
			: &options[0];
		PlayVec2(move, MOVE::O);

	}
	else
	{
		//Pick random from the priority vec
		Vec2* move = priority.size() > 1 ? &priority[rand() % (priority.size())]
			: &priority[0];

		PlayVec2(move, MOVE::O);
	}

	return true;
}

//int GetValidIntInput(std::string promptStr)
//{
//	std::cout << promptStr << " ";
//	std::vector<int> inputList;
//
//	while (true)
//	{
//		int input = getchar();
//
//		if (input != EOF && input != '\n')
//		{
//			int ret = input - '0';
//			inputList.push_back(ret);
//		}
//		else
//		{
//			//Convert to full int
//			std::stringstream ss;
//			for (int i : inputList)
//			{
//				ss << i;
//			}
//
//			int ret = atoi(ss.str().c_str());
//			ret--; //User input of 1 == 0 internal index 
//
//			if (IsValid(ret))
//			{
//				return ret;
//			}
//			else
//			{
//				std::cout << "Invalid input!" << std::endl;
//				std::cout << promptStr << " ";
//				inputList.clear();
//			}
//		}
//	}
//}

//Returns true when enter is pressed
bool RunCursor()
{
	int input = _getch();

	if (input == RETURN_KEY)
	{
		return true;
	}

	if (input == ARROW_BEGIN)
	{
		input = _getch();

		int potentialX = cursorX;
		int potentialY = cursorY;

		switch (input)
		{
			case ARROW_UP:
			{
				potentialY--;
				break;
			}
			case ARROW_DOWN:
			{
				potentialY++;
				break;
			}
			case ARROW_LEFT:
			{
				potentialX--;
				break;
			}
			case ARROW_RIGHT:
			{
				potentialX++;
				break;
			}
		}

		if (IsValid(potentialX) && IsValid(potentialY))
		{
			cursorX = potentialX;
			cursorY = potentialY;

			PrintBoard();
		}
	}

	return false;
}

bool CheckGameOver()
{
	MOVE winningMove = MOVE::NONE;
	if (GameOver(winningMove))
	{
		switch (winningMove)
		{
			case MOVE::NONE:
			{
				std::cout << "It's a draw!" << std::endl;

#ifdef BEEP
				Beep(500, 150);
				Beep(800, 200);
				Beep(650, 600);
#endif
				break;
			}
			case MOVE::O:
			{
				std::cout << "You've lost! Better luck next time..." << std::endl;
#ifdef BEEP
				Beep(800, 200);
				Beep(600, 200);
				Beep(580, 200);
				Beep(500, 800);
#endif
				break;
			}
			case MOVE::X:
			{
				std::cout << "You've won! Congratulations!" << std::endl;

#ifdef BEEP
				Beep(500, 120);
				Beep(600, 120);
				Beep(700, 120);
				Beep(800, 300);
				Beep(650, 120);
				Beep(800, 800);
#endif
				break;
			}
		}

		return true;
	}

	return false;
}

void AllocBoard(int size)
{
	playSpace = new int[size * size];
}

bool BoardAllocated()
{
	return playSpace;
}

int main(int argc, char** argv)
{
	//Get the board size from commnad line args
	for (int i = 0; i < argc; i++)
	{
		if (!strncmp(*argv, "-boardsize", 10))
		{
			char* sizeStart = strchr(*argv, '=');

			if (sizeStart && sizeStart + 1)
			{
				++sizeStart;

				std::string boardSizeStr;

				while (*sizeStart != '\0')
				{
					boardSizeStr.append(sizeStart);
					++sizeStart;
				}

				boardSizeStr.pop_back();
				int finalSize = atoi(boardSizeStr.c_str());
				boardSize = finalSize;

				break;
			}
		}

		++argv;
	}

	if (boardSize < MIN_BOARD_SIZE)
	{
		boardSize = MIN_BOARD_SIZE;
	}

	AllocBoard(boardSize);

	if (!BoardAllocated())
	{
		return -1;
	}

	srand(time(NULL));

	//Zero out array
	for (int x = 0; x < boardSize; x++)
	{
		for (int y = 0; y < boardSize; y++)
		{
			playSpace[BoardIndex(x, y)] = 0;
		}
	}

	while (true)
	{
		PlayAIMove();
		PrintBoard();
		if (CheckGameOver())
		{
			break;
		}

		std::cout << std::endl;

		while (true)
		{
			if (RunCursor())
			{
				Vec2 v2(cursorX, cursorY);

				if (IsValidVec2(v2))
				{
					break;
				}

#ifdef BEEP
				Beep(600, 100);
#endif
			}
		}

		playSpace[BoardIndex(cursorX, cursorY)] = MOVE::X;

		PrintBoard();
		if (CheckGameOver())
		{
			break;
		}
	}

	if (playSpace)
	{
		delete[] playSpace;
	}

	return 0;
}