#include "WhirlpoolEffect.h"


#include <iostream>
#include "Clock.h"

WhirlpoolEffect::WhirlpoolEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, int tiling, float offset, float speed, bool applyNoise) : EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
	image = loadImage("pec1-assets/gradient.png");

	this->tiling = tiling;
	this->offset = offset;
	this->speed = speed;
	this->applyNoise = applyNoise;
}

void WhirlpoolEffect::init() {
}

void WhirlpoolEffect::update(float deltaTime) {
	offset += speed * deltaTime / 1000.f;

	if (offset > screenWidth)
	{
		offset -= screenWidth;
	}
}

int WhirlpoolEffect::noise(int k, int n)
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


	//std::cout << offset << std::endl;

	for (int j = 0; j < screenHeight; j++)
	{
		dst = initbuffer + j * surface->pitch;

		for (int i = 0; i < screenWidth; i++)
		{
			int iStar = noise(i, screenWidth);
			int jStar = noise(j, screenHeight);
			src = getImagePosition(iStar,jStar, tiling, offset);
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

/*
* adds some hot pixels to a buffer
*/
void WhirlpoolEffect::seed(unsigned char* dst)
{
	int i, j;

	j = (rand() % 512);
	// add some random hot spots at the bottom of the buffer
	for (i = 0; i < j; i++)
	{
		dst[(screenWidth * 1 / 5 * (screenHeight - 3)) + (rand() % (screenWidth * 3))] = 255;
		dst[(screenWidth * 2 / 5 * (screenHeight - 3)) + (rand() % (screenWidth * 3))] = 255;
		dst[(screenWidth * 3 / 5 * (screenHeight - 3)) + (rand() % (screenWidth * 3))] = 255;
		dst[(screenWidth * 4 / 5 * (screenHeight - 3)) + (rand() % (screenWidth * 3))] = 255;
	}
}

void WhirlpoolEffect::sharpen(unsigned char* src, unsigned char* dst)
{
	int offs = 0;
	unsigned char b;
	//float omega = 1.f / 140.f;
	float omega = 1.f / 140.f;
	float time = Clock::getInstance().getCurrentTime();

	minValue = -1;
	maxValue = -1;
	for (int j = 1; j < (screenHeight - 2); j++)
	{
		// set first pixel of the line to 0
		dst[offs] = 0; offs++;
		// calculate the filter for all the other pixels
		for (int i = 1; i < (screenWidth - 1); i++)
		{
			// calculate the average
			b = (int)(src[offs - 1] - 7 * src[offs] + src[offs + 1]
				+ src[offs + (screenWidth - 1)] + src[offs + (screenWidth)] + src[offs + (screenWidth + 1)]
				+ src[offs - (screenWidth - 1)] + src[offs - screenWidth] + src[offs - (screenWidth + 1)]) / 8;
			// store the pixel
			int threshold = 150 + 150 * sinf(time * omega);
			dst[offs] = (threshold > 0 && b > threshold) ? b : 0;

			offs++;

			if (threshold > 0 && b > threshold)
			{
				if (minValue < 0)
				{
					minValue = b;
				}
				else if (minValue >= b)
				{
					minValue = b;
				}

				if (maxValue < 0)
				{
					maxValue = b;
				}
				else if (maxValue <= b)
				{
					maxValue = b;
				}
			}

		}
		// set last pixel of the line to 0
		dst[offs] = 0; offs++;
	}
}
