#include "ColumnTransitionEffect.h"

ColumnTransitionEffect::ColumnTransitionEffect(SDL_Surface* surface, int screenHeight, int screenWidth, EffectTemplate* src, EffectTemplate* dst) : TransitionEffect(surface, screenHeight, screenWidth, src, dst)
{
	deltaLines = 0;
}

ColumnTransitionEffect::~ColumnTransitionEffect()
{
	;
}

bool ColumnTransitionEffect::useSourceBuffer(int i, int j)
{
	return getIsReverse() && j <= lineNum || !getIsReverse() && (i > lineNum);
}

void ColumnTransitionEffect::init()
{
	TransitionEffect::init();
	lineNum = (getIsReverse()) ? screenWidth : 0;
	deltaLines = screenHeight / NUM_FRAMES_TRANSITION;

}

void ColumnTransitionEffect::prepareNextFrame()
{
	lineNum += ((getIsReverse()) ? -1 : 1) * deltaLines;

	if (getIsReverse() && lineNum <= 0 || lineNum >= screenWidth)
	{
		setIsEnded(true);
	}
}