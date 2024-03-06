#include "ColumnTransitionEffect.h"

ColumnTransitionEffect::ColumnTransitionEffect(SDL_Surface* surface, int screenHeight, int screenWidth, EffectTemplate* src, EffectTemplate* dst) : TransitionEffect(surface, screenHeight, screenWidth, src, dst)
{
	;
}

ColumnTransitionEffect::~ColumnTransitionEffect()
{
	;
}

Uint8* ColumnTransitionEffect::getImageBuffer(int i, int j, Uint8* imageBufferSrc, Uint8* imageBufferDst)
{
	if (i > lineNum)
	{
		//std::cout << "change (" << i << "," << j << ")" << std::endl;
		return imageBufferSrc;
	}
	else
	{
		return 	imageBufferDst;
	}
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