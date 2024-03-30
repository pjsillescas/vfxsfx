#include "BlackScreenEffect.h"

BlackScreenEffect::BlackScreenEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title) : EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
	;
}

void BlackScreenEffect::init()
{
	;
}

void BlackScreenEffect::update(float deltaTime)
{
	;
}

void BlackScreenEffect::render()
{
	SDL_FillRect(surface, NULL, 0);
}
