#include "BarsEffect.h"

#include <iostream>
#include "Clock.h"

SDL_Surface* image;

BarsEffect::BarsEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, const char* fileName) : EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
	// two fire buffers
	bars1 = new unsigned char[screenWidth * screenHeight];
	bars2 = new unsigned char[screenWidth * screenHeight];
	palette = new RGBColor[MAX_PALETTE];

	tmp = NULL;

	image = loadImage((fileName) ? fileName : "uoc.png");
}

void BarsEffect::init() {
	buildPalette();
	// clear the buffers
	memset(bars1, 0, screenWidth * screenHeight);
	memset(bars2, 0, screenWidth * screenHeight);
}

void BarsEffect::update(float deltaTime) {
	// swap our two fire buffers
	tmp = bars1;
	bars1 = bars2;
	bars2 = tmp;

	// heat the fire
	seed(bars1);
	// apply the filter
	sharpen(bars1, bars2);
}

int clampIndex(int index, int min, int max)
{
	if (index < min)
	{
		return min;
	}
	else if(index > max)
	{
		return max;
	}

	return index;
}

void BarsEffect::render() {
	Uint8* dst;
	int src = 0;
	long i, j;
	Uint8* initbuffer = (Uint8*)surface->pixels;
	int bpp = surface->format->BytesPerPixel;

	SDL_BlitSurface(image, NULL, surface, NULL);

	SDL_LockSurface(surface);
	
	dst = initbuffer;
	for (j = 0; j < (screenHeight - 3); j++) // Menos las 3 ultimas lineas
	{
		dst = initbuffer + j * surface->pitch;
		for (i = 0; i < screenWidth; i++)
		{
			if (bars2[src] > 0)
			{
				// plot the pixel from fire2
				unsigned int Color = 0;
				int indexColor = clampIndex((int)(255.f * (bars2[src] - minValue) / (maxValue - minValue)), 0, 255);
				Color = 0xFF000000 + (palette[indexColor].R << 16) + (palette[indexColor].G << 8) + palette[indexColor].B;
				*(Uint32*)dst = Color;
			}
			dst += bpp;
			src++;
		}
	}


	SDL_UnlockSurface(surface);
}

BarsEffect::~BarsEffect()
{
	delete[] palette;
	delete[] bars1;
	delete[] bars2;

}

void BarsEffect::buildPalette()
{
	// setup some fire-like colours
	shadePal(0, 23, 0, 0, 0, 32, 0, 64);
	shadePal(24, 47, 32, 0, 64, 255, 0, 0);
	shadePal(48, 63, 255, 0, 0, 255, 255, 0);
	shadePal(64, 127, 255, 255, 0, 255, 255, 255);
	shadePal(128, 255, 255, 255, 255, 255, 255, 255);
}

/*
* create a shade of colours in the palette from s to e
*/
void BarsEffect::shadePal(int s, int e, int r1, int g1, int b1, int r2, int g2, int b2)
{
	int i;
	float k;
	for (i = 0; i <= e - s; i++)
	{
		k = (float)i / (float)(e - s);
		palette[s + i].R = (int)(r1 + (r2 - r1) * k);
		palette[s + i].G = (int)(g1 + (g2 - g1) * k);
		palette[s + i].B = (int)(b1 + (b2 - b1) * k);
	}
}

/*
* adds some hot pixels to a buffer
*/
void BarsEffect::seed(unsigned char* dst)
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

void BarsEffect::sharpen(unsigned char* src, unsigned char* dst)
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
			b = (int)(src[offs - 1]  - 7 * src[offs] + src[offs + 1]
				+ src[offs + (screenWidth - 1)] + src[offs + (screenWidth)] + src[offs + (screenWidth + 1)]
				+ src[offs - (screenWidth - 1)] + src[offs - screenWidth] + src[offs - (screenWidth + 1)])/8;
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
