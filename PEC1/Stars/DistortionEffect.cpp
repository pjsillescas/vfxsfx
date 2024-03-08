#include "DistortionEffect.h"

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <cmath>
#include "Clock.h"


DistortionEffect::DistortionEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout) : EffectTemplate(surface, screenHeight, screenWidth, timeout)
{
	// load the background image
	image = loadImage("uoc.png");
}

DistortionEffect::DistortionEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, const char* fileName) : EffectTemplate(surface, screenHeight, screenWidth, timeout)
{
	// load the background image
	image = loadImage(fileName);
}

void DistortionEffect::init() {

	// two buffers
	dispX = new char[screenWidth * screenHeight * 4];
	dispY = new char[screenWidth * screenHeight * 4];
	// create two distortion functions
	precalculate();
	
}

void DistortionEffect::update(float deltaTime) {
	int currentTime = Clock::getInstance().getCurrentTime();
	// move distortion buffer
	windowx1 = (screenWidth / 2) + (int)(((screenWidth / 2) - 1) * cos((double)currentTime / 2050));
	windowx2 = (screenWidth / 2) + (int)(((screenWidth / 2) - 1) * sin((double)-currentTime / 1970));
	windowy1 = (screenHeight / 2) + (int)(((screenHeight / 2) - 1) * sin((double)currentTime / 2310));
	windowy2 = (screenHeight / 2) + (int)(((screenHeight / 2) - 1) * cos((double)-currentTime / 2240));
}

void DistortionEffect::render() {
	int currentTime = Clock::getInstance().getCurrentTime();
	
	// draw the effect showing without filter and with filter each 2 seconds
	if ((currentTime & 2048) < 1024) {
		Distort();
	}
	else {
		Distort_Bili();
	}
}

DistortionEffect::~DistortionEffect()
{
	delete[] dispX;
	delete[] dispY;
	SDL_FreeSurface(image);
}

/*
* calculate a distorion function for X and Y in 5.3 fixed point
*/
void DistortionEffect::precalculate()
{
	int i, j, dst;
	dst = 0;
	for (j = 0; j < (screenHeight * 2); j++)
	{
		for (i = 0; i < (screenWidth * 2); i++)
		{
			float x = (float)i;
			float y = (float)j;
			// notice the values contained in the buffers are signed
			// i.e. can be both positive and negative
			// also notice we multiply by 8 to get 5.3 fixed point distortion
			// coefficients for our bilinear filtering
			dispX[dst] = (signed char)(8 * (2 * sin(x / 20) + sin(x * y / 2000)));
			dispY[dst] = (signed char)(8 * (cos(x / 31) + cos(x * y / 1783)));

			// Uncomment this to take another beautiful distorsion
			/*
			dispX[dst] = (signed char)(8 * (2 * (sin(x / 20) + sin(x*y / 2000)
				+ sin((x + y) / 100) + sin((y - x) / 70) + sin((x + 4 * y) / 70)
				+ 2 * sin(hypot(256 - x, (150 - y / 8)) / 40))));
			dispY[dst] = (signed char)(8 * ((cos(x / 31) + cos(x*y / 1783) +
				+2 * cos((x + y) / 137) + cos((y - x) / 55) + 2 * cos((x + 8 * y) / 57)
				+ cos(hypot(384 - x, (274 - y / 9)) / 51))));
			*/
			dst++;
		}
	}
}

/*
*   copy an image to the screen with added distortion.
*   no bilinear filtering.
*/
void DistortionEffect::Distort()
{
	// setup the offsets in the buffers
	Uint8* dst;
	int	src1 = windowy1 * (screenWidth * 2) + windowx1,
		src2 = windowy2 * (screenWidth * 2) + windowx2;
	int dX, dY;
	Uint8* initbuffer = (Uint8*)surface->pixels;
	int bpp = surface->format->BytesPerPixel;
	Uint8* imagebuffer = (Uint8*)image->pixels;
	int bppImage = image->format->BytesPerPixel;

	SDL_LockSurface(surface);
	// loop for all lines
	for (int j = 0; j < screenHeight; j++)
	{
		dst = initbuffer + j * surface->pitch;
		// for all pixels
		for (int i = 0; i < screenWidth; i++)
		{
			// get distorted coordinates, use the integer part of the distortion
			// buffers and truncate to closest texel
			dY = j + (dispY[src1] >> 3);
			dX = i + (dispX[src2] >> 3);
			// check the texel is valid
			if ((dY >= 0) && (dY < (screenHeight - 1)) && (dX >= 0) && (dX < (screenWidth - 1)))
			{
				// copy it to the screen
				Uint8* p = (Uint8*)imagebuffer + dY * image->pitch + dX * bppImage;
				*(Uint32*)dst = *(Uint32*)p;
			}
			// otherwise, just set it to black
			else
			{
				*(Uint32*)dst = 0;
			}
			
			// next pixel
			dst += bpp;
			
			src1++; src2++;
		}
		// next line
		src1 += screenWidth;
		src2 += screenWidth;
	}
	SDL_UnlockSurface(surface);
}

/*
*   copy an image to the screen with added distortion.
*   with bilinear filtering.
*/
void DistortionEffect::Distort_Bili()
{
	// setup the offsets in the buffers
	Uint8* dst;
	int src1 = windowy1 * (screenWidth * 2) + windowx1,
		src2 = windowy2 * (screenWidth * 2) + windowx2;
	int dX, dY, cX, cY;
	Uint8* initbuffer = (Uint8*)surface->pixels;
	int bpp = surface->format->BytesPerPixel;
	Uint32* imagebuffer = (Uint32*)image->pixels;
	int bppImage = image->format->BytesPerPixel;

	SDL_LockSurface(surface);
	// loop for all lines
	for (int j = 0; j < screenHeight; j++)
	{
		dst = initbuffer + j * surface->pitch;
		// for all pixels
		for (int i = 0; i < screenWidth; i++)
		{
			// get distorted coordinates, by using the truncated integer part
			// of the distortion coefficients
			dY = j + (dispY[src1] >> 3);
			dX = i + (dispX[src2] >> 3);
			// get the linear interpolation coefficiants by using the fractionnal
			// part of the distortion coefficients
			cY = dispY[src1] & 0x7;
			cX = dispX[src2] & 0x7;
			// check if the texel is valid
			if ((dY >= 0) && (dY < (screenHeight - 1)) && (dX >= 0) && (dX < (screenWidth - 1)))
			{
				// load the 4 surrounding texels and multiply them by the
				// right bilinear coefficients, then get rid of the fractionnal
				// part by shifting right by 6
				SDL_Color Colorvalues[4];
				SDL_GetRGB(*(Uint32*)((Uint8*)image->pixels + dY * image->pitch + dX * bppImage), image->format, &Colorvalues[0].r, &Colorvalues[0].g, &Colorvalues[0].b);
				SDL_GetRGB(*(Uint32*)((Uint8*)image->pixels + dY * image->pitch + (dX + 1) * bppImage), image->format, &Colorvalues[1].r, &Colorvalues[1].g, &Colorvalues[1].b);
				SDL_GetRGB(*(Uint32*)((Uint8*)image->pixels + (dY + 1) * image->pitch + dX * bppImage), image->format, &Colorvalues[2].r, &Colorvalues[2].g, &Colorvalues[2].b);
				SDL_GetRGB(*(Uint32*)((Uint8*)image->pixels + (dY + 1) * image->pitch + (dX + 1) * bppImage), image->format, &Colorvalues[3].r, &Colorvalues[3].g, &Colorvalues[3].b);
				Uint32 Color[3]; // 0=R  1=G  2=B
				Color[0] = (Colorvalues[0].r * (0x8 - cX) * (0x8 - cY) +
					Colorvalues[1].r * cX * (0x8 - cY) +
					Colorvalues[2].r * (0x8 - cX) * cY +
					Colorvalues[3].r * cX * cY) >> 6;
				Color[1] = (Colorvalues[0].g * (0x8 - cX) * (0x8 - cY) +
					Colorvalues[1].g * cX * (0x8 - cY) +
					Colorvalues[2].g * (0x8 - cX) * cY +
					Colorvalues[3].g * cX * cY) >> 6;
				Color[2] = (Colorvalues[0].b * (0x8 - cX) * (0x8 - cY) +
					Colorvalues[1].b * cX * (0x8 - cY) +
					Colorvalues[2].b * (0x8 - cX) * cY +
					Colorvalues[3].b * cX * cY) >> 6;
				Uint32 resultColor = SDL_MapRGB(image->format, Color[0], Color[1], Color[2]);
				*(Uint32*)dst = resultColor;
			}
			// otherwise, just make it black
			else *(Uint32*)dst = 0;
			dst += bpp;
			src1++; src2++;
		}
		// next line
		src1 += screenWidth;
		src2 += screenWidth;
	}
	SDL_UnlockSurface(surface);
}
