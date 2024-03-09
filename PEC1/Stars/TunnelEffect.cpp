#include "TunnelEffect.h"

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <cmath>

#include "Clock.h"

TunnelEffect::TunnelEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title) : EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
	// alloc memory to store SCREEEN SIZE times u, v
	texcoord = new unsigned char[(long long) screenWidth * screenHeight * 2];
}

void TunnelEffect::init()
{
	long offs = 0;
	// precalc the (u,v) coordinates
	for (int j = -(screenHeight / 2); j < (screenHeight / 2); j++) {
		for (int i = -(screenWidth / 2); i < (screenWidth / 2); i++)
		{
			// get coordinates of ray that projects through this pixel
			float dx = (float)i / screenHeight;
			float dy = (float)-j / screenHeight;
			float dz = 1;
			// normalize them
			float d = 20 / sqrt(dx * dx + dy * dy + 1);
			dx *= d;
			dy *= d;
			dz *= d;
			// start interpolation at origin
			float x = 0;
			float y = 0;
			float z = 0;
			// set original precision
			d = 16;
			// interpolate along ray
			while (d > 0)
			{
				// continue until we hit a wall
				while (((x - getXPos(z)) * (x - getXPos(z)) + (y - getYPos(z)) * (y - getYPos(z)) < getRadius(z)) && (z < 1024))
				{
					x += dx;
					y += dy;
					z += dz;
				};
				// reduce precision and reverse direction
				x -= dx;  dx /= 2;
				y -= dy;  dy /= 2;
				z -= dz;  dz /= 2;
				d -= 1;
			}
			// calculate the texture coordinates
			x -= getXPos(z);
			y -= getYPos(z);
			float ang = (float) atan2(y, x) * 256.f / (float) M_PI;
			unsigned char u = (unsigned char)ang;
			unsigned char v = (unsigned char)z;
			// store texture coordinates
			texcoord[offs] = u;
			texcoord[offs + 1] = v;
			offs += 2;
		}
	}

	// load the texture
	texdata = IMG_Load("texture.png");
}

void TunnelEffect::update(float deltaTime)
{
}

void TunnelEffect::render()
{
	int currentTime = Clock::getInstance().getCurrentTime();

	DrawHole(currentTime / 16, currentTime / 32);
}

TunnelEffect::~TunnelEffect()
{
	delete[] texcoord;
	SDL_FreeSurface(texdata);
}

/*
* size of the hole
*/
float TunnelEffect::getRadius(float f)
{
	return 128;
};

/*
* position of the centre of the hole along the X axis
*/
float TunnelEffect::getXPos(float f)
{
	return -16 * (float) sin(f * M_PI / 256);
};

/*
* position of the centre of the hole along the Y axis
*/
float TunnelEffect::getYPos(float f)
{
	return -16.f * (float) sin(f * M_PI / 256);
};

void TunnelEffect::DrawHole(int du, int dv)
{
	Uint8* dst;
	Uint8* initbuffer = (Uint8*)surface->pixels;
	int bpp = surface->format->BytesPerPixel;
	Uint8* initbufferTexture = (Uint8*)texdata->pixels;
	int bppTexture = texdata->format->BytesPerPixel;
	long doffs = 0, soffs = 0;
	SDL_LockSurface(surface);
	for (int j = 0; j < screenHeight; j++) {
		dst = initbuffer + (long long) j * surface->pitch;
		for (int i = 0; i < screenWidth; i++) {
			// load (u,v) and add displacement
			unsigned char u = texcoord[soffs] + du;
			unsigned char v = texcoord[soffs + 1] + dv;

			SDL_Color Color;
			SDL_GetRGB(*(Uint32*)((Uint8*)texdata->pixels + v * texdata->pitch + u * bppTexture), texdata->format, &Color.r, &Color.g, &Color.b);
			Uint32 resultColor = SDL_MapRGB(surface->format, Color.r, Color.g, Color.b);
			*(Uint32*)dst = resultColor;
			dst += bpp;

			soffs += 2;
		}
	}
	SDL_UnlockSurface(surface);
}
