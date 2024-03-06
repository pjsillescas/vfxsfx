#include "EffectTemplate.h"

EffectTemplate::EffectTemplate(SDL_Surface* surface, int screenHeight, int screenWidth)
{
	this->surface = surface;
	this->screenHeight = screenHeight;
	this->screenWidth = screenWidth;
}

SDL_Surface* EffectTemplate::getSurface()
{
	return surface;
}

