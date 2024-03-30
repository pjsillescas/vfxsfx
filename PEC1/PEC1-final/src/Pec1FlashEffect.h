#pragma once

#include "EffectTemplate.h"

class Pec1FlashEffect : public EffectTemplate
{
private:
	const int NUM_FLASH_IMAGES = 15;

	Uint32 Backgroundcolor;
	SDL_Surface** textures;

	int currentImage;
	int currentTime;
	int previousTime;
	int flashtime;
	float flashMaxTime;

public:

	Pec1FlashEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, const char* fileName, float flashMaxTime);
	~Pec1FlashEffect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;

private:
	void renderFlash(SDL_Surface* surf, Uint8 alpha);
	void changeBackgroundColor();
	Uint32 getRandomColor();

};
