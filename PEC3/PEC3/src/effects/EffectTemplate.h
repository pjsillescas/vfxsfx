#pragma once

#include <SDL.h>
#include <string>

class EffectTemplate
{
protected:
	SDL_Surface* surface;
	int screenHeight;
	int screenWidth;
	int timeout;
	bool bIsEnded;
	std::string title;

public:
	EffectTemplate(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title);

	void start();

	virtual void init() = 0;
	virtual void update(float deltaTime) = 0;
	virtual void render() = 0;
	virtual void onKeyPressed(SDL_Scancode key) = 0;
	virtual const char* getTitle();
	void setTitle(std::string title);

	bool isEnded() const;
	void setIsEnded(bool isEnded);
	int getTimeout();

	SDL_Surface* getSurface();
	void setSurface(SDL_Surface* surface);

protected:
	SDL_Surface* loadImage(const char* fileName);
};
