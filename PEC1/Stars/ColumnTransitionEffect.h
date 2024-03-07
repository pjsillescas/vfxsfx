#pragma once

#include "TransitionEffect.h"

class ColumnTransitionEffect : public TransitionEffect
{
private:
	int lineNum = 0;
	int deltaLines;

public:

	ColumnTransitionEffect(SDL_Surface* surface, int screenHeight, int screenWidth, EffectTemplate* src, EffectTemplate* dst);
	~ColumnTransitionEffect();

	virtual bool useSourceBuffer(int i, int j) override;
	virtual void init() override;
	virtual void prepareNextFrame() override;

};
