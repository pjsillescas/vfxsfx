#include "TerraEffect.h"

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <cmath>

#include "Clock.h"

TerraEffect::TerraEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title) : EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
}

void TerraEffect::init()
{
	SDL_Surface* temp = IMG_Load("texture.png");
	if (temp == NULL) {
		std::cout << "Image can be loaded! " << IMG_GetError();
		exit(1);
	}
	texture = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_ARGB8888, 0);

}

void TerraEffect::update(float deltaTime)
{
	int currentTime = Clock::getInstance().getCurrentTime();
	// setup the 3 control points of our plane, could do something much
	// more dynamic (with rotX also) but this looks good enough
	A = VECTOR((float)(currentTime) / 50, 8, (float)(currentTime) / 10);
	B = rotY(0.32) * VECTOR(256, 0, 0);
	C = rotY(0.32) * VECTOR(0, 0, 256);
}

void TerraEffect::render()
{
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));

	// draw plane
	DrawPlane(A, B, C);
}

TerraEffect::~TerraEffect()
{
	SDL_FreeSurface(texture);
}


/*
* draw a perspective correctly textured plane
*/
void TerraEffect::DrawPlane(VECTOR Bp, VECTOR Up, VECTOR Vp)
{
	// compute the 9 magic constants
	float Cx = Up[1] * Vp[2] - Vp[1] * Up[2],
		Cy = Vp[0] * Up[2] - Up[0] * Vp[2],
		// the 500 represents the distance of the projection plane
		// change it to modify the field of view
		Cz = (Up[0] * Vp[1] - Vp[0] * Up[1]) * 500,
		Ax = Vp[1] * Bp[2] - Bp[1] * Vp[2],
		Ay = Bp[0] * Vp[2] - Vp[0] * Bp[2],
		Az = (Vp[0] * Bp[1] - Bp[0] * Vp[1]) * 500,
		Bx = Bp[1] * Up[2] - Up[1] * Bp[2],
		By = Up[0] * Bp[2] - Bp[0] * Up[2],
		Bz = (Bp[0] * Up[1] - Up[0] * Bp[1]) * 500;
	// only render the lower part of the plane, looks ugly above
	SDL_LockSurface(surface);
	for (int j = (screenHeight / 2); j < screenHeight; j++)
	{
		// compute the (U,V) coordinates for the start of the line
		float a = Az + Ay * (j - (screenHeight / 2)) + Ax * -((screenWidth / 2) + 1),
			b = Bz + By * (j - (screenHeight / 2)) + Bx * -((screenWidth / 2) + 1),
			c = Cz + Cy * (j - (screenHeight / 2)) + Cx * -((screenWidth / 2) + 1),
			ic;
		// quick distance check, if it's too far reduce it
		if (fabs(c) > 65536) ic = 1 / c; else ic = 1 / 65536;
		// compute original (U,V)
		Uint32 u = (int)(a * 16777216 * ic),
			v = (int)(b * 16777216 * ic),
			// and the deltas we need to interpolate along this line
			du = (int)(16777216 * Ax * ic),
			dv = (int)(16777216 * Bx * ic);
		// setup the offsets in the buffers
		Uint8* dst;
		Uint8* initbuffer = (Uint8*)surface->pixels;
		int bpp = surface->format->BytesPerPixel;
		Uint8* imagebuffer = (Uint8*)texture->pixels;
		int bppImage = texture->format->BytesPerPixel;
		dst = initbuffer + j * surface->pitch;
		// start the loop
		for (int i = 0; i < screenWidth; i++)
		{
			// load texel and store pixel
			Uint8* p = (Uint8*)imagebuffer + ((v >> 16) & 0xff) * texture->pitch + ((u >> 16) & 0xff) * bppImage;
			// copy it to the screen
			*(Uint32*)dst = *(Uint32*)p;
			// next pixel
			dst += bppImage;
			// interpolate
			u += du;
			v += dv;
		}
	}
	SDL_UnlockSurface(surface);
}
