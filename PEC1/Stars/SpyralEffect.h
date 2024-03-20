#pragma once

#include "EffectTemplate.h"

// this record contains the information for one star
struct TSpyralStar
{
	float x, y;             // position of the star
	float theta;	// angle
	float radius;
};

class SpyralEffect : public EffectTemplate
{
public:
	// Stars
	// change this to adjust the number of stars
	const int MAXSTARS = 256;

private:
	// this is a pointer to an array of stars
	TSpyralStar* stars = NULL;

	void putPixel(SDL_Surface* surface, int x, int y, Uint32 pixel) const;
	void updateStarsPosition();

public:

	SpyralEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title);
	~SpyralEffect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;

};
