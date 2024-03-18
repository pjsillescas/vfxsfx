#pragma once

#include "EffectTemplate.h"

struct TVortexParticle;

class VortexEffect : public EffectTemplate
{
private:
	// this is a pointer to an array of stars
	TVortexParticle* grid = NULL;

	void putPixel(SDL_Surface* surface, int x, int y, Uint32 pixel) const;

	float minVorticity = 0;
	float maxVorticity = 0;

public:

	VortexEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title);
	~VortexEffect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;

private:
	void calculateVorticity();
	void applyVorticityConf(float deltaTime);
	int pixelToIndex(int i, int j) const;
};
