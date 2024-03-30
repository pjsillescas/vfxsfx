#include "SpyralEffect.h"

#include <stdlib.h>
#include <math.h>

struct TSpyralStar
{
	float x, y;		// position of the star
	float theta;	// angle
	float radius;
	Uint32 color;
};


SpyralEffect::SpyralEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title) : EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
	// allocate memory for all our stars
	stars = new TSpyralStar[MAXSTARS];
}

void SpyralEffect::init()
{
	const float MIN_RADIUS = 20.f;
	const float MAX_RADIUS = 0.9f * ((float)screenHeight / 2);

	int k = 0;
	int i;

	Uint32 minColor = 0xFF0000FF;
	Uint32 maxColor = 0xFFFF0000;

	float step = 8.f / MAXSTARS;
	for (i = 0; i < MAXSTARS / 8; i++)
	{
		Uint32 currentColor = minColor + (maxColor - minColor) * ((float)i * 8.f / MAXSTARS);
		float radius = MIN_RADIUS + (MAX_RADIUS - MIN_RADIUS) * ((float)i * step);

		stars[k].radius = radius;
		stars[k].theta = 0;
		stars[k].color = currentColor;
		k++;
		
		stars[k].radius = radius;
		stars[k].theta = M_PI / 4.f;
		stars[k].color = currentColor;
		k++;

		stars[k].radius = radius;
		stars[k].theta = M_PI / 2.f;
		stars[k].color = currentColor;
		k++;

		stars[k].radius = radius;
		stars[k].theta = 3.f* M_PI / 4.f;
		stars[k].color = currentColor;
		k++;

		stars[k].radius = radius;
		stars[k].theta = M_PI;
		stars[k].color = currentColor;
		k++;

		stars[k].radius = radius;
		stars[k].theta = 5.f * M_PI / 4.f;
		stars[k].color = currentColor;
		k++;

		stars[k].radius = radius;
		stars[k].theta = 3.f * M_PI / 2.f;
		stars[k].color = currentColor;
		k++;
		
		stars[k].radius = radius;
		stars[k].theta = 7.f * M_PI / 4.f;
		stars[k].color = currentColor;
		k++;
	}

	updateStarsPosition();
	
}

void SpyralEffect::updateStarsPosition()
{
	float xc = (float)screenWidth / 2.f;
	float yc = (float)screenHeight / 2.f;
	for (int i = 0; i < MAXSTARS; i++)
	{
		stars[i].x = xc + stars[i].radius * cosf(-stars[i].theta);
		stars[i].y = yc - stars[i].radius * sinf(-stars[i].theta);
	}
}

float clampCircular(float x, float xf)
{
	while (x > xf)
	{
		x -= xf;
	}

	return x;
}

void SpyralEffect::update(float deltaTime)
{
	// update all stars
	for (int i = 0; i < MAXSTARS; i++)
	{
		stars[i].theta = clampCircular(stars[i].theta + STAR_VELOCITY * deltaTime / (stars[i].radius * 1000.f), 2.f * M_PI);
	}

	updateStarsPosition();
}

void SpyralEffect::render()
{
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
	// update all stars
	for (int i = 0; i < MAXSTARS; i++)
	{
		// draw this star, with a colour depending on the plane
		Uint32 color = stars[i].color;
		putPixel(surface, (int)stars[i].x, (int)stars[i].y, color);
		putPixel(surface, (int)stars[i].x + 1, (int)stars[i].y, color);
		putPixel(surface, (int)stars[i].x, (int)stars[i].y + 1, color);
		putPixel(surface, (int)stars[i].x + 1, (int)stars[i].y + 1, color);

	}
}

/*
* Set the pixel at (x, y) to the given value
* NOTE: The surface must be locked before calling this!
*/
void SpyralEffect::putPixel(SDL_Surface* surface, int x, int y, Uint32 pixel) const
{
	// Clipping
	if ((x < 0) || (x >= screenWidth) || (y < 0) || (y >= screenHeight))
	{
		return;
	}

	int bpp = surface->format->BytesPerPixel;
	// Here p is the address to the pixel we want to set
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp)
	{
	case 1:
		*p = pixel;
		break;

	case 2:
		*(Uint16*)p = pixel;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
		{
			p[0] = (pixel >> 16) & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = pixel & 0xff;
		}
		else
		{
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

SpyralEffect::~SpyralEffect()
{
	// free memory
	delete[] stars;
}