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
	deltaLines = screenHeight / 10;

}

void ColumnTransitionEffect::prepareNextFrame()
{
	lineNum += deltaLines;

	if (lineNum >= screenWidth)
	{
		setIsEnded(true);
	}
}