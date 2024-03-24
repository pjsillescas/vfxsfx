#pragma once

#include "EffectTemplate.h"
#include "TransitionEffect.h"

class EffectWithTransition : public EffectTemplate
{
private:
	EffectTemplate* oldEffect;
	EffectTemplate* effect;
	TransitionEffect* transitionEffect;
	bool isInitialized;
	SDL_Surface* auxSurface;
	bool isTransitionEnded;

public:
	EffectWithTransition(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, EffectTemplate* oldEffect, EffectTemplate* effect, SDL_Surface* auxSurface);
	~EffectWithTransition();

	virtual void init();
	virtual void update(float deltaTime);
	virtual void render();

private:
	TransitionEffect* getNewTransition(SDL_Surface* screenSurface, int screenHeight, int screenWidth, EffectTemplate* oldEffect, EffectTemplate* effect);
};

