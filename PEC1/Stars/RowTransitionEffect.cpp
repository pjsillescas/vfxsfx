#include "RowTransitionEffect.h"

RowTransitionEffect::RowTransitionEffect(SDL_Surface* surface, int screenHeight, int screenWidth, EffectTemplate* src, EffectTemplate* dst) : TransitionEffect(surface, screenHeight, screenWidth, src, dst)
{
	;
}

RowTransitionEffect::~RowTransitionEffect()
{
	;
}

bool RowTransitionEffect::useSourceBuffer(int i, int j)
{
	return getIsReverse() && j <= lineNum || !getIsReverse() && (j > lineNum);
}


void RowTransitionEffect::init()
{
	TransitionEffect::init();
	lineNum = (getIsReverse()) ? screenHeight : 0;
	deltaLines = screenHeight / NUM_FRAMES_TRANSITION;
}

void RowTransitionEffect::prepareNextFrame()
{

	lineNum += ((getIsReverse()) ? -1 : 1) * deltaLines;

	if (getIsReverse() && lineNum <= 0 || lineNum >= screenHeight)
	{
		setIsEnded(true);
	}
}