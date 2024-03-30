#pragma once

#include "EffectTemplate.h"

// this record contains the information for one star
struct TParticle
{
	float x, y;             // Particle position
	float vx, vy;			// Particle velocity
	Uint32 color;
};

class FlockingEffect : public EffectTemplate
{
private:
	// this is a pointer to an array of stars
	TParticle* particles = NULL;

	void putPixel(SDL_Surface* surface, int x, int y, Uint32 pixel) const;

public:

	FlockingEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title);
	~FlockingEffect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;

private:
	float getRandomFloat();
	void updateParticle(int i);
	void wrapEdges(TParticle& particle) const;
	float getDy2(float x1, float y1, float x2, float y2);
	float getDx2(float x1, float y1, float x2, float y2);
};