#include "FlockingEffect.h"

#include <iostream>
#include <stdlib.h>
#include <math.h>

// https://medium.com/@pramodayajayalath/flocking-algorithm-simulating-collective-behavior-in-nature-inspired-systems-dc6d7fb884cc

//const int MAX_PARTICLES = 150;

const int MAX_PARTICLES = 500;

/*
const float MAX_SPEED = 2.f;
const float PERCEPTION_RADIUS = 50.f;
const float SEPARATION_DISTANCE = 25.f;
*/

const float MAX_SPEED = 3.f;
const float PERCEPTION_RADIUS = 50.f;
const float SEPARATION_DISTANCE = 50.f;

const float PERCEPTION_RADIUS2 = PERCEPTION_RADIUS * PERCEPTION_RADIUS;
const float SEPARATION_DISTANCE2 = SEPARATION_DISTANCE * SEPARATION_DISTANCE;


FlockingEffect::FlockingEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title) : EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
	// allocate memory for all our stars
	particles = new TParticle[MAX_PARTICLES];
}

float FlockingEffect::getRandomFloat()
{
	return 2.f * ((float)rand() / RAND_MAX) - 1.f;
}

void FlockingEffect::init() {
	// randomly generate some stars
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		particles[i].x = (float)(rand() % screenWidth);
		particles[i].y = (float)(rand() % screenHeight);
		particles[i].vx = MAX_SPEED * getRandomFloat();
		particles[i].vy = MAX_SPEED * getRandomFloat();
		particles[i].color = 0xFF000000 | ((rand() % 256) << 16) | ((rand() % 256) << 8) | (rand() % 256);
		//std::cout << i << " (" << particles[i].vx << "," << particles[i].vy << ")" << std::endl;
	}
}

void FlockingEffect::wrapEdges(TParticle& particle)
{
	// Check and update the entity's position to wrap around the screen edges
	if (particle.x < 0)
	{
		particle.x = screenWidth - 1;
	}
	
	if (particle.y < 0)
	{
		particle.y = screenHeight - 1;
	}

	if (particle.x >= screenWidth)
	{
		particle.x = 0;
	}
	
	if (particle.y >= screenHeight)
	{
		particle.y = 0;
	}
}

float getMinimum3(float d1, float d2, float d3)
{
	if (d1 > d2)
	{
		return (d2 > d3) ? d3 : d2;
	}
	else
	{
		return (d1 > d3) ? d3 : d1;
	}
}

float FlockingEffect::getDy2(float x1, float y1, float x2, float y2)
{
	float dy1 = fabs(y1 - y2);
	float dy2 = fabs(y1 - y2 + screenHeight);
	float dy3 = fabs(y1 - y2 - screenHeight);

	return getMinimum3(dy1, dy2, dy3);
}

float FlockingEffect::getDx2(float x1, float y1, float x2, float y2)
{
	float dx1 = fabs(x1 - x2);
	float dx2 = fabs(x1 - x2 + screenWidth);
	float dx3 = fabs(x1 - x2 - screenWidth);

	return getMinimum3(dx1, dx2, dx3);
}

void FlockingEffect::updateParticle(TParticle& particle, int i)
{
	if (i == 0)
	{
		std::cout << "(" << particle.x << "," << particle.y << ") + (" << particle.vx << "," << particle.vy << ") = " << std::endl;
	}
	
	particle.x += particle.vx;
	particle.y += particle.vy;

	if (i == 0)
	{
		std::cout << "(" << particle.x << "," << particle.y << ")" << std::endl;
	}

	wrapEdges(particle);

	float avgX = 0;
	float avgY = 0;

	float avgVx = 0;
	float avgVy = 0;
	
	float avgSeparationX = 0;
	float avgSeparationY = 0;
	
	int numNeighbors = 0;

	for (int k = 0; k< MAX_PARTICLES;k++)
	{
		if (k == i)
		{
			continue;
		}

		TParticle* currentParticle = &(particles[k]);

		float dX = getDx2(currentParticle->x, currentParticle->y, particle.x, particle.y);
		float dY = getDy2(currentParticle->x, currentParticle->y, particle.x, particle.y);
		float distance2 = dX * dX + dY * dY;
		if (distance2 < PERCEPTION_RADIUS2)
		{
			avgX += currentParticle->x;
			avgY += currentParticle->y;
			
			avgVx += currentParticle->vx;
			avgVy += currentParticle->vy;

			if (distance2 < SEPARATION_DISTANCE2)
			{
				avgSeparationX += - dX / distance2 * (k == 0 ? 1000.f : 1.f);
				avgSeparationY += - dY / distance2 * (k == 0 ? 1000.f : 1.f);
			}

			numNeighbors++;
		}
	}

	if (numNeighbors > 0)
	{
		avgX /= (float) numNeighbors;
		avgY /= (float) numNeighbors;

		avgVx /= (float) numNeighbors;
		avgVy /= (float) numNeighbors;

		avgSeparationX /= (float) numNeighbors;
		avgSeparationY /= (float) numNeighbors;
	}

	// velocity update
	particle.vx += avgVx * 0.02f;
	particle.vy += avgVy * 0.02f;

	particle.vx += avgX * 0.01f;
	particle.vy += avgY * 0.01f;

	particle.vx -= avgSeparationX * 0.03f;
	particle.vy -= avgSeparationY * 0.03f;

	// velocity normalization to MAX_SPEED
	float speed = sqrtf(particle.vx * particle.vx + particle.vy * particle.vy);
	if (speed > 0.1)
	{
		particle.vx = particle.vx * MAX_SPEED / speed;
		particle.vy = particle.vy * MAX_SPEED / speed;
	}
}

float phi = 0;
float deltaPhi = M_PI / 40.f;

void FlockingEffect::updateLeadParticle(int i, float deltaTime) {
	const float r = (screenHeight / 2.f) * 0.8f;
	TParticle* leader = &particles[i];

	leader->x = r * cosf(phi) + (float) screenWidth / 2.f;
	leader->y = r * sinf(phi) + (float) screenHeight / 2.f;

	phi += deltaPhi;

	if (phi > 2 * M_PI)
	{
		phi = 0;
	}
}

void FlockingEffect::update(float deltaTime) {
	
	updateLeadParticle(0, deltaTime);
	// update all stars
	for (int i = 1; i < MAX_PARTICLES; i++)
	{
		updateParticle(particles[i], i);
	}
}

void FlockingEffect::render() {
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
	// update all stars
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		int x = (int)particles[i].x;
		int y = (int)particles[i].y;
		Uint32 color = particles[i].color;

		putPixel(surface, x, y, color);
		putPixel(surface, x + 1, y, color);
		putPixel(surface, x, y + 1, color);
		putPixel(surface, x + 1, y + 1, color);
	}
}

/*
* Set the pixel at (x, y) to the given value
* NOTE: The surface must be locked before calling this!
*/
void FlockingEffect::putPixel(SDL_Surface* surface, int x, int y, Uint32 pixel) const
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

FlockingEffect::~FlockingEffect()
{
	// free memory
	delete[] particles;
}