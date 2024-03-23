#pragma once

#include "EffectTemplate.h"

class BlackScreenEffect: public EffectTemplate
{
public:
	BlackScreenEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title);

	virtual void init();
	virtual void update(float deltaTime);
	virtual void render();

};

