#pragma once

#include "EffectTemplate.h"

struct TFlowStar;

class FlowEffect : public EffectTemplate
{
public:
	// Stars
	// change this to adjust the number of stars
	const int MAXSTARS = 256;

private:
	// this is a pointer to an array of stars
	TFlowStar* stars = NULL;

	// Flow velocity
	float flowVx = 0;
	float flowVy = 0;

	void putPixel(SDL_Surface* surface, int x, int y, Uint32 pixel) const;

public:

	FlowEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title);
	~FlowEffect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;

private:
	float clampValueCircular(float value, float minValue, float maxValue);
};
