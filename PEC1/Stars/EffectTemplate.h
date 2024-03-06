#pragma once

#include <SDL.h>

class EffectTemplate
{
protected:
	SDL_Surface* surface;
	int screenHeight;
	int screenWidth;

public:
	EffectTemplate(SDL_Surface* surface,int screenHeight,int screenWidth);
	virtual void init() = 0;
	virtual void update(float deltaTime) = 0;
	virtual void render() = 0;
	virtual bool isEnded() = 0;

	SDL_Surface* getSurface();

};
