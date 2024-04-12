#include "EffectCave.h"

#include <SDL_mixer.h>
#include "../utils/TextUtils.h"
#include "Player.h"
#include <iostream>

Mix_Chunk* stepSound;
Mix_Chunk* crashSound;
Mix_Chunk* eatingSound;

Mix_Music* gameOverMusic;
Mix_Music* exitMusic;
Mix_Chunk* waterfallSound;
Mix_Chunk* snoreSound;

int snoreChannel = 0;
int waterfallChannel = 0;

EffectCave::EffectCave(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, short board[10][10]): EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
	this->previousResult = TResult::RS_NONE;

	this->isGameOver = false;
	this->isEnding = false;
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

	stepSound = Mix_LoadWAV("assets/audio/footstep.wav");
	crashSound = Mix_LoadWAV("assets/audio/crash.wav");
	eatingSound = Mix_LoadWAV("assets/audio/zombies-eating.wav");
	
	gameOverMusic = Mix_LoadMUS("assets/audio/game-over-music.mp3");
	exitMusic = Mix_LoadMUS("assets/audio/exit-music.mp3");

	waterfallSound = Mix_LoadWAV("assets/audio/exit-waterfall.wav");
	snoreSound = Mix_LoadWAV("assets/audio/monster-snore.wav");
}

EffectCave::~EffectCave()
{
	Mix_FreeChunk(stepSound);
	Mix_FreeChunk(crashSound);
	Mix_FreeChunk(eatingSound);
	Mix_FreeChunk(waterfallSound);
	Mix_FreeChunk(snoreSound);
	Mix_FreeMusic(gameOverMusic);
	Mix_FreeMusic(exitMusic);
	
	delete[] board;
	delete player;
}

void EffectCave::init()
{
	this->isGameOver = false;
	this->isEnding = false;
	player->square = startSquare;
	player->direction = TDirection::NORTH;
	monsterSquare = originalMonsterSquare;

	snoreChannel = Mix_PlayChannel(1,snoreSound,0);
	waterfallChannel = Mix_PlayChannel(2,waterfallSound, 0);

	Mix_Volume(snoreChannel, MIX_MAX_VOLUME);
	Mix_Volume(waterfallChannel, MIX_MAX_VOLUME);

	updateEnvironment();
}

void EffectCave::update(float deltaTime)
{
	if (this->isEnding)
	{
		if (!Mix_PlayingMusic())
		{
			this->isGameOver = true;
			this->isEnding = false;
		}
	}
	else
	{
		if (!isGameOver)
		{
			if (!Mix_Playing(snoreChannel))
			{
				std::cout << "reset snore" << std::endl;
				snoreChannel = Mix_PlayChannel(1, snoreSound, 0);
			}

			if (!Mix_Playing(waterfallChannel))
			{
				std::cout << "reset waterfall" << std::endl;
				waterfallChannel = Mix_PlayChannel(2, waterfallSound, 0);
			}
		}
	}
}

