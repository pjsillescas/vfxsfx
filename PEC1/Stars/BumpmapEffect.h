#pragma once

#include "EffectTemplate.h"

class BumpmapEffect : public EffectTemplate
{
private:
	// size of the spot light
	//const float LIGHTSIZE = 2.4f;
	const float LIGHTSIZE = 1.6f;
	const int LIGHT_PIXEL_RES = 256;

	// contains the precalculated spotlight
	unsigned char* light;
	// image color
	SDL_Surface* image;
	// imagen bump
	SDL_Surface* bump;

	// define the distortion buffer movement
	int windowx1, windowy1, windowx2, windowy2, windowZ;

public:

	BumpmapEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title);
	BumpmapEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, const char* wallFileName, const char* bumpFileName);
	~BumpmapEffect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;

private:
	void computeLight();
	void doBump();

};

