#include "ParticleEffect.h"

#include "TunnelEffect.h"

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <cmath>

#include "Clock.h"

ParticleEffect::ParticleEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout) : EffectTemplate(surface, screenHeight, screenWidth, timeout)
{
	scaleX = new int[screenWidth];
	scaleY = new int[screenHeight];
}

void ParticleEffect::init()
{
	// generate our points
	pts = new VECTOR[MAXPTS];
	for (int i = 0; i < MAXPTS; i++) {
		pts[i] = (rotX(2.0f * M_PI * sin((float)i / 203))
			* rotY(2.0f * M_PI * cos((float)i / 157))
			* rotZ(-2.0f * M_PI * cos((float)i / 181))) * VECTOR(64 + 16 * sin((float)i / 191), 0, 0);
	}
	// create the second buffer for effects
	secondScreen = SDL_CreateRGBSurface(0, screenWidth, screenHeight, 32, 0, 0, 0, 0);
	//	secondScreen = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, screenSurface->format->format);
}

void ParticleEffect::update(float deltaTime)
{
	int currentTime = Clock::getInstance().getCurrentTime();

	// recompute parameters for image rescaling
	int sx = (int)((screenWidth / 2) - (screenWidth / 4) * sin((float)currentTime / 5590)),
		sy = (int)((screenHeight / 2) + (screenHeight / 4) * sin((float)currentTime / 6110));
	for (int i = 0; i < screenWidth; i++) scaleX[i] = (int)(sx + (i - sx) * 0.85f);
	for (int i = 0; i < screenHeight; i++) scaleY[i] = (int)(sy + (i - sy) * 0.85f);
	// setup the position of the object
	base_dist = 128 + 64 * sin((float)currentTime / 3200);
	obj = rotX(2.0f * M_PI * sin((float)currentTime / 2800))
		* rotY(2.0f * M_PI * cos((float)currentTime / 3000))
		* rotZ(-2.0f * M_PI * sin((float)currentTime / 2500));

}

void ParticleEffect::render()
{
	//SDL_LockSurface(screenSurface);

	// rescale the image
	Rescale(surface, secondScreen);
	//SDL_BlitSurface(screenSurface, NULL, secondScreen, NULL);
	// blur it
	Blur(secondScreen, surface);
	//SDL_BlitSurface(secondScreen, NULL, screenSurface, NULL);

	// draw the particles
	for (int i = 0; i < MAXPTS; i++)
	{
		Draw(surface, obj * pts[i]);
	}

	//SDL_UnlockSurface(screenSurface);

}

ParticleEffect::~ParticleEffect()
{
	delete[] scaleX;
	delete[] scaleY;
	
	SDL_FreeSurface(secondScreen);
}


/*
* scale an image away from a given point
*/
void ParticleEffect::Rescale(SDL_Surface* src, SDL_Surface* dst)
{
	Uint8* dstPixel;
	Uint8* initbuffer = (Uint8*)src->pixels;
	Uint8* finalbuffer = (Uint8*)dst->pixels;
	int initbufferbpp = src->format->BytesPerPixel;
	int finalbufferbpp = dst->format->BytesPerPixel;
	for (int j = 0; j < screenHeight; j++)
	{
		dstPixel = finalbuffer + j * dst->pitch;
		for (int i = 0; i < screenWidth; i++)
		{
			// get value from pixel in scaled image, and store
			SDL_Color resultColor;
			SDL_GetRGB(*(Uint32*)((Uint8*)src->pixels + scaleY[j] * src->pitch + scaleX[i] * initbufferbpp), src->format, &resultColor.r, &resultColor.g, &resultColor.b);
			*(Uint32*)dstPixel = SDL_MapRGB(dst->format, resultColor.r, resultColor.g, resultColor.b);
			dstPixel += finalbufferbpp;
		}
	}
}

/*
* smooth a buffer
*/
void ParticleEffect::Blur(SDL_Surface* src, SDL_Surface* dst)
{
	Uint8* dstPixel;
	Uint8* initbuffer = (Uint8*)src->pixels;
	Uint8* finalbuffer = (Uint8*)dst->pixels;
	int initbufferbpp = src->format->BytesPerPixel;
	int finalbufferbpp = dst->format->BytesPerPixel;

	//first line black
	memset(finalbuffer, 0, screenWidth * finalbufferbpp);

	for (int j = 1; j < (screenHeight - 1); j++)
	{
		dstPixel = finalbuffer + j * dst->pitch;
		// set first pixel of the line to 0
		*(Uint32*)dstPixel = 0;
		dstPixel += finalbufferbpp;
		// calculate the filter for all the other pixels
		for (int i = 1; i < (screenWidth - 1); i++)
		{
			// calculate the average
			SDL_Color resultColor[8];
			SDL_GetRGB(*(Uint32*)((Uint8*)src->pixels + (j - 1) * src->pitch + (i - 1) * initbufferbpp), src->format, &resultColor[0].r, &resultColor[0].g, &resultColor[0].b);
			SDL_GetRGB(*(Uint32*)((Uint8*)src->pixels + (j - 1) * src->pitch + (i)*initbufferbpp), src->format, &resultColor[1].r, &resultColor[1].g, &resultColor[1].b);
			SDL_GetRGB(*(Uint32*)((Uint8*)src->pixels + (j - 1) * src->pitch + (i + 1) * initbufferbpp), src->format, &resultColor[2].r, &resultColor[2].g, &resultColor[2].b);
			SDL_GetRGB(*(Uint32*)((Uint8*)src->pixels + (j)*src->pitch + (i - 1) * initbufferbpp), src->format, &resultColor[3].r, &resultColor[3].g, &resultColor[3].b);
			SDL_GetRGB(*(Uint32*)((Uint8*)src->pixels + (j)*src->pitch + (i + 1) * initbufferbpp), src->format, &resultColor[4].r, &resultColor[4].g, &resultColor[4].b);
			SDL_GetRGB(*(Uint32*)((Uint8*)src->pixels + (j + 1) * src->pitch + (i - 1) * initbufferbpp), src->format, &resultColor[5].r, &resultColor[5].g, &resultColor[5].b);
			SDL_GetRGB(*(Uint32*)((Uint8*)src->pixels + (j + 1) * src->pitch + (i)*initbufferbpp), src->format, &resultColor[6].r, &resultColor[6].g, &resultColor[6].b);
			SDL_GetRGB(*(Uint32*)((Uint8*)src->pixels + (j + 1) * src->pitch + (i + 1) * initbufferbpp), src->format, &resultColor[7].r, &resultColor[7].g, &resultColor[7].b);
			SDL_Color medianColor = { 0,0,0 };
			for (int c = 0; c < 8; c++) {
				medianColor.r += resultColor[c].r;
				medianColor.g += resultColor[c].g;
				medianColor.b += resultColor[c].b;
			}
			medianColor.r = medianColor.r / 8;
			medianColor.g = medianColor.g / 8;
			medianColor.b = medianColor.b / 8;
			// store the pixel
			Uint32 finalColor = SDL_MapRGB(dst->format, medianColor.r, medianColor.g, medianColor.b);
			*(Uint32*)dstPixel = finalColor;
			dstPixel += finalbufferbpp;
		}
		// set last pixel of the line to 0
		*(Uint32*)dstPixel = 0;
		dstPixel += finalbufferbpp;
	}
}

/*
* draw one single particle
*/
void ParticleEffect::Draw(SDL_Surface* where, VECTOR v)
{
	// calculate the screen coordinates of the particle
	float iz = 1 / (v[2] + base_dist),
		x = (screenWidth / 2) + (screenWidth / 2) * v[0] * iz,
		y = (screenHeight / 2) + (screenHeight / 2) * v[1] * iz;
	// clipping
	if ((x < 0) || (x > (screenWidth - 1)) || (y < 0) || (y > (screenHeight - 1))) return;
	// convert to fixed point to help antialiasing
	int sx = (int)(x * 8.0f),
		sy = (int)(y * 8.0f);
	// commpute color with Z
	Uint32 colorZ = (int)(iz * 20000);
	if (colorZ > 255) colorZ = 255;
	Uint32 color = 0xFF000000 + (colorZ << 16) + (colorZ << 8) + colorZ;
	// draw particle
	*((Uint32*)((Uint8*)where->pixels + (sy / 8) * where->pitch + (sx / 8) * where->format->BytesPerPixel)) = color;
}