void EffectCave::movePlayer(bool goForward)
{
	TSquare* nextSquare = getNextSquare(player->square, player->direction, goForward);

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

void EffectCave::onKeyPressed(SDL_Scancode key)
{
	if (key == SDL_SCANCODE_W && !isGameOver && !isEnding)
	{
		movePlayer(true);
	}
	if (key == SDL_SCANCODE_S && !isGameOver && !isEnding)
	{
		movePlayer(false);
	}
	if (key == SDL_SCANCODE_D && !isGameOver && !isEnding)
	{
		player->turnRight();
		onStep();
		updateEnvironment();
	}
	if (key == SDL_SCANCODE_A && !isGameOver && !isEnding)
	{
		player->turnLeft();
		onStep();
		updateEnvironment();
	}

	if (key == SDL_SCANCODE_T && !isGameOver && !isEnding)
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

TSquare* EffectCave::getNextSquare(TSquare* initialSquare, TDirection direction, bool goForward)
{
	if (initialSquare != NULL)
	{
		int i = initialSquare->i;
		int j = initialSquare->j;

		if (goForward)
		{
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
		}
		else
		{
			switch (direction)
			{
			case TDirection::NORTH:
				i++;
				break;
			case TDirection::SOUTH:
				i--;
				break;
			case TDirection::EAST:
				j--;
				break;
			case TDirection::WEST:
			default:
				j++;
				break;
			}

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
	//std::cout << "wall" << std::endl;
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
		nextMonsterSquare = getNextSquare(monsterSquare, intToDirection((move + k) % 4), true);
		k++;
	}

	if (nextMonsterSquare != NULL)
	{
		monsterSquare = nextMonsterSquare;

		int channel = Mix_PlayChannel(-1, stepSound, 0);
		Mix_SetReverseStereo(channel,1);

		if (monsterSquare == player->square)
		{
			onMonsterHit();
		}
	}
}

void EffectCave::onExitReached()
{
	this->previousResult = TResult::RS_EXIT;
	//std::cout << "exit yay" << std::endl;
	Mix_PlayMusic(exitMusic, 0);
	gameOver();
}

void EffectCave::onMonsterHit()
{
	this->previousResult = TResult::RS_MONSTER;
	//std::cout << "game over" << std::endl;
	Mix_PlayChannel(-1, eatingSound, 0);

	Mix_PlayMusic(gameOverMusic, 0);

	gameOver();
}

void EffectCave::onStep()
{
	//std::cout << "tap" << std::endl;
	Mix_PlayChannel(-1, stepSound, 0);
}

const float BOARD_DIAGONAL = 10 * sqrtf(2);

Uint8 getDistance(TSquare* sq1, TSquare* sq2, float normalizationDistance)
{
	int di = sq1->i - sq2->i;
	int dj = sq1->j - sq2->j;

	float rawDistance = sqrtf(di * di + dj * dj);
	float d = (rawDistance > normalizationDistance ? BOARD_DIAGONAL * 0.95f : rawDistance) / BOARD_DIAGONAL * 255.f;
	//std::cout << "d(" << (int)sq1->i << ", " << (int)sq1->j << "), (" << sq2->i << "," << sq2->j << ")" << std::endl;

	d *= (expf(-0.05f * d));
	return (Uint8) d;
}

Sint16 getAngle(TSquare* sq1, TSquare* sq2, TDirection direction)
{
	float di = sq2->i - sq1->i;
	float dj = sq2->j - sq1->j;
	float d = sqrtf(di * di + dj * dj);

	float dx = (direction == TDirection::NORTH ? -1 : direction == TDirection::SOUTH ? 1 : 0);
	float dy = (direction == TDirection::WEST ? -1 : direction == TDirection::EAST ? 1 : 0);

	float y = dj / d;
	float x = di / d;
	//std::cout << "di " << di << " dj " << dj << " dx " << dx << " dy " << dy << " " << y << " " << x << std::endl;

	//float angle = atan2f(y , x) * 180.f / M_PI;
	//float angle = acosf(y * dy + x * dx) * 180.f / M_PI;
	float angle = atan2f(x * dy - y * dx , y * dy + x * dx) * 180.f / M_PI;
	//std::cout << "di " << di << " dj " << dj << " dx " << dx << " dy " << dy << " " << y << " " << x << " " << angle << " " << (Sint16) angle << std::endl;
	return (Sint16)angle;
}

void EffectCave::updateEnvironment()
{
	// waterfall
	Uint8 exitDistance = getDistance(player->square, exitSquare, BOARD_DIAGONAL);
	Sint16 exitAngle = getAngle(player->square, exitSquare, player->direction);
	std::cout << "waterfall d: " << (int) exitDistance << " a: " << exitAngle << std::endl;
	//sprintf_s(str, 100, "waterfall d: %d a:%d", exitDistance, exitAngle);
	
	Mix_SetPosition(waterfallChannel, exitAngle, exitDistance);
	// monster
	
	Uint8 monsterDistance = getDistance(player->square, monsterSquare, BOARD_DIAGONAL);
	Sint16 monsterAngle = getAngle(player->square, monsterSquare, player->direction);
	std::cout << "monster d: " << (int) monsterDistance << " a: " << monsterAngle << std::endl;

	Mix_SetPosition(snoreChannel, monsterAngle, monsterDistance);
}

void EffectCave::gameOver()
{
	this->isEnding = true;

	std::cout << "halting channels" << std::endl;

	Mix_HaltChannel(snoreChannel);
	Mix_HaltChannel(waterfallChannel);
	std::cout << "channels halted" << std::endl;

	//this->isGameOver = true;
}
