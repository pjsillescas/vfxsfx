#include "PlasmaEffect.h"

#include "Clock.h"


PlasmaEffect::PlasmaEffect(SDL_Surface* surface, int screenHeight, int screenWidth) : EffectTemplate(surface, screenHeight, screenWidth)
{
	//plasma1 = (unsigned char*)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * 4);
	//plasma2 = (unsigned char*)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * 4);
	plasma1 = new unsigned char[screenWidth * screenHeight * 4];
	plasma2 = new unsigned char[screenWidth * screenHeight * 4];

	src1 = 0;
	src2 = 0;

	palette = new RGBColor[MAX_PALETTE];
}

PlasmaEffect::~PlasmaEffect()
{
	delete[] palette;
	delete[] plasma1;
	delete[] plasma2;

}

bool PlasmaEffect::isEnded()
{
	return false;
}

void PlasmaEffect::init()
{
	int dst = 0;
	for (int j = 0; j < (screenHeight * 2); j++)
	{
		for (int i = 0; i < (screenWidth * 2); i++)
		{
			plasma1[dst] = (unsigned char)(64 + 63 * (sin((double)hypot(screenHeight - j, screenWidth - i) / 16)));
			plasma2[dst] = (unsigned char)(64 + 63 * sin((float)i / (37 + 15 * cos((float)j / 74)))
				* cos((float)j / (31 + 11 * sin((float)i / 57))));
			dst++;
		}
	}
}

void PlasmaEffect::buildPalette() {
	int currentTime = Clock::getInstance().getCurrentTime();
	for (int i = 0; i < MAX_PALETTE; i++)
	{
		palette[i].R = (unsigned char)(128 + 127 * cos(i * M_PI / 128 + (double)currentTime / 740));
		palette[i].G = (unsigned char)(128 + 127 * sin(i * M_PI / 128 + (double)currentTime / 630));
		palette[i].B = (unsigned char)(128 - 127 * cos(i * M_PI / 128 + (double)currentTime / 810));
	}
}


void PlasmaEffect::update(float deltaTime)
{
	int currentTime = Clock::getInstance().getCurrentTime();
	// setup some nice colours, different every frame
	// this is a palette that wraps around itself, with different period sine
	// functions to prevent monotonous colours
	buildPalette();

	// move plasma with more sine functions :)
	int Windowx1 = (screenWidth / 2) + (int)((((double) screenWidth / 2) - 1) * cos((double)currentTime / 970));
	int Windowx2 = (screenWidth / 2) + (int)((((double) screenWidth / 2) - 1) * sin((double)-currentTime / 1140));
	int Windowy1 = (screenHeight / 2) + (int)((((double) screenHeight / 2) - 1) * sin((double)currentTime / 1230));
	int Windowy2 = (screenHeight / 2) + (int)((((double) screenHeight / 2) - 1) * cos((double)-currentTime / 750));
	// we only select the part of the precalculated buffer that we need
	src1 = Windowy1 * (screenWidth * 2) + Windowx1;
	src2 = Windowy2 * (screenWidth * 2) + Windowx2;
}

void PlasmaEffect::render()
{
	Uint8* dst;
	long i, j;
	Uint8* initbuffer = (Uint8*)surface->pixels;
	int bpp = surface->format->BytesPerPixel;

	SDL_LockSurface(surface);

	dst = initbuffer;
	for (j = 0; j < screenHeight; j++)
	{
		dst = initbuffer + j * surface->pitch;
		for (i = 0; i < screenWidth; i++)
		{
			// plot the pixel as a sum of all our plasma functions
			unsigned int Color = 0;
			int indexColor = (plasma1[src1] + plasma2[src2]) % 256;
			Color = 0xFF000000 + (palette[indexColor].R << 16) + (palette[indexColor].G << 8) + palette[indexColor].B;
			*(Uint32*)dst = Color;

			dst += bpp;
			src1++; src2++;
		}
		// get the next line in the precalculated buffers
		src1 += screenWidth; src2 += screenWidth;
	}
	SDL_UnlockSurface(surface);
}


