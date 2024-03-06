#pragma once

#include "TransitionEffect.h"

class RowTransitionEffect : public TransitionEffect
{
private:
	int lineNum = 0;

public:

	RowTransitionEffect(SDL_Surface* surface, int screenHeight, int screenWidth, EffectTemplate* src, EffectTemplate* dst);
	~RowTransitionEffect();

	virtual Uint8* getImageBuffer(int i, int j, Uint8* imageBufferSrc, Uint8* imageBufferDst) override;

	virtual void init() override;
	virtual void prepareNextFrame() override;

};
