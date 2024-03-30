#include "FractalPec1Effect.h"

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <cmath>
#include "Clock.h"


FractalPec1Effect::FractalPec1Effect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title) : EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
	// allocate memory for our fractal
	frac1 = new unsigned char[screenWidth * screenHeight * 4];
	frac2 = new unsigned char[screenWidth * screenHeight * 4];

	di = 0;
	dr = 0;
	pr = 0;
	pi = 0;
	sr = 0;
	si = 0;
	offs = 0;
	fractmp = 0;
	i = 0;
	j = 0;
}

void FractalPec1Effect::init()
{
	// calculate the first fractal
	StartFrac(OR - zx, OI - zy, OR + zx, OI + zy);
	for (j = 0; j < (screenHeight / 2); j++)
	{
		ComputeFrac();
	}

	DoneFrac();

	// adjust zooming coefficient for next view
	zx *= (zoom_in) ? ZOOM_IN_FACTOR : ZOOM_OUT_FACTOR;
	zy *= (zoom_in) ? ZOOM_IN_FACTOR : ZOOM_OUT_FACTOR;

	// start calculating the next fractal
	StartFrac(OR - zx, OI - zy, OR + zx, OI + zy);
	j = 0;
}

void FractalPec1Effect::buildPalette()
{
	int currentTime = Clock::getInstance().getCurrentTime();

	for (int i = 0; i < 256; i++)
	{
		palette[i].R = (unsigned char)(128 + 127 * cos(i * M_PI / 128 + (double)currentTime / 740));
		palette[i].G = (unsigned char)(128 + 127 * sin(i * M_PI / 128 + (double)currentTime / 630));
		palette[i].B = (unsigned char)(128 - 127 * cos(i * M_PI / 128 + (double)currentTime / 810));
	}
}

void FractalPec1Effect::update(float deltaTime)
{
	buildPalette();
	if (j < (screenHeight / 2))
	{
		j++;
		// calc another few lines
		ComputeFrac();
	}
	else
	{
		// adjust zooming coefficient for next view
		if (zoom_in)
		{
			zx *= ZOOM_IN_FACTOR;
			zy *= ZOOM_IN_FACTOR;
		}
		else
		{
			zx *= ZOOM_OUT_FACTOR;
			zy *= ZOOM_OUT_FACTOR;
		}
		j = 0;
		// start calculating the next fractal
		StartFrac(OR - zx, OI - zy, OR + zx, OI + zy);
		// one more image displayed
		k++;
		// check if we've gone far enough
		if (k % 38 == 0)
		{
			// if so, reverse direction
			zoom_in = !zoom_in;
			if (zoom_in)
			{
				zx *= ZOOM_IN_FACTOR;
				zy *= ZOOM_IN_FACTOR;
			}
			else
			{
				zx *= ZOOM_OUT_FACTOR;
				zy *= ZOOM_OUT_FACTOR;
			}
			// and make sure we use the same fractal again, in the other direction
			fractmp = frac1;
			frac1 = frac2;
			frac2 = fractmp;
		}

		DoneFrac();
	}
}

void FractalPec1Effect::render()
{
	// display the old fractal, zooming in or out
	Zoom((zoom_in) ? (double)j / (screenHeight / 2) : 1.0f - (double)j / (screenHeight / 2));
	
	// select some new colours
	//updatePalette(k * 100 + j);
}

FractalPec1Effect::~FractalPec1Effect()
{
	delete[] frac1;
	delete[] frac2;
}


void FractalPec1Effect::StartFrac(double _sr, double _si, double er, double ei)
{
	// compute deltas for interpolation in complex plane
	dr = (er - _sr) / (screenWidth * 2);
	di = (ei - _si) / (screenHeight * 2);
	// remember start values
	pr = _sr;
	pi = _si;
	sr = _sr;
	si = _si;
	offs = 0;
}

/*
* compute 4 lines of fracal
*/
void FractalPec1Effect::ComputeFrac()
{
	for (int j = 0; j < 4; j++)
	{
		pr = sr;
		for (int i = 0; i < (screenWidth * 2); i++)
		{
			unsigned char c = 0;
			double vi = pi, vr = pr, nvi, nvr;
			// loop until distance is above 2, or counter hits limit
			while ((vr * vr + vi * vi < 4) && (c < 255))
			{
				// compute Z(n+1) given Z(n)
				nvr = vr * vr - vi * vi + pr;
				nvi = 2 * vi * vr + pi;

				// that becomes Z(n)
				vi = nvi;
				vr = nvr;

				// increment counter
				c++;
			}
			// store colour
			frac1[offs] = c;
			offs++;
			// interpolate X
			pr += dr;
		}
		// interpolate Y
		pi += di;
	}
}

/*
* finished the computation, swap buffers
*/
void FractalPec1Effect::DoneFrac()
{
	fractmp = frac1;
	frac1 = frac2;
	frac2 = fractmp;
}

/*
* the zooming procedure
* takes a double screen bitmap, and scales it to screen size given the zooming coef
*/
void FractalPec1Effect::Zoom(double z)
{
	// setup the offsets in the buffers
	Uint8* dst;
	Uint8* initbuffer = (Uint8*)surface->pixels;
	int bpp = surface->format->BytesPerPixel;

	// what's the size of rectangle in the source image we want to display
	int width = (int)(((screenWidth * 2) << 16) / (256.0f * (1 + z))) << 8,
		height = (int)(((screenHeight * 2) << 16) / (256.0f * (1 + z))) << 8,
		// where do we start our interpolation
		startx = (((screenWidth * 2) << 16) - width) >> 1,
		starty = (((screenHeight * 2) << 16) - height) >> 1,
		// get our deltas
		deltax = width / screenWidth,
		deltay = height / screenHeight,
		px, py = starty;

	for (int j = 0; j < screenHeight; j++)
	{
		// set start value
		px = startx;
		dst = initbuffer + j * surface->pitch;
		for (int i = 0; i < screenWidth; i++)
		{
			unsigned int Color = 0;
			int indexColor = frac2[(py >> 16) * (screenWidth * 2) + (px >> 16)]; // Direct Pixel color
			Color = 0xFF000000 + (palette[indexColor].R << 16) + (palette[indexColor].G << 8) + palette[indexColor].B;
			*(Uint32*)dst = Color;
			// interpolate X
			px += deltax;
			dst += bpp;
		}
		// interpolate Y
		py += deltay;
	}
}
