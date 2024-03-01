#include "RotozoomEffect.h"

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <cmath>

#include "Clock.h"

RotozoomEffect::RotozoomEffect(SDL_Surface* surface, int screenHeight, int screenWidth) : EffectTemplate(surface, screenHeight, screenWidth)
{
}

void RotozoomEffect::init()
{
	IMG_Init(IMG_INIT_PNG);
	// load the texture
	SDL_Surface* temp = IMG_Load("texture_zoom.png");
	if (temp == NULL) {
		std::cout << "Image can be loaded! " << IMG_GetError();
		exit(1);
	}
	texdata = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_ARGB8888, 0);
}

void RotozoomEffect::update(float deltaTime)
{
	int currentTime = Clock::getInstance().getCurrentTime();

	DoRotoZoom(
		1024 * sin((float)currentTime / 3000.0f),       // X centre coord
		2048 * cos((float)currentTime / 2700.0f),       // Y centre coord
		256.0f + 192.0f * cos((float)currentTime / 400.0f), // zoom coef
		(float)(currentTime) / 700.0f);                 // angle
}

void RotozoomEffect::render()
{
	TextureScreen();
}

RotozoomEffect::~RotozoomEffect()
{
	SDL_FreeSurface(texdata);
}

/*
* wrapper for the TextureScreen procedure
* all parameters are 16.16 fixed point
*/
void RotozoomEffect::DoRotoZoom(float cx, float cy, float radius, float angle)
{
	pointx1 = (int)(65536.0f * (cx + radius * cos(angle))),
		pointy1 = (int)(65536.0f * (cy + radius * sin(angle))),
		pointx2 = (int)(65536.0f * (cx + radius * cos(angle + 2.02458))),
		pointy2 = (int)(65536.0f * (cy + radius * sin(angle + 2.02458))),
		pointx3 = (int)(65536.0f * (cx + radius * cos(angle - 1.11701))),
		pointy3 = (int)(65536.0f * (cy + radius * sin(angle - 1.11701)));
}

/*
* render a textured screen with no blocks
* all parameters are 16.16 fixed point
*/
void RotozoomEffect::TextureScreen()
{
	// setup the offsets in the buffers
	Uint8* dst;
	Uint8* initbuffer = (Uint8*)surface->pixels;
	int bpp = surface->format->BytesPerPixel;
	Uint8* imagebuffer = (Uint8*)texdata->pixels;
	int bppImage = texdata->format->BytesPerPixel;
	// compute deltas
	int dxdx = (pointx2 - pointx1) / screenWidth,
		dydx = (pointy2 - pointy1) / screenWidth,
		dxdy = (pointx3 - pointx1) / screenHeight,
		dydy = (pointy3 - pointy1) / screenHeight;

	SDL_LockSurface(surface);
	// loop for all lines
	for (int j = 0; j < screenHeight; j++)
	{
		dst = initbuffer + j * surface->pitch;
		pointx3 = pointx1;
		pointy3 = pointy1;
		// for each pixel
		for (int i = 0; i < screenWidth; i++)
		{
			// get texel and store pixel
			Uint8* p = (Uint8*)imagebuffer + ((pointy3 >> 16) & 0xff) * texdata->pitch + ((pointx3 >> 16) & 0xFF) * bppImage;
			// copy it to the screen
			*(Uint32*)dst = *(Uint32*)p;
			// interpolate to get next texel in texture space
			pointx3 += dxdx;
			pointy3 += dydx;
			// next pixel
			dst += bpp;
		}
		// interpolate to get start of next line in texture space
		pointx1 += dxdy;
		pointy1 += dydy;
	}
	SDL_UnlockSurface(surface);
}


void initRotozoom();
void updateRotozoom();
void renderRotozoom();

void DoRotoZoom(float cx, float cy, float radius, float angle);
void TextureScreen();


void initRotozoom() {

}

void updateRotozoom() {
}

void renderRotozoom() {
}


