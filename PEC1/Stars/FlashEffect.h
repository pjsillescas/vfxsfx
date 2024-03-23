#pragma once

#include "EffectTemplate.h"

class FlashEffect : public EffectTemplate
{
private:
	Uint32 Backgroundcolor;
	SDL_Surface* flashTexture;
	SDL_Surface* blackSurface;


	int currentTime;
	int previousTime;
	int flashtime;
	float flashMaxTime;

public:

	FlashEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title);
	FlashEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, const char* fileName, float flashMaxTime);
	~FlashEffect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;

private:
	void renderFlash(SDL_Surface* surf, Uint8 alpha);
	void changeBackgroundColor();
	Uint32 getRandomColor();

};
