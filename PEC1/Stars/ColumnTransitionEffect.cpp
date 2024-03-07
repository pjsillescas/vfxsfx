#include "ColumnTransitionEffect.h"

ColumnTransitionEffect::ColumnTransitionEffect(SDL_Surface* surface, int screenHeight, int screenWidth, EffectTemplate* src, EffectTemplate* dst) : TransitionEffect(surface, screenHeight, screenWidth, src, dst)
{
	;
}

ColumnTransitionEffect::~ColumnTransitionEffect()
{
	;
}

bool ColumnTransitionEffect::useSourceBuffer(int i, int j)
{
	return (i > lineNum);
}

void ColumnTransitionEffect::init()
{
	TransitionEffect::init();
	lineNum = 0;
}

void ColumnTransitionEffect::prepareNextFrame()
{
	lineNum += 2;

	if (lineNum >= screenWidth)
	{
		setIsEnded(true);
	}
}