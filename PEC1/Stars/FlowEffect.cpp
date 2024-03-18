#include "FlowEffect.h"

#include <stdlib.h>

// this record contains the information for one star
struct TFlowStar
{
	float x, y;             // position of the star
	unsigned char plane;    // remember which plane it belongs to
	float vx, vy;			// velocity
};

FlowEffect::FlowEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title) : EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
	// allocate memory for all our stars
	stars = new TFlowStar[MAXSTARS];
}

/*
const float STAR_MAX_SPEED = 0.1f;
const float RAND_SEED = 42;
const float FLOW_FORCE = 0.1f;
*/
const float STAR_MAX_SPEED = 0.1f;
const float RAND_SEED = 42;
const float FLOW_FORCE = 0.03f;

float getRandomNumber()
{
	return (float)rand() / RAND_MAX;
}

void FlowEffect::init() {

	flowVx = 0.1f;
	flowVy = 0.05f;

	//srand(RAND_SEED);

	for (int i = 0; i < MAXSTARS; i++)
	{
		/*
		stars[i].x = (float)(rand() % screenWidth);
		stars[i].y = (float)(rand() % screenHeight);
		stars[i].plane = rand() % 3;     // star colour between 0 and 2
		*/

		stars[i].x = (float)(rand() % screenWidth);
		stars[i].y = (float)(rand() % screenHeight);
		stars[i].vx = getRandomNumber() * 2 * STAR_MAX_SPEED - STAR_MAX_SPEED;
		stars[i].vy = getRandomNumber() * 2 * STAR_MAX_SPEED - STAR_MAX_SPEED;
		stars[i].plane = rand() % 3;     // star colour between 0 and 2
	}

}

float getRandomFlowFactorModifier(float flowFactor)
{
	float sign = (getRandomNumber() < 0.5f) ? 1 : -1;
	return flowFactor * sign * getRandomNumber();
}

float getRandomFlowFactorModifier()
{
	float flowFactor = 0.001f;
	return getRandomFlowFactorModifier(flowFactor);
}

void FlowEffect::update(float deltaTime) {
	// update all stars

	flowVx += getRandomFlowFactorModifier();
	flowVy += getRandomFlowFactorModifier();

	float flowForce = FLOW_FORCE * (1 + getRandomFlowFactorModifier(3.f));


	for (int i = 0; i < MAXSTARS; i++)
	{
		/*
		// move this star right, determine how fast depending on which
		// plane it belongs to
		stars[i].x += (deltaTime + (float)stars[i].plane) * 0.15f;
		// check if it's gone out of the right of the screen
		if (stars[i].x > screenWidth)
		{
			// if so, make it return to the left
			stars[i].x = -(float)(rand() % screenWidth);
			// and randomly change the y position
			stars[i].y = (float)(rand() % screenHeight);
		}
		*/

		// update velocity
		float fx = (flowVx - stars[i].vx) * flowForce;
		float fy = (flowVy - stars[i].vy) * flowForce;
		deltaTime /= 10.f;
		stars[i].vx += fx * deltaTime;
		stars[i].vy += fy * deltaTime;

		// update position
		//stars[i].x += stars[i].vx * deltaTime;
		//stars[i].y += stars[i].vy * deltaTime;

		stars[i].x = clampValueCircular(stars[i].x + stars[i].vx * deltaTime, 0, screenWidth);
		stars[i].y = clampValueCircular(stars[i].y + stars[i].vy * deltaTime, 0, screenHeight);
	}
}

float FlowEffect::clampValueCircular(float value, float minValue, float maxValue)
{
	if (value < minValue)
	{
		return maxValue - 1;
	}
	
	if (value >= maxValue)
	{
		return minValue;
	}

	return value;
}

void FlowEffect::render() {
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
	// update all stars
	for (int i = 0; i < MAXSTARS; i++)
	{
		// draw this star, with a colour depending on the plane
		unsigned int color = 0;
		switch (1 + stars[i].plane) {
		case 1:
			color = 0xFF606060; // dark grey
			break;
		case 2:
			color = 0xFFC2C2C2; // light grey
			break;
		case 3:
			color = 0xFFFFFFFF; // white
			break;
		}
		putPixel(surface, (int)stars[i].x, (int)stars[i].y, color);
	}
}

/*
* Set the pixel at (x, y) to the given value
* NOTE: The surface must be locked before calling this!
*/
void FlowEffect::putPixel(SDL_Surface* surface, int x, int y, Uint32 pixel) const
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

FlowEffect::~FlowEffect()
{
	// free memory
	delete[] stars;
}