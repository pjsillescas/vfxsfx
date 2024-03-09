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
	return (j > lineNum);
}


void RowTransitionEffect::init()
{
	TransitionEffect::init();
	lineNum = 0;
	deltaLines = screenHeight / NUM_FRAMES_TRANSITION;
}

void RowTransitionEffect::prepareNextFrame()
{
	lineNum += deltaLines;

	if (lineNum >= screenHeight)
	{
		setIsEnded(true);
	}
}