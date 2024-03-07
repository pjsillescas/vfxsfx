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
	return ((x * x + y * y) >= radius * radius);
}

void CircleTransitionEffect::init()
{
	TransitionEffect::init();
	radius = 0;
	maxRadius = (screenWidth > screenHeight) ? screenWidth : screenHeight;
	deltaRadius = maxRadius / 10;
}

void CircleTransitionEffect::prepareNextFrame()
{
	radius += deltaRadius;
	
	if (radius >= maxRadius)
	{
		setIsEnded(true);
	}
}