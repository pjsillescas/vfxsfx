#include "TransitionEffect.h"


#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <cmath>
#include "Clock.h"

TransitionEffect::TransitionEffect(SDL_Surface* surface, int screenHeight, int screenWidth, EffectTemplate* src, EffectTemplate* dst) : EffectTemplate(surface, screenHeight, screenWidth, 0)
{
	this->src = src;
	this->dst = dst;
}

void TransitionEffect::init() {

	startTime = Clock::getInstance().getCurrentTime();
	currentTime = Clock::getInstance().getCurrentTime();

	if (src)
	{
		src->init();
	}

	if (dst)
	{
		dst->init();
	}
}

void TransitionEffect::update(float deltaTime)
{
	if (!isEnded())
	{
		src->update(deltaTime);
		dst->update(deltaTime);
	}
}

void TransitionEffect::render() {
	if (!isEnded())
	{
		src->render();
		dst->render();

		renderFrame();
	}
}

TransitionEffect::~TransitionEffect()
{
	;
}



/*
*   copy an image to the screen with added distortion.
*   no bilinear filtering.
*/
void TransitionEffect::renderFrame()
{
	// setup the offsets in the buffers
	Uint8* dst;
	Uint8* initbuffer = (Uint8*)surface->pixels;
	int bpp = surface->format->BytesPerPixel;
	
	int bppImage;
	int pitch;

	Uint8* imageBufferSrc = (Uint8*)this->src->getSurface()->pixels;
	Uint8* imageBufferDst = (Uint8*)this->dst->getSurface()->pixels;
	bppImage = this->dst->getSurface()->format->BytesPerPixel;
	pitch = this->dst->getSurface()->pitch;

	SDL_LockSurface(surface);
	// loop for all lines
	for (int j = 0; j < screenHeight; j++)
	{
		dst = initbuffer + j * surface->pitch;
		
		// for all pixels
		for (int i = 0; i < screenWidth; i++)
		{
			if (useSourceBuffer(i, j))
			{
				// copy it to the screen
				Uint8* p = (Uint8*)imageBufferSrc + j * pitch + i * bppImage;

				*(Uint32*)dst = *(Uint32*)p;
			}
			// next pixel
			dst += bpp;
		}
	}
	SDL_UnlockSurface(surface);
	
	prepareNextFrame();
}
