#include "FlashEffect.h"

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <cmath>

#include "Clock.h"
#include "TextUtils.h"
#include "FlockingEffect.h"
#include "PlasmaPec1Effect.h"
#include "FractalPec1Effect.h"
#include "DistortionPec1Effect.h"
#include "SpyralEffect.h"
#include "BarsEffect.h"
#include "WhirlpoolEffect.h"
#include "BlackScreenEffect.h"

FlashEffect::FlashEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title) : EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
	// load the texture
	flashTexture = loadImage("uoc.png");
}

FlashEffect::FlashEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, const char* fileName, float flashMaxTime) : EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
	// load the texture
	flashTexture = loadImage(fileName);
	this->flashMaxTime = flashMaxTime;
}

Uint32 FlashEffect::getRandomColor()
{
	return 0xFF000000 | ((rand() % 256) << 16) | ((rand() % 256) << 8) | (rand() % 256);
}

void FlashEffect::init()
{
	currentTime = 0;
	previousTime = 0;
	flashtime = 0;
	Backgroundcolor = getRandomColor();

	blackSurface = SDL_CreateRGBSurfaceWithFormat(0, screenWidth, screenHeight, 32, SDL_PIXELFORMAT_RGBA32);
	SDL_FillRect(blackSurface, NULL, SDL_MapRGB(blackSurface->format, 0, 0, 0));
}

void FlashEffect::update(float deltaTime)
{
	currentTime += (int)deltaTime;
	previousTime = currentTime;
	
	if (currentTime >= flashMaxTime)
	{
		currentTime = 0;
		flashtime = flashMaxTime;
	}

	if (flashtime > 0)
	{
		flashtime -= (int)deltaTime;
	}
	else
	{
		flashtime = 0;
	}
}

void FlashEffect::changeBackgroundColor()
{
	if (previousTime != currentTime)
	{
		Backgroundcolor = getRandomColor();
	}

	SDL_FillRect(surface, NULL, Backgroundcolor);
}

void FlashEffect::render()
{
	changeBackgroundColor();
	if (flashtime > 0)
	{
		renderFlash(flashTexture, (Uint8)(255 * (1.f - flashtime / flashMaxTime)));
	}
}

FlashEffect::~FlashEffect()
{

	SDL_FreeSurface(blackSurface);
}

void FlashEffect::renderFlash(SDL_Surface* surf, Uint8 alpha)
{
	SDL_SetSurfaceAlphaMod(surf, alpha);
	SDL_BlitSurface(surf, NULL, surface, NULL);
	SDL_SetSurfaceAlphaMod(surf, 255);
}

