#include "EffectWithTransition.h"

#include "ColumnTransitionEffect.h"
#include "CircleTransitionEffect.h"
#include "RowTransitionEffect.h"

EffectWithTransition::EffectWithTransition(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, EffectTemplate* oldEffect, EffectTemplate* effect, SDL_Surface* auxSurface) : EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
	this->effect = effect;
	this->oldEffect = oldEffect;
	this->transitionEffect = getNewTransition(surface, screenHeight, screenWidth, oldEffect, effect);
	isInitialized = false;
	this->auxSurface = auxSurface;
	isTransitionEnded = false;

}

TransitionEffect* EffectWithTransition::getNewTransition(SDL_Surface* screenSurface, int screenHeight, int screenWidth, EffectTemplate* oldEffect, EffectTemplate* effect)
{

	int transitionNum = (rand()) % 3;
	switch (transitionNum) {
	case 1:
		return new ColumnTransitionEffect(screenSurface, screenHeight, screenWidth, oldEffect, effect);
		break;
	case 2:
		return new CircleTransitionEffect(screenSurface, screenHeight, screenWidth, oldEffect, effect);
		break;
	case 0:
	default:
		return new RowTransitionEffect(screenSurface, screenHeight, screenWidth, oldEffect, effect);
		break;
	}
}


void EffectWithTransition::init()
{
}

void EffectWithTransition::update(float deltaTime)
{
	if (!isInitialized)
	{
		oldEffect->setSurface(auxSurface);
		effect->setSurface(surface);
		
		// initialization in the first update
		transitionEffect->init();
		transitionEffect->start();

		isInitialized = true;
	}

	if (!isTransitionEnded)
	{
		transitionEffect->update(deltaTime);
		
		if (transitionEffect->isEnded())
		{
			isTransitionEnded = true;
		}
	}
	else
	{
		effect->update(deltaTime);
	}
}

void EffectWithTransition::render()
{
	if (!isTransitionEnded)
	{
		transitionEffect->render();

		if (transitionEffect->isEnded())
		{
			isTransitionEnded = true;
		}

	}
	else
	{
		effect->render();
	}
}

EffectWithTransition::~EffectWithTransition()
{
	;
}