#include "EffectTemplate.h"

#include <iostream>
#include <SDL_image.h>
#include "Clock.h"

EffectTemplate::EffectTemplate(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title)
{
	this->surface = surface;
	this->screenHeight = screenHeight;
	this->screenWidth = screenWidth;
	this->timeout = timeout;
	this->bIsLateInit = false;
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

bool EffectTemplate::isLateInit() const
{
	return bIsLateInit;
}

const char* EffectTemplate::getTitle()
{
	return title.c_str();
}

void EffectTemplate::setTitle(std::string title)
{
	this->title = title;
}

EffectTemplate* EffectTemplate::setIsLateInit(bool isLateInit)
{
	bIsLateInit = isLateInit;

	return this;
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
		SDL_Surface* temp = IMG_Load(fileName);
		if (temp == NULL) {
			std::cout << "Image can be loaded! " << IMG_GetError();
			exit(1);
		}
		return SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_ARGB8888, 0);
}