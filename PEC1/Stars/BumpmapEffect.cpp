#include "BumpmapEffect.h"

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <cmath>
#include "Clock.h"


BumpmapEffect::BumpmapEffect(SDL_Surface* surface, int screenHeight, int screenWidth) : EffectTemplate(surface, screenHeight, screenWidth)
{
	// contains the image of the spotlight
	light = new unsigned char[(long long) LIGHT_PIXEL_RES * LIGHT_PIXEL_RES];
	bump = NULL;
	image = NULL;
	windowx1 = 0;
	windowy1 = 0;
	windowx2 = 0;
	windowy2 = 0;
	windowZ = 0;
}

void BumpmapEffect::init()
{
	IMG_Init(IMG_INIT_PNG);

	// generate the light pattern
	computeLight();
	// load the color image
	SDL_Surface* temp = IMG_Load("wall.png");
	if (temp == NULL) {
		std::cout << "Image can be loaded! " << IMG_GetError();
		exit(1);
	}
	image = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_ARGB8888, 0);
	// load the bump image
	temp = IMG_Load("bump.png");
	if (temp == NULL) {
		std::cout << "Image can be loaded! " << IMG_GetError();
		exit(1);
	}
	bump = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_ARGB8888, 0);
}

void BumpmapEffect::update(float deltaTime) {
	// move the light.... more sines :)
	int currentTime = Clock::getInstance().getCurrentTime();

	windowx1 = (int)((LIGHT_PIXEL_RES / 2) * cos((double)currentTime / 640)) - 20;
	windowy1 = (int)((LIGHT_PIXEL_RES / 2) * sin((double)-currentTime / 450)) + 20;
	windowx2 = (int)((LIGHT_PIXEL_RES / 2) * cos((double)-currentTime / 510)) - 20;
	windowy2 = (int)((LIGHT_PIXEL_RES / 2) * sin((double)currentTime / 710)) + 20;
	windowZ = 192 + (int)(((LIGHT_PIXEL_RES / 2) - 1) * sin((double)currentTime / 1120));
}

void BumpmapEffect::render() {
	// draw the bumped image
	doBump();
}

BumpmapEffect::~BumpmapEffect()
{
	delete[] light;

	SDL_FreeSurface(image);
	SDL_FreeSurface(bump);

}

/*
* generate a "spot light" pattern
*/
void BumpmapEffect::computeLight()
{
	for (int j = 0; j < LIGHT_PIXEL_RES; j++)
		for (int i = 0; i < LIGHT_PIXEL_RES; i++)
		{
			// get the distance from the centre
			float dist = (float)((LIGHT_PIXEL_RES / 2) - i) * ((LIGHT_PIXEL_RES / 2) - i) + ((LIGHT_PIXEL_RES / 2) - j) * ((LIGHT_PIXEL_RES / 2) - j);
			if (fabs(dist) > 1) dist = sqrt(dist);
			// then fade if according to the distance, and a random coefficient
			int c = (int)(LIGHTSIZE * dist) + (rand() & 7) - 3;
			// clip it
			if (c < 0) c = 0;
			if (c > 255) c = 255;
			// and store it
			light[(j * LIGHT_PIXEL_RES) + i] = 255 - c;
		}
}

/*
* this needs a bump map and a colour map, and 2 light coordinates
* it computes the output colour with the look up table
*/
void BumpmapEffect::doBump()
{
	int i, j, px, py, x, y, c;
	// setup the offsets in the buffers
	Uint8* dst;
	Uint8* initbuffer = (Uint8*)surface->pixels;
	int bpp = surface->format->BytesPerPixel;
	Uint32* imagebuffer = (Uint32*)image->pixels;
	int bppImage = image->format->BytesPerPixel;
	Uint32* bumpbuffer = (Uint32*)bump->pixels;
	int bppBump = bump->format->BytesPerPixel;

	// we skip the first line since there are no pixels above
	// to calculate the slope with
	// loop for all the other lines
	SDL_LockSurface(surface);
	for (j = 1; j < screenHeight; j++)
	{
		// likewise, skip first pixel since there are no pixels on the left
		dst = initbuffer + j * surface->pitch;
		*(Uint32*)dst = 0;
		for (i = 1; i < screenWidth; i++)
		{
			// calculate coordinates of the pixel we need in light map
			// given the slope at this point, and the zoom coefficient
			SDL_Color Colors[3]; // 0=left pixel 1=center pixel 2=up pixel
			SDL_GetRGB(*(Uint32*)((Uint8*)image->pixels + j * image->pitch + (i - 1) * bppImage), image->format, &Colors[0].r, &Colors[0].g, &Colors[0].b);
			SDL_GetRGB(*(Uint32*)((Uint8*)image->pixels + j * image->pitch + i * bppImage), image->format, &Colors[1].r, &Colors[1].g, &Colors[1].b);
			SDL_GetRGB(*(Uint32*)((Uint8*)image->pixels + (j - 1) * image->pitch + i * bppImage), image->format, &Colors[2].r, &Colors[2].g, &Colors[2].b);
			px = (i * windowZ >> 8) + Colors[0].r - Colors[1].r;
			py = (j * windowZ >> 8) + Colors[2].r - Colors[1].r;
			// add the movement of the first light
			x = px + windowx1;
			y = py + windowy1;
			// check if the coordinates are inside the light buffer
			if ((y >= 0) && (y < LIGHT_PIXEL_RES) && (x >= 0) && (x < LIGHT_PIXEL_RES))
			{
				// if so get the pixel
				c = light[(y * LIGHT_PIXEL_RES) + x];
				// otherwise assume intensity 0
			}
			else
			{
				c = 0;
			}
			// now do the same for the second light
			x = px + windowx2;
			y = py + windowy2;
			// this time we add the light's intensity to the first value
			if ((y >= 0) && (y < LIGHT_PIXEL_RES) && (x >= 0) && (x < LIGHT_PIXEL_RES))
			{
				c += light[(y * LIGHT_PIXEL_RES) + x];
			}
			// make sure it's not too big
			if (c > 255)
			{
				c = 255;
			}
			// look up the colour multiplied by the light coeficient
			SDL_Color ColorBump; // 0=left pixel 1=center pixel 2=up pixel
			SDL_GetRGB(*(Uint32*)((Uint8*)bump->pixels + j * bump->pitch + i * bppBump), bump->format, &ColorBump.r, &ColorBump.g, &ColorBump.b);

			Uint32 Color[3]; // 0=R  1=G  2=B
			Color[0] = (Uint32)((((Colors[0].r * ColorBump.r) / 255)) * (c / 255.0f));
			Color[1] = (Uint32)((((Colors[0].g * ColorBump.g) / 255)) * (c / 255.0f));
			Color[2] = (Uint32)((((Colors[0].b * ColorBump.b) / 255)) * (c / 255.0f));
			if (Color[0] > 255) Color[0] = 255;
			if (Color[1] > 255) Color[1] = 255;
			if (Color[2] > 255) Color[2] = 255;
			Uint32 resultColor = SDL_MapRGB(image->format, Color[0], Color[1], Color[2]);
			*(Uint32*)dst = resultColor;
			dst += bpp;
		}
	}
	SDL_UnlockSurface(surface);
}



