#include "EffectCave.h"

#include "../utils/TextUtils.h"
#include "Player.h"
#include <iostream>

EffectCave::EffectCave(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, short board[10][10]): EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
	this->board = new TSquare[MAX_SQUARES * MAX_SQUARES];
	this->player = 0;
	this->startSquare = NULL;
	this->exitSquare = NULL;
	for (int i = 0; i < MAX_SQUARES; i++)
	{
		for (int j = 0; j < MAX_SQUARES; j++)
		{
			int k = i * MAX_SQUARES + j;

			this->board[k].i = i;
			this->board[k].j = j;
			this->board[k].isWall = false;
			this->board[k].isMonster = false;

			switch (board[i][j])
			{
			case 0: // empty
				break;
			case 4: // exit
				this->exitSquare = &this->board[k];
				break;
			case 3: // monster
				this->board[k].isMonster = true;
				break;
			case 2: // start
				this->startSquare = &this->board[k];
				break;
			case 1: // wall
				this->board[k].isWall = true;
				break;
			}
		}
	}

	player = new Player();
}

EffectCave::~EffectCave()
{
	delete[] board;
	delete player;
}

void EffectCave::init()
{
	player->square = startSquare;
	player->direction = TDirection::NORTH;
}

void EffectCave::update(float deltaTime)
{
	;
}

void EffectCave::onKeyPressed(SDL_Scancode key)
{
	if (key == SDL_SCANCODE_W)
	{
		std::cout << "palante" << std::endl;
		TSquare* nextSquare = getNextSquare(player->square, player->direction);

		if (nextSquare->isWall)
		{
			onWallHit();
		}
		else
		{
			updateEnvironment();
			player->square = nextSquare;
			onStep();

			if (nextSquare->isMonster)
			{
				onMonsterHit();
				//init();
			}
			else if (nextSquare == exitSquare)
			{
				onExitReached();
			}
		}

	}
	if (key == SDL_SCANCODE_D)
	{
		std::cout << "derecha" << std::endl;
		player->turnRight();
		onStep();
	}
	if (key == SDL_SCANCODE_A)
	{
		std::cout << "izquierda" << std::endl;
		player->turnLeft();
		onStep();
	}
}

void EffectCave::render()
{
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
	SDL_Color fg = { 0xFF,0xFF,0xFF }, bg = { 0x00,0x00,0x00 };
	int x0 = 100;
	int y0 = 100;
	int dx = 30;
	int dy = 30;
	char str[10];
	for (int i = 0; i < MAX_SQUARES; i++)
	{
		for (int j = 0; j < MAX_SQUARES; j++)
		{
			char d1 = ' ';
			char d2 = ' ';

			int k = i * MAX_SQUARES + j;

			if (player->square == &this->board[k])
			{
				d1 = 'P';
				switch (player->direction)
				{
				case TDirection::NORTH:
					d2 = 'N';
					break;
				case TDirection::WEST:
					d2 = 'W';
					break;
				case TDirection::SOUTH:
					d2 = 'S';
					break;
				case TDirection::EAST:
					d2 = 'E';
					break;
				}
			}
			else
			{
				if (this->board[k].isWall)
				{
					d1 = 'X';
					d2 = 'X';
				}

				if (this->board[k].isMonster)
				{
					d1 = 'M';
				}
				
				if (&this->board[k] == startSquare)
				{
					d1 = 'S';
				}
				if (&this->board[k] == exitSquare)
				{
					d1 = 'I';
					d2 = 'T';
				}
			}

			sprintf_s(str, 10, "%c%c", d1,d2);
			int x = x0 + j * dx;
			int y = y0 + i * dy;
			TextUtils::drawText(surface, str, 12, x, y, fg, bg);
		}
	}
}

TSquare* EffectCave::getNextSquare(TSquare* initialSquare, TDirection direction)
{
	if (initialSquare != NULL)
	{
		int i = initialSquare->i;
		int j = initialSquare->j;

		switch (direction)
		{
		case TDirection::NORTH:
			i--;
			break;
		case TDirection::SOUTH:
			i++;
			break;
		case TDirection::EAST:
			j++;
			break;
		case TDirection::WEST:
			j--;
			break;
		}

		if (0 <= i && i < MAX_SQUARES && 0 <= j && j < MAX_SQUARES)
		{
			return &this->board[i * MAX_SQUARES + j];
		}
	}
	return NULL;
}

void EffectCave::onWallHit()
{
	std::cout << "wall" << std::endl;
}

void EffectCave::onExitReached()
{
	std::cout << "exit yay" << std::endl;
}

void EffectCave::onMonsterHit()
{
	std::cout << "game over" << std::endl;
}

void EffectCave::onStep()
{
	std::cout << "tap" << std::endl;
}

void EffectCave::updateEnvironment()
{
	;
}
