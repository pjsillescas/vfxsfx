#include "WhirlpoolEffect.h"

#include <iostream>
#include "Clock.h"

WhirlpoolEffect::WhirlpoolEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, const char* texture, int tiling, float offset, float speed, bool applyNoise) : EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
	image = loadImage(texture);

	this->tiling = tiling;
	this->offset = offset;
	this->speed = speed;
	this->applyNoise = applyNoise;
}

void WhirlpoolEffect::init()
{
}

void WhirlpoolEffect::update(float deltaTime)
{
	offset += speed * deltaTime / 1000.f;

	if (offset > screenWidth)
	{
		offset -= screenWidth;
	}
}

int WhirlpoolEffect::noise(int k, int n) const
{
	if (!applyNoise)
	{
		return k;
	}

	int r = rand() % 3;
	switch (r)
	{
	case 0:
		return (k == 0) ? n - 1 : k - 1;
		break;
	case 1:
		return k;
		break;
	case 2:
	default:
		return (k == n - 1) ? 0 : k + 1;
		break;
	}
}

void WhirlpoolEffect::render()
{
	Uint8* initbuffer = (Uint8*)surface->pixels;
	Uint8* dst;
	Uint8* src;
	int bpp = surface->format->BytesPerPixel;

	for (int j = 0; j < screenHeight; j++)
	{
		dst = initbuffer + j * surface->pitch;

		for (int i = 0; i < screenWidth; i++)
		{
			int iStar = noise(i, screenWidth);
			int jStar = noise(j, screenHeight);
			src = getImagePosition(iStar,jStar, (float) tiling, offset);
			*(Uint32*)dst = *(Uint32*)src;

			dst += bpp;
		}
	}

	SDL_UnlockSurface(surface);
}

Uint8* WhirlpoolEffect::getImagePosition(int i, int j, float tiling, float offset)
{
	Uint8* initbuffer = (Uint8*)image->pixels;
	Uint8* src = 0;
	int bpp = image->format->BytesPerPixel;
	
	float deltax = ((float)i - (float)screenWidth / 2.f);
	float deltay = ((float)j - (float)screenHeight / 2.f);
	float r = sqrtf(deltax * deltax + deltay * deltay);

	float rMax = 0.8f * (float) screenHeight;

	int id = (int) ((float)screenWidth / 2.f * (r / rMax) * tiling + offset);

	while (id > screenWidth / 2)
	{
		id -= screenWidth / 2;
	}

	return initbuffer + id * bpp;
}

WhirlpoolEffect::~WhirlpoolEffect()
{
	SDL_FreeSurface(image);
}
