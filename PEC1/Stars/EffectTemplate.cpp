#include "EffectTemplate.h"

#include "Clock.h"

int currentTime;
int startTime;

EffectTemplate::EffectTemplate(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout)
{
	this->surface = surface;
	this->screenHeight = screenHeight;
	this->screenWidth = screenWidth;
	this->timeout = timeout;
}

void EffectTemplate::start()
{
	init();
	
	bIsEnded = false;
	startTime = Clock::getInstance().getCurrentTime();
}

SDL_Surface* EffectTemplate::getSurface()
{
	return surface;
}

void EffectTemplate::setSurface(SDL_Surface* surface)
{
	this->surface = surface;
}

void EffectTemplate::updateFixed(float deltaTime)
{
	update(deltaTime);

	currentTime = Clock::getInstance().getCurrentTime();
	int countdown = (currentTime - startTime) / 1000;
	bIsEnded = timeout > 0 && countdown > timeout;

}

bool EffectTemplate::isEnded()
{
	return bIsEnded;
}

void EffectTemplate::setIsEnded(bool isEnded)
{
	this->bIsEnded = isEnded;
}
