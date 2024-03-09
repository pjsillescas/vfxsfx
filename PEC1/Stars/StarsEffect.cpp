#include "StarsEffect.h"

#include <stdlib.h>

StarsEffect::StarsEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title) : EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
	// allocate memory for all our stars
	stars = new TStar[MAXSTARS];
}

void StarsEffect::init() {
	// randomly generate some stars
	for (int i = 0; i < MAXSTARS; i++)
	{
		stars[i].x = (float)(rand() % screenWidth);
		stars[i].y = (float)(rand() % screenHeight);
		stars[i].plane = rand() % 3;     // star colour between 0 and 2
	}

}

void StarsEffect::update(float deltaTime) {
	// update all stars
	for (int i = 0; i < MAXSTARS; i++)
	{
		// move this star right, determine how fast depending on which
		// plane it belongs to
		stars[i].x += (deltaTime + (float)stars[i].plane) * 0.15f;
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

void StarsEffect::render() {
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
	// update all stars
	for (int i = 0; i < MAXSTARS; i++)
	{
		// draw this star, with a colour depending on the plane
		unsigned int color = 0;
		switch (1 + stars[i].plane) {
		case 1:
			color = 0xFF606060; // dark grey
			break;
		case 2:
			color = 0xFFC2C2C2; // light grey
			break;
		case 3:
			color = 0xFFFFFFFF; // white
			break;
		}
		putPixel(surface, (int)stars[i].x, (int)stars[i].y, color);
	}
}

/*
* Set the pixel at (x, y) to the given value
* NOTE: The surface must be locked before calling this!
*/
void StarsEffect::putPixel(SDL_Surface* surface, int x, int y, Uint32 pixel) const
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

StarsEffect::~StarsEffect()
{
	// free memory
	delete[] stars;
}