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

void EffectTemplate::setSurface(SDL_Surface* surface)
{
	this->surface = surface;
}
