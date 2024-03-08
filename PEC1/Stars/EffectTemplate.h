#pragma once

#include <SDL.h>

class EffectTemplate
{
protected:
	SDL_Surface* surface;
	int screenHeight;
	int screenWidth;
	int timeout;
	bool bIsEnded;

public:
	EffectTemplate(SDL_Surface* surface,int screenHeight,int screenWidth, int timeout);

	void updateFixed(float deltaTime);
	void start();

	virtual void init() = 0;
	virtual void update(float deltaTime) = 0;
	virtual void render() = 0;
	
	bool isEnded();
	void setIsEnded(bool isEnded);

	SDL_Surface* getSurface();
	void setSurface(SDL_Surface* surface);

protected:
	SDL_Surface* loadImage(const char* fileName);
};
