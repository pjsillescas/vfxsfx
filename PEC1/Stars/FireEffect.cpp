#include "FireEffect.h"

#include <iostream>


FireEffect::FireEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout) : EffectTemplate(surface, screenHeight, screenWidth, timeout)
{
	// two fire buffers
	fire1 = new unsigned char[screenWidth * screenHeight];
	fire2 = new unsigned char[screenWidth * screenHeight];
	palette = new RGBColor[MAX_PALETTE];

	tmp = NULL;
}

void FireEffect::init() {
	buildPalette();
	// clear the buffers
	memset(fire1, 0, screenWidth * screenHeight);
	memset(fire2, 0, screenWidth * screenHeight);
}

void FireEffect::update(float deltaTime) {
	// swap our two fire buffers
	tmp = fire1;
	fire1 = fire2;
	fire2 = tmp;

	// heat the fire
	Heat(fire1);
	// apply the filter
	Blur_Up(fire1, fire2);
}

void FireEffect::render() {
	Uint8* dst;
	int src = 0;
	long i, j;
	Uint8* initbuffer = (Uint8*)surface->pixels;
	int bpp = surface->format->BytesPerPixel;

	SDL_LockSurface(surface);
	dst = initbuffer;
	for (j = 0; j < (screenHeight - 3); j++) // Menos las 3 ultimas lineas
	{
		dst = initbuffer + j * surface->pitch;
		for (i = 0; i < screenWidth; i++)
		{
			// plot the pixel from fire2
			unsigned int Color = 0;
			int indexColor = fire2[src];
			Color = 0xFF000000 + (palette[indexColor].R << 16) + (palette[indexColor].G << 8) + palette[indexColor].B;
			*(Uint32*)dst = Color;
			dst += bpp;
			src++;
		}
	}
	SDL_UnlockSurface(surface);
}

FireEffect::~FireEffect()
{
	delete[] palette;
	delete[] fire1;
	delete[] fire2;

}

void FireEffect::buildPalette()
{
	// setup some fire-like colours
	Shade_Pal(0, 23, 0, 0, 0, 32, 0, 64);
	Shade_Pal(24, 47, 32, 0, 64, 255, 0, 0);
	Shade_Pal(48, 63, 255, 0, 0, 255, 255, 0);
	Shade_Pal(64, 127, 255, 255, 0, 255, 255, 255);
	Shade_Pal(128, 255, 255, 255, 255, 255, 255, 255);
}

/*
* create a shade of colours in the palette from s to e
*/
void FireEffect::Shade_Pal(int s, int e, int r1, int g1, int b1, int r2, int g2, int b2)
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
void FireEffect::Heat(unsigned char* dst)
{
	int i, j;

	j = (rand() % 512);
	// add some random hot spots at the bottom of the buffer
	for (i = 0; i < j; i++)
	{
		dst[(screenWidth * 3 / 4 * (screenHeight - 3)) + (rand() % (screenWidth * 3))] = 255;
	}
}

/*
* smooth a buffer upwards, make sure not to read pixels that are outside of
* the buffer!
*/
void FireEffect::Blur_Up(unsigned char* src, unsigned char* dst)
{
	int offs = 0;
	unsigned char b;
	for (int j = 0; j < (screenHeight - 2); j++)
	{
		// set first pixel of the line to 0
		dst[offs] = 0; offs++;
		// calculate the filter for all the other pixels
		for (int i = 1; i < (screenWidth - 1); i++)
		{
			// calculate the average
			b = (int)(src[offs - 1] + src[offs + 1]
				+ src[offs + (screenWidth - 1)] + src[offs + (screenWidth)] + src[offs + (screenWidth + 1)]
				+ src[offs + ((screenWidth * 2) - 1)] + src[offs + (screenWidth * 2)] + src[offs + ((screenWidth * 2) + 1)]) / 8;
			// store the pixel
			dst[offs] = b;
			offs++;
		}
		// set last pixel of the line to 0
		dst[offs] = 0; offs++;
	}
	// clear the last 2 lines
	memset((void*)((long long)(dst)+offs), 0, screenWidth * 2);
}
