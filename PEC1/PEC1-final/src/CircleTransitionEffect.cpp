#include "CircleTransitionEffect.h"

#include <iostream>

CircleTransitionEffect::CircleTransitionEffect(SDL_Surface* surface, int screenHeight, int screenWidth, EffectTemplate* src, EffectTemplate* dst) : TransitionEffect(surface, screenHeight, screenWidth, src, dst)
{
	;
}

CircleTransitionEffect::~CircleTransitionEffect()
{
	;
}

bool CircleTransitionEffect::useSourceBuffer(int i, int j)
{
	int x = ( i - screenWidth / 2);
	int y = (j - screenHeight / 2);
	bool useBuffer = ((x * x + y * y) >= radius * radius);

	return getIsReverse() && !useBuffer || !getIsReverse() && useBuffer;
}

void CircleTransitionEffect::init()
{
	TransitionEffect::init();
	maxRadius = (screenWidth > screenHeight) ? screenWidth : screenHeight;
	deltaRadius = maxRadius / NUM_FRAMES_TRANSITION;
	
	radius = (getIsReverse()) ? maxRadius : 0;
}

void CircleTransitionEffect::prepareNextFrame()
{
	radius += ((getIsReverse()) ? -1 : 1) * deltaRadius;
	
	if (getIsReverse() && radius <= 0 || radius >= maxRadius)
	{
		setIsEnded(true);
	}
}