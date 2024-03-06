#pragma once

#include "EffectTemplate.h"

// this record contains the information for one star
struct TStarChallenge2
{
	float x, y;             // position of the star
	float speed;
	Uint32 color;
};


class StarsEffectChallenge2 : public EffectTemplate
{
private:
	const float MIN_SPEED = 0.15f;
	const float MAX_SPEED = 0.50f;
	const int MIN_COLOR_VALUE = 100;
	const int MAX_COLOR_VALUE = 255;

public:
	// Stars
	// change this to adjust the number of stars
	const int MAXSTARS = 256;

private:
	// this is a pointer to an array of stars
	TStarChallenge2* stars = NULL;

	void putPixel(SDL_Surface* surface, int x, int y, Uint32 pixel) const;

	float getRandomNumber();
	float getSpeed(float rng);
	Uint32 getColor(float rng);
public:

	StarsEffectChallenge2(SDL_Surface* surface, int screenHeight, int screenWidth);
	~StarsEffectChallenge2();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;
	virtual bool isEnded() override;


};

