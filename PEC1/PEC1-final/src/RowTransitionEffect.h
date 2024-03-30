#pragma once

#include "TransitionEffect.h"

class RowTransitionEffect : public TransitionEffect
{
private:
	int lineNum = 0;
	int deltaLines = 0;
public:

	RowTransitionEffect(SDL_Surface* surface, int screenHeight, int screenWidth, EffectTemplate* src, EffectTemplate* dst);
	~RowTransitionEffect();

	virtual bool useSourceBuffer(int i, int j) override;

	virtual void init() override;
	virtual void prepareNextFrame() override;

};
