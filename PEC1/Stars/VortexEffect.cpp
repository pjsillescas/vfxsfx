#include "VortexEffect.h"


#include <stdlib.h>
#include <iostream>


// this record contains the information for one star
struct TVortexParticle
{
	float u; // x component of velocity
	float v; // y component of velocity
	float vorticity;
};

VortexEffect::VortexEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title) : EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
	// allocate memory for all our stars
	grid = new TVortexParticle[screenHeight * screenWidth];
}

static float getRandomNumber()
{
	float x= (float)rand() / RAND_MAX;
	//std::cout << "random " <<  x << std::endl;
	return x;
}

static int wrap(int i, int size)
{
	if (i < 0)
	{
		return size + i;
	}
	if (i >= size)
	{
		return i - size;
	}

	return i;
}

int VortexEffect::pixelToIndex(int i, int j) const
{
	//std::cout << "(" << i << "," << j << "): " << i * screenHeight + j << std::endl;
	return i * screenHeight + j;
}

void VortexEffect::calculateVorticity()
{
	//std::cout << "calc vorticity" << std::endl;
	for (int i = 0; i < screenWidth; i++)
	{
		for (int j = 0; j < screenHeight; j++)
		{
			float du_dy = (grid[pixelToIndex(wrap(i + 1, screenWidth) ,j)].u - grid[pixelToIndex(wrap(i - 1, screenWidth),j)].u) / 2.0;
			float dv_dx = (grid[pixelToIndex(i, wrap(j + 1, screenHeight))].v - grid[pixelToIndex(i, wrap(j - 1, screenHeight))].v) / 2.0;
			
			int k = pixelToIndex(i, j);
			grid[k].vorticity += du_dy - dv_dx;

			//std::cout << "(" << i << "," << j << "): " << k << std::endl;
			if (i == 0 && j == 0)
			{
				minVorticity = grid[k].vorticity;
				maxVorticity = grid[k].vorticity;
			}
			else
			{
				if (grid[k].vorticity < minVorticity)
				{
					minVorticity = grid[k].vorticity;
				}
				
				if (grid[k].vorticity > maxVorticity)
				{
					maxVorticity = grid[k].vorticity;
				}
			}
			//std::cout << "ok" << std::endl;
		}
	}

	std::cout << "min " << minVorticity << " max " << maxVorticity << std::endl;
}

const float VORTICITY_EPSILON = 0.0001f;
const float VORTICITY_CONF_PARAM = 0.1f;


// Apply vorticity confinement to the grid
void VortexEffect::applyVorticityConf(float deltaTime)
{
	deltaTime /= 1000.f;
	std::cout << deltaTime << std::endl;
	for (int i = 0; i < screenWidth; i++)
	{
		for (int j = 0; j < screenHeight; j++)
		{
			float dw_dx = (grid[pixelToIndex(wrap(i + 1, screenWidth) ,j)].vorticity - grid[pixelToIndex(wrap(i - 1, screenWidth) , j)].vorticity) / 2.0;
			float dw_dy = (grid[pixelToIndex(i ,wrap(j + 1, screenHeight))].vorticity - grid[pixelToIndex(i, wrap(j - 1, screenHeight))].vorticity) / 2.0;
			float length = sqrt(dw_dx * dw_dx + dw_dy * dw_dy) + VORTICITY_EPSILON;
			float nx = dw_dx / length;
			float ny = dw_dy / length;
			grid[pixelToIndex(i , j)].u += VORTICITY_CONF_PARAM * grid[pixelToIndex(i ,j)].vorticity * ny * deltaTime;
			grid[pixelToIndex(i , j)].v -= VORTICITY_CONF_PARAM * grid[pixelToIndex(i , j)].vorticity * nx * deltaTime;
		}
	}
}

void VortexEffect::init()
{
	//srand(42);
	
	for (int k = 0; k < screenWidth * screenHeight; k++)
	{
		
		grid[k].u = 0.0;
		grid[k].v = 0.0;
		grid[k].vorticity = getRandomNumber() * 2.f - 1.f;
		
		/*
		grid[k].u = 2.f * getRandomNumber() - 1.f;
		grid[k].v = 2.f * getRandomNumber() - 1.f;
		grid[k].vorticity = 0; // getRandomNumber() * 2.f - 1.f;
		*/
	}
}

void VortexEffect::update(float deltaTime) {
	// update all stars
	calculateVorticity();
	applyVorticityConf(deltaTime);
}

void VortexEffect::render() {
	//SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
	// update all stars
	for (int i = 0; i < screenWidth; i++)
	{
		for (int j = 0; j < screenHeight; j++)
		{
			int k = pixelToIndex(i ,j);
			// draw this star, with a colour depending on the plane
			unsigned int color = 0;

			float normalizedVorticity = (grid[k].vorticity - minVorticity) / (maxVorticity - minVorticity);
			unsigned char colorValue = 255 * normalizedVorticity;
			//color = 0xff + 256 * 256 * colorValue + 256 * colorValue + colorValue;
			color = 0xFF000000 | (colorValue << 16) | (colorValue << 8) | colorValue;
			putPixel(surface, i, j, color);
		}
	}
}

/*
* Set the pixel at (x, y) to the given value
* NOTE: The surface must be locked before calling this!
*/
void VortexEffect::putPixel(SDL_Surface* surface, int x, int y, Uint32 pixel) const
{
	// Clipping
	if ((x < 0) || (x >= screenWidth) || (y < 0) || (y >= screenHeight))
	{
		return;
	}

	int bpp = surface->format->BytesPerPixel;
	// Here p is the address to the pixel we want to set
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp) {
	case 1:
		*p = pixel;
		break;

	case 2:
		*(Uint16*)p = pixel;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			p[0] = (pixel >> 16) & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = pixel & 0xff;
		}
		else {
			p[0] = pixel & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = (pixel >> 16) & 0xff;
		}
		break;

	case 4:
		*(Uint32*)p = pixel;
		break;
	}
}

VortexEffect::~VortexEffect()
{
	// free memory
	delete[] grid;
}