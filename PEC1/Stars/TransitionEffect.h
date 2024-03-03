#pragma once
#include "EffectTemplate.h"
class TransitionEffect : public EffectTemplate
{
private:
	// displacement buffers
	char* dispX, * dispY;
	// image background
	SDL_Surface* image;
	// define the distortion buffer movement
	int windowx1, windowy1, windowx2, windowy2;

public:

	TransitionEffect(SDL_Surface* surface, int screenHeight, int screenWidth);
	~TransitionEffect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;

private:
	void precalculate();
	void Distort_Bili();
	void Distort();

};

