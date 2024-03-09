#pragma once

#include "EffectTemplate.h"

#include "vector.h"
#include "matrix.h"

class ParticleEffectOptimized : public EffectTemplate
{
private:
	// number of points in the object
	const int MAXPTS = 2048;

	// matrix that describes the rotation of the object
	MATRIX obj;
	// how far it is from the viewer
	float base_dist;
	// points of our object
	VECTOR* pts;
	// store the precalculated scaling values
	int* scaleX;
	int* scaleY;
	// Second buffer for effects
	SDL_Surface* secondScreen;

public:

	ParticleEffectOptimized(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title);
	~ParticleEffectOptimized();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;


private:
	void Rescale(SDL_Surface* src, SDL_Surface* dst);
	void Blur(SDL_Surface* src, SDL_Surface* dst);
	void Draw(SDL_Surface* where, VECTOR v);
};

