#include "Pec1FlashEffect.h"


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

Pec1FlashEffect::Pec1FlashEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, const char* fileName, float flashMaxTime) : EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
	// load the texture
	//flashTexture = loadImage(fileName);
	this->flashMaxTime = flashMaxTime;

	textures = new SDL_Surface * [NUM_FLASH_IMAGES];
	char fileNameFlash[32];
	for (int i = 0; i < NUM_FLASH_IMAGES; i++)
	{
		sprintf_s(fileNameFlash, "flash/%d.png", i+1);
		textures[i] = loadImage(fileNameFlash);
	}

}

Uint32 Pec1FlashEffect::getRandomColor()
{
	return 0xFF000000 | ((rand() % 256) << 16) | ((rand() % 256) << 8) | (rand() % 256);
}

void Pec1FlashEffect::init()
{
	currentTime = 0;
	currentImage = 0;
	previousTime = 0;
	flashtime = 0;
	Backgroundcolor = getRandomColor();

	blackSurface = SDL_CreateRGBSurfaceWithFormat(0, screenWidth, screenHeight, 32, SDL_PIXELFORMAT_RGBA32);
	SDL_FillRect(blackSurface, NULL, SDL_MapRGB(blackSurface->format, 0, 0, 0));
}

int count = 0;
void Pec1FlashEffect::update(float deltaTime)
{
	currentTime += (int)deltaTime;
	previousTime = currentTime;

	if (currentTime >= flashMaxTime)
	{
		//count++;
		//std::cout << count << std::endl;
		currentImage = (currentImage + 1) % NUM_FLASH_IMAGES;
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

void Pec1FlashEffect::changeBackgroundColor()
{
	if (previousTime != currentTime)
	{
		Backgroundcolor = getRandomColor();
	}

	SDL_FillRect(surface, NULL, Backgroundcolor);
}

void Pec1FlashEffect::render()
{
	changeBackgroundColor();
	if (flashtime > 0)
	{
		renderFlash(textures[currentImage], (Uint8)(255 * (1.f - flashtime / flashMaxTime)));
	}
}

Pec1FlashEffect::~Pec1FlashEffect()
{

	for (int i = 0; i < NUM_FLASH_IMAGES; i++)
	{
		SDL_FreeSurface(textures[i]);
	}

	delete[] textures;

	SDL_FreeSurface(blackSurface);
}

void Pec1FlashEffect::renderFlash(SDL_Surface* surf, Uint8 alpha)
{
	SDL_SetSurfaceAlphaMod(surf, alpha);
	SDL_BlitSurface(surf, NULL, surface, NULL);
	SDL_SetSurfaceAlphaMod(surf, 255);
}

