#include <iostream>
#include <vector>

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

void PrintBoard()
{
	system("cls");

	std::cout << "  ";

	//Print the top board values
	for (int x = 0; x < BOARD_SIZE; x++)
	{
		std::cout << x + 1 << " ";
	}

	std::cout << std::endl;

	for (int x = 0; x < BOARD_SIZE; x++)
	{
		//Print the side board values
		std::cout << x + 1 << " ";

		for (int y = 0; y < BOARD_SIZE; y++)
		{
			MOVE blockState = (MOVE)playSpace[x][y];

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

			std::cout << " ";
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

	for (int x = 0; x < BOARD_SIZE; x++)
	{
		for (int y = 0; y < BOARD_SIZE; y++)
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
	//4 directions we can go
	//Left, right, up, down
	// 

	//No direction, we're starting out
	if(xDir == 99 && yDir == 99)
	{
		Vec2 allDirs[]
		{
			Vec2(0, 1), //Down
			Vec2(0,- 1),	//Up
			Vec2(- 1, 0),	//Left
			Vec2(1, 0),	//Right
			Vec2(1, 1),	//Diag right down
			Vec2(1, - 1),	//Diag right up
			Vec2(-1, 1), //Diag left down
			Vec2(-1,- 1), //Diag left up
		};
		
		int highestAdj = 0;
		for (int i = 0; i < sizeof(allDirs) / sizeof(allDirs[0]); i++)
		{
			Vec2 next(start.x + allDirs[i].x, start.y + allDirs[i].y);

			if(!IsValid(next.x) || !IsValid(next.y))
			{
				continue;
			}

			if(playSpace[next.y][next.x] == type)
			{
				//Trace this route
				count++;
				TraceAdjacent(next, type, allDirs[i].x, allDirs[i].y, count);

				if(count >= BOARD_SIZE - 1)
				{
					return true;
				}

				if(count > highestAdj)
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


	//TODO fix this check	
	for(int x = 0; x < BOARD_SIZE; x++)
	{
		for(int y = 0; y < BOARD_SIZE; y++)
		{
			MOVE move = (MOVE)playSpace[y][x];

			if(move == MOVE::NONE)
			{
				continue;
			}

			Vec2 movePos = Vec2(x,y);

			int adjCount = 0;
			if (TraceAdjacent(movePos, move, 99, 99, adjCount))
			{
				return true;
			}

			//std::cout << "Pos X:" << x + 1 << " Y:" << y + 1 << " has max adjacent count : " << adjCount << std::endl;

		}
	}

	return false;	//TODO!
}

void PlayAIMove()
{
	//Find free space on the board
	//Place an O

	std::vector<Vec2> options;

	for (int x = 0; x < BOARD_SIZE; x++)
	{
		for (int y = 0; y < BOARD_SIZE; y++)
		{
			Vec2 v2 = Vec2(x, y);

			if (!IsValidVec2(v2))
			{
				continue;
			}

			options.push_back(v2);
		}
	}

	if(options.size() == 0)
	{
		return; //TODO concede;
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

		return;
	}

	//Pick random from the priority vec
	Vec2* move = priority.size() > 1 ? &priority[rand() % (priority.size() - 1)]
		: &priority[0];

	PlayVec2(move, MOVE::O);
}

int GetValidIntInput(std::string promptStr)
{
	std::cout << promptStr << " ";

	while (true)
	{
		int input = getchar();

		if (input != EOF && input != '\n')
		{
			int ret = input - '0';

			ret--; //User input of 1 == 0 internal index 
			if (IsValid(ret))
			{
				return ret;
			}

			PrintBoard();

			std::cout << "Invalid input!" << std::endl;

			std::cout << promptStr << " ";
		}
	}
}

void GameLoop()
{
	PlayAIMove();

	PrintBoard();

	//Take X Input
	//Take Y input
	//Plot (if pos is valid)
	//Run AI move
	//Evaluate game
	//If no winner, go again
	int x = -1, y = -1;

	x = GetValidIntInput("Enter X pos:");
	y = GetValidIntInput("Enter Y pos:");

	Vec2 v2(x, y);

	if(!IsValidVec2(v2))
	{
		std::cout << "Invalid input!" << std::endl;
		while (getchar() != EOF && getchar() != '\n') {};

		return;
	}

	playSpace[y][x] = MOVE::X;


	PrintBoard();
}

int main()
{
	//Zero out array
	for (int x = 0; x < BOARD_SIZE; x++)
	{
		for (int y = 0; y < BOARD_SIZE; y++)
		{
			playSpace[y][x] = 0;
		}
	}

	while (!GameOver())
	{
		GameLoop();
	}
}