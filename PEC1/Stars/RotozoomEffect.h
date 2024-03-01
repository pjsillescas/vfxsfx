#pragma once

#include "EffectTemplate.h"

class RotozoomEffect : public EffectTemplate
{
private:
	// buffer containing the texture
	SDL_Surface* texdata;
	// Points from texture
	int pointx1;
	int pointy1;
	int pointx2;
	int pointy2;
	int pointx3;
	int pointy3;

public:

	RotozoomEffect(SDL_Surface* surface, int screenHeight, int screenWidth);
	~RotozoomEffect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;

private:
	void DoRotoZoom(float cx, float cy, float radius, float angle);
	void TextureScreen();

};

