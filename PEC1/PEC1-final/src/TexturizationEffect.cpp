#include "TexturizationEffect.h"

#include <SDL.h>
#include <iostream>
#include <cmath>

#include "Clock.h"

TexturizationEffect::TexturizationEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, const char* fileName, float speedX, float speedY) : EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
	// alloc memory to store SCREEEN SIZE times u, v
	texcoord = new unsigned char[(long long)screenWidth * screenHeight * 2];
	
	// load the texture
	texdata = loadImage(fileName);

	this->speedX = speedX;
	this->speedY = speedY;
}

void TexturizationEffect::init()
{
	long offs = 0;
	// precalc the (u,v) coordinates
	for (int j = -(screenHeight / 2); j < (screenHeight / 2); j++)
	{
		for (int i = -(screenWidth / 2); i < (screenWidth / 2); i++)
		{
			// get coordinates of ray that projects through this pixel
			float dx = (float)i / screenHeight;
			float dy = (float)-j / screenHeight;
			float dz = 1;
			// normalize them
			float d = sqrt(dx * dx + dy * dy + 1);
			
			float x = RADIUS * dx / d;
			float y = RADIUS * dy / d;
			float z = RADIUS * dz / d;

			float ang = (float)atan2(y, x) / ((float)M_PI) * 256.f;
			unsigned char u = (unsigned char)ang;
			unsigned char v = (unsigned char)(z / RADIUS * 256.f);
			// store texture coordinates
			texcoord[offs] = u;
			texcoord[offs + 1] = v;
			offs += 2;
		}
	}
}

void TexturizationEffect::update(float deltaTime)
{
}

void TexturizationEffect::render()
{
	int currentTime = Clock::getInstance().getCurrentTime();

	DrawHole((int) ((float)currentTime * speedX), (int)((float) currentTime * speedY));
}

TexturizationEffect::~TexturizationEffect()
{
	delete[] texcoord;
	SDL_FreeSurface(texdata);
}

void TexturizationEffect::DrawHole(int du, int dv)
{
	Uint8* dst;
	Uint8* initbuffer = (Uint8*)surface->pixels;
	int bpp = surface->format->BytesPerPixel;
	Uint8* initbufferTexture = (Uint8*)texdata->pixels;
	int bppTexture = texdata->format->BytesPerPixel;
	long doffs = 0, soffs = 0;
	SDL_LockSurface(surface);
	for (int j = 0; j < screenHeight; j++)
	{
		dst = initbuffer + (long long)j * surface->pitch;
		for (int i = 0; i < screenWidth; i++)
		{
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
