#pragma once
#include "EffectTemplate.h"
class TransitionEffect : public EffectTemplate
{
private:
	// image background
	SDL_Surface* imageSrc;
	// image background
	SDL_Surface* imageDst;

	EffectTemplate* src;
	EffectTemplate* dst;

public:

	TransitionEffect(SDL_Surface* surface, int screenHeight, int screenWidth, EffectTemplate* src, EffectTemplate* dst);
	~TransitionEffect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;
	virtual bool isEnded() override;

	virtual Uint8* getImageBuffer(int i, int j, Uint8* imageBufferSrc, Uint8* imageBufferDst);
private:
	void copyImage();

};

