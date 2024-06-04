#pragma once

#include "EffectTemplate.h"

class Player;

struct TSquare;
enum class TDirection;

enum class TResult {
	RS_EXIT,
	RS_MONSTER,
	RS_NONE,
};

class EffectCave : public EffectTemplate
{
public:
	const int MAX_SQUARES = 10;

private:
	TSquare *board;
	Player* player;
	TSquare* startSquare;
	TSquare* exitSquare;
	TSquare* originalMonsterSquare;
	TSquare* monsterSquare;

	TResult previousResult;

	bool renderDebug;
	bool isGameOver;
	bool isEnding;

	int snoreChannel = 0;
	int waterfallChannel = 0;

public:

	EffectCave(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, short board[10][10]);
	~EffectCave();
	
	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;
	virtual void onKeyPressed(SDL_Scancode key) override;

private:
	TSquare* getNextSquare(TSquare* initialSquare, TDirection direction, bool goForward);
	void onWallHit();
	void onExitReached();
	void onMonsterHit();
	void onStep();
	void onMonsterStep();
	void updateEnvironment();
	void renderDebugGraphics();
	void gameOver();
	void moveMonster();
	TDirection intToDirection(int n);
	void movePlayer(bool goForward);
	Uint8 getDistance(TSquare* sq1, TSquare* sq2, float normalizationDistance);
	Sint16 getAngle(TSquare* sq1, TSquare* sq2, TDirection direction);
};

