#pragma once

#include "TransitionEffect.h"

class CircleTransitionEffect : public TransitionEffect
{
private:
	int radius;
	int maxRadius;
	int deltaRadius;

public:

	CircleTransitionEffect(SDL_Surface* surface, int screenHeight, int screenWidth, EffectTemplate* src, EffectTemplate* dst);
	~CircleTransitionEffect();

	virtual bool useSourceBuffer(int i, int j) override;
	virtual void init() override;
	virtual void prepareNextFrame() override;

};
