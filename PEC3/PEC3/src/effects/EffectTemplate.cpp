#include "EffectTemplate.h"

#include <iostream>
#include "../utils/Clock.h"

EffectTemplate::EffectTemplate(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title)
{
	this->surface = surface;
	this->screenHeight = screenHeight;
	this->screenWidth = screenWidth;
	this->timeout = timeout;
	this->title = title;
}

void EffectTemplate::start()
{
	bIsEnded = false;
}

SDL_Surface* EffectTemplate::getSurface()
{
	return surface;
}

void EffectTemplate::setSurface(SDL_Surface* surface)
{
	this->surface = surface;
}

bool EffectTemplate::isEnded() const
{
	return bIsEnded;
}

const char* EffectTemplate::getTitle()
{
	return title.c_str();
}

void EffectTemplate::setTitle(std::string title)
{
	this->title = title;
}

int EffectTemplate::getTimeout() {
	return timeout;
}

void EffectTemplate::setIsEnded(bool isEnded)
{
	this->bIsEnded = isEnded;
}

SDL_Surface* EffectTemplate::loadImage(const char* fileName)
{
	return NULL;
}