#include "StarsEffectChallenge2.h"

#include <stdlib.h>

StarsEffectChallenge2::StarsEffectChallenge2(SDL_Surface* surface, int screenHeight, int screenWidth) : EffectTemplate(surface, screenHeight, screenWidth)
{
	// allocate memory for all our stars
	stars = new TStarChallenge2[MAXSTARS];
}

float StarsEffectChallenge2::getRandomNumber()
{
	return (float)rand() / RAND_MAX;
}

float StarsEffectChallenge2::getSpeed(float rng)
{
	return MIN_SPEED + (MAX_SPEED - MIN_SPEED) * rng;
}

Uint32 StarsEffectChallenge2::getColor(float rng)
{
	int colorValue = MIN_COLOR_VALUE + (int)((MAX_COLOR_VALUE - MIN_COLOR_VALUE) * rng);

	return 0xFF000000 + colorValue * 256 * 256 + colorValue * 256 + colorValue;
}

void StarsEffectChallenge2::init() {
	// randomly generate some stars
	for (int i = 0; i < MAXSTARS; i++)
	{
		stars[i].x = (float)(rand() % screenWidth);
		stars[i].y = (float)(rand() % screenHeight);

		float rng = getRandomNumber();
		stars[i].speed = getSpeed(rng);
		stars[i].color = getColor(rng);
	}

}

void StarsEffectChallenge2::update(float deltaTime) {
	// update all stars
	for (int i = 0; i < MAXSTARS; i++)
	{
		// move this star right, determine how fast depending on which
		// plane it belongs to
		stars[i].x += deltaTime * stars[i].speed;
		// check if it's gone out of the right of the screen
		if (stars[i].x > screenWidth)
		{
			// if so, make it return to the left
			stars[i].x = -(float)(rand() % screenWidth);
			// and randomly change the y position
			stars[i].y = (float)(rand() % screenHeight);
		}
	}
}

void StarsEffectChallenge2::render() {
	// update all stars
	for (int i = 0; i < MAXSTARS; i++)
	{
		putPixel(surface, (int)stars[i].x, (int)stars[i].y, stars[i].color);
	}
}

/*
* Set the pixel at (x, y) to the given value
* NOTE: The surface must be locked before calling this!
*/
void StarsEffectChallenge2::putPixel(SDL_Surface* surface, int x, int y, Uint32 pixel) const
{
	// Clipping
	if ((x < 0) || (x >= screenWidth) || (y < 0) || (y >= screenHeight))
	{
		return;
	}

	int bpp = surface->format->BytesPerPixel;
	// Here p is the address to the pixel we want to set
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp) {
	case 1:
		*p = pixel;
		break;

	case 2:
		*(Uint16*)p = pixel;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			p[0] = (pixel >> 16) & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = pixel & 0xff;
		}
		else {
			p[0] = pixel & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = (pixel >> 16) & 0xff;
		}
		break;

	case 4:
		*(Uint32*)p = pixel;
		break;
	}
}

StarsEffectChallenge2::~StarsEffectChallenge2()
{
	// free memory
	delete[] stars;
}