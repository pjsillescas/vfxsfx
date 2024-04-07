#pragma once

#include "EffectTemplate.h"

class Player;

struct TSquare;
enum class TDirection;

class EffectCave : public EffectTemplate
{
public:
	const int MAX_SQUARES = 10;

private:
	TSquare *board;
	Player* player;
	TSquare* startSquare;
	TSquare* exitSquare;

public:

	EffectCave(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, short board[10][10]);
	~EffectCave();
	
	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;
	virtual void onKeyPressed(SDL_Scancode key) override;

private:
	TSquare* getNextSquare(TSquare* initialSquare, TDirection direction);
	void onWallHit();
	void onExitReached();
	void onMonsterHit();
	void onStep();
	void updateEnvironment();
};

