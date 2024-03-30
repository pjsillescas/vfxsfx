#pragma once

#include "EffectTemplate.h"

class DistortionPec1Effect : public EffectTemplate
{
private:
	// displacement buffers
	char* dispX, * dispY;
	// image background
	SDL_Surface* image;
	// define the distortion buffer movement
	int windowx1, windowy1, windowx2, windowy2;

public:

	DistortionPec1Effect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, const char* fileName);
	~DistortionPec1Effect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;

private:
	void precalculate();
	void Distort_Bili();
	void Distort();

};

