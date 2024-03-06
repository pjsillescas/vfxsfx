#include "RowTransitionEffect.h"

RowTransitionEffect::RowTransitionEffect(SDL_Surface* surface, int screenHeight, int screenWidth, EffectTemplate* src, EffectTemplate* dst) : TransitionEffect(surface, screenHeight, screenWidth, src, dst)
{
	;
}

RowTransitionEffect::~RowTransitionEffect()
{
	;
}

Uint8* RowTransitionEffect::getImageBuffer(int i, int j, Uint8* imageBufferSrc, Uint8* imageBufferDst)
{
	if (j > lineNum)
	{
		//std::cout << "change (" << i << "," << j << ")" << std::endl;
		return imageBufferSrc;
	}
	else
	{
		return 	imageBufferDst;
	}
}

void RowTransitionEffect::init()
{
	TransitionEffect::init();
	lineNum = 0;
}

void RowTransitionEffect::prepareNextFrame()
{
	lineNum += 2;

	if (lineNum >= screenHeight)
	{
		setIsEnded(true);
	}
}