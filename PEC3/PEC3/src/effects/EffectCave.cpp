#include "EffectCave.h"

#include <SDL_mixer.h>
#include "../utils/TextUtils.h"
#include "Player.h"
#include <iostream>

Mix_Chunk* stepSound;
Mix_Chunk* crashSound;

EffectCave::EffectCave(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, short board[10][10]): EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
	this->previousResult = TResult::RS_NONE;

	this->isGameOver = false;
	this->renderDebug = false;
	this->board = new TSquare[MAX_SQUARES * MAX_SQUARES];
	this->player = 0;
	this->startSquare = NULL;
	this->exitSquare = NULL;
	this->monsterSquare = NULL;
	this->originalMonsterSquare = NULL;

	for (int i = 0; i < MAX_SQUARES; i++)
	{
		for (int j = 0; j < MAX_SQUARES; j++)
		{
			int k = i * MAX_SQUARES + j;

			this->board[k].i = i;
			this->board[k].j = j;
			this->board[k].isWall = false;

			switch (board[i][j])
			{
			case 0: // empty
			default:
				break;
			case 4: // exit
				this->exitSquare = &this->board[k];
				break;
			case 3: // monster
				originalMonsterSquare = &this->board[k];
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

	stepSound = Mix_LoadWAV("assets/footstep.wav");
	crashSound = Mix_LoadWAV("assets/crash.wav");
}

EffectCave::~EffectCave()
{
	//SDL_FreeWAV(stepSound);

	delete[] board;
	delete player;
}

void EffectCave::init()
{
	this->isGameOver = false;
	player->square = startSquare;
	player->direction = TDirection::NORTH;
	monsterSquare = originalMonsterSquare;
}

void EffectCave::update(float deltaTime)
{
	;
}

void EffectCave::onKeyPressed(SDL_Scancode key)
{
	if (key == SDL_SCANCODE_W && !isGameOver)
	{
		TSquare* nextSquare = getNextSquare(player->square, player->direction);

		if (nextSquare->isWall)
		{
			onWallHit();
		}
		else
		{
			player->square = nextSquare;
			onStep();

			if (nextSquare == monsterSquare)
			{
				onMonsterHit();
			}
			else if (nextSquare == exitSquare)
			{
				onExitReached();
			}
		}
		
		updateEnvironment();
	}
	if (key == SDL_SCANCODE_D && !isGameOver)
	{
		player->turnRight();
		onStep();
	}
	if (key == SDL_SCANCODE_A && !isGameOver)
	{
		player->turnLeft();
		onStep();
	}

	if (key == SDL_SCANCODE_T && !isGameOver)
	{
		renderDebug = !renderDebug;
	}

	if (key == SDL_SCANCODE_SPACE)
	{
		if (isGameOver)
		{
			init();
		}
	}
}

void EffectCave::render()
{
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
	
	if (isGameOver)
	{
		SDL_Color fg = { 0xFF,0xFF,0xFF }, bg = { 0x00,0x00,0x00 };
		
		switch(previousResult)
		{
		case TResult::RS_EXIT:
			TextUtils::drawText(surface, "You escaped!!", 12, 50, 70, fg, bg);
			break;
		case TResult::RS_MONSTER:
			TextUtils::drawText(surface, "The monster ate you!!", 12, 50, 70, fg, bg);
			break;
		case TResult::RS_NONE:
		default:
			break;
		}
		
		TextUtils::drawText(surface, "Press SPACE to start", 12, 50, 100, fg, bg);
	}
	else
	{
		if (renderDebug)
		{
			renderDebugGraphics();
		}
	}
}

void EffectCave::renderDebugGraphics()
{
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
				default:
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

				if (&this->board[k] == monsterSquare)
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

			sprintf_s(str, 10, "%c%c", d1, d2);
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
		default:
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
	Mix_PlayChannel(-1, crashSound, 0);

	moveMonster();
}

TDirection EffectCave::intToDirection(int n)
{
	switch (n)
	{
	case 0:
		return TDirection::NORTH;
		break;
	case 1:
		return TDirection::EAST;
		break;
	case 2:
		return TDirection::SOUTH;
		break;
	case 3:
	default:
		return TDirection::WEST;
		break;
	}
}

void EffectCave::moveMonster()
{
	int move = rand() % 4;

	TSquare* nextMonsterSquare = NULL;
	int k = 0;
	while (k < 4 && (nextMonsterSquare == NULL || nextMonsterSquare->isWall))
	{
		nextMonsterSquare = getNextSquare(monsterSquare, intToDirection((move + k) % 4));
		k++;
	}

	if (nextMonsterSquare != NULL)
	{
		monsterSquare = nextMonsterSquare;

		if (monsterSquare == player->square)
		{
			onMonsterHit();
		}
	}
}

void EffectCave::onExitReached()
{
	this->previousResult = TResult::RS_EXIT;
	std::cout << "exit yay" << std::endl;
	gameOver();
}

void EffectCave::onMonsterHit()
{
	this->previousResult = TResult::RS_MONSTER;
	std::cout << "game over" << std::endl;
	gameOver();
}

void EffectCave::onStep()
{
	std::cout << "tap" << std::endl;
	Mix_PlayChannel(-1, stepSound, 0);
}

void EffectCave::updateEnvironment()
{
	;
}

void EffectCave::gameOver()
{
	this->isGameOver = true;
}
