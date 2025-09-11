#include <iostream>
#include <vector>
#include <iostream>
#include <sstream>

#define BOARD_SIZE 3
int playSpace[BOARD_SIZE][BOARD_SIZE];

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

	sprintf_s(bufA, "%d", BOARD_SIZE);
	sprintf_s(bufB, "%d", currVal);
	std::string ret;

	for (int i = 0; i < 2 + (strlen(bufA) - strlen(bufB)); i++)
	{
		ret.append(" ");
	}

	if (ret.length() + strlen(bufB) <= 3)
	{
		ret.clear();
		int amount = 4 - strlen(bufB);

		for(int i = 0; i < amount; i++)
		{
			ret.append(" ");
		}
	}

	return ret;
}

void PrintBoard()
{
	system("cls");

	std::cout << "  X ";

	//Print the top board values

	for (int x = 0; x < BOARD_SIZE; x++)
	{
		std::cout << x + 1 << " ";
	}

	std::cout << std::endl;
	std::cout << "Y ";
	std::cout << std::endl;

	for (int y = 0; y < BOARD_SIZE; y++)
	{
		//Print the side board values
		std::cout << y + 1 << GetSpaceFromHighestVal(y + 1);

		for (int x = 0; x < BOARD_SIZE; x++)
		{
			MOVE blockState = (MOVE)playSpace[y][x];
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

			std::cout << DynamicSpace(x + 2);
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
	return (abs(a.x - b.x) == 1 || abs(a.y - b.y == 1))
		&& !(a.x == b.x && a.y == b.y);
}

std::vector<Vec2> GetExistingMoves(MOVE type)
{
	std::vector<Vec2> ret;

	for (int y = 0; y < BOARD_SIZE; y++)
	{
		for (int x = 0; x < BOARD_SIZE; x++)
		{
			if (playSpace[y][x] == type)
			{
				ret.push_back(Vec2(x, y));
			}
		}
	}

	return ret;
}

void PlayVec2(Vec2* v2, MOVE type)
{
	playSpace[v2->y][v2->x] = type;
}

bool IsValid(int pos)
{
	return pos >= 0 && pos < BOARD_SIZE;
}

bool IsValidVec2(Vec2& v2)
{
	if (!IsValid(v2.x) || !IsValid(v2.y))
	{
		return false;
	}

	MOVE m = (MOVE)playSpace[v2.y][v2.x];

	return m == MOVE::NONE;
}

bool TraceAdjacent(Vec2 start, MOVE type, int xDir, int yDir, int& count)
{
	//No direction, we're starting out
	if (xDir == 99 && yDir == 99)
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

			if (playSpace[next.y][next.x] == type)
			{
				//Trace this route
				count++;
				TraceAdjacent(next, type, allDirs[i].x, allDirs[i].y, count);

				if (count >= BOARD_SIZE - 1)
				{
					return true;
				}

				if (count > highestAdj)
				{
					highestAdj = count;
				}

				//If we're now going in the opposite dir, don't reset the count as it's still adjacent
				if (i + 1 < sizeof(allDirs[0]))
				{
					Vec2 v = allDirs[i + 1];

					if (v.x == -next.x && v.y == -next.y)
					{
						continue;
					}
				}

				count = 0;
			}
		}

		count = highestAdj;
	}
	else
	{
		Vec2 next(start.x + xDir, start.y + yDir);
		if (!IsValid(next.x) || !IsValid(next.y))
		{
			return false;
		}

		if (playSpace[next.y][next.x] == type)
		{
			//Trace this route
			count++;
			TraceAdjacent(next, type, xDir, yDir, count);
		}
	}

	return false;
}

bool GameOver()
{
	//See if BOARD_SIZE amount of same type is adjacent

	for (int x = 0; x < BOARD_SIZE; x++)
	{
		for (int y = 0; y < BOARD_SIZE; y++)
		{
			MOVE move = (MOVE)playSpace[y][x];

			if (move == MOVE::NONE)
			{
				continue;
			}

			Vec2 movePos = Vec2(x, y);

			int adjCount = 0;
			if (TraceAdjacent(movePos, move, 99, 99, adjCount))
			{
				return true;
			}

			//std::cout << "Pos X:" << x + 1 << " Y:" << y + 1 << " has max adjacent count : " << adjCount << std::endl;
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

	for (int y = 0; y < BOARD_SIZE; y++)
	{
		for (int x = 0; x < BOARD_SIZE; x++)
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

		Vec2* move = options.size() > 1 ? &options[rand() % (options.size() - 1)]
			: &options[0];
		PlayVec2(move, MOVE::O);

	}
	else
	{
		//Pick random from the priority vec
		Vec2* move = priority.size() > 1 ? &priority[rand() % (priority.size() - 1)]
			: &priority[0];

		PlayVec2(move, MOVE::O);
	}

	return true;
}

int GetValidIntInput(std::string promptStr)
{
	std::cout << promptStr << " ";
	std::vector<int> inputList;

	while(true)
	{
		int input = getchar();

		if (input != EOF && input != '\n')
		{
			int ret = input - '0';
			inputList.push_back(ret);
		}
		else
		{
			//Convert to full int
			std::stringstream ss;
			for (int i : inputList)
			{
				ss << i;
			}

			int ret = atoi(ss.str().c_str());
			ret--; //User input of 1 == 0 internal index 

			if (IsValid(ret))
			{
				return ret;
			}
			else
			{
				std::cout << "Invalid input!" << std::endl;
				std::cout << promptStr << " ";
				inputList.clear();
			}
		}
	}
}

int main()
{
	srand(time(NULL));

	//Zero out array
	for (int x = 0; x < BOARD_SIZE; x++)
	{
		for (int y = 0; y < BOARD_SIZE; y++)
		{
			playSpace[y][x] = 0;
		}
	}

	while (true)
	{
		bool aiResult = PlayAIMove();

		PrintBoard();

		if (GameOver() || !aiResult)
		{
			//AI has won
			std::cout << "You've lost! Better luck next time..." << std::endl;
			break;
		}

		//Take X Input
		//Take Y input
		//Plot (if pos is valid)
		//Run AI move
		//Evaluate game
		//If no winner, go again
		int x = -1, y = -1;

		std::cout << std::endl;

		while(true)
		{
			x = GetValidIntInput("Enter X pos:");
			y = GetValidIntInput("Enter Y pos:");
			Vec2 v2(x, y);

			if (IsValidVec2(v2))
			{
				break;
			}

			std::cout << "Invalid input!" << std::endl;
		}

		playSpace[y][x] = MOVE::X;
		PrintBoard();

		if (GameOver())
		{
			//Player has won
			std::cout << "You've won! Congratulations!" << std::endl;
			break;
		}
	}

	return 0;
}