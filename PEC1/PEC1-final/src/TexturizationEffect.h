#pragma once

#include "EffectTemplate.h"
class TexturizationEffect : public EffectTemplate
{
private:
	const float RADIUS = 3000.f;
	
	// buffer containing the (u,v) pairs at each pixel
	unsigned char* texcoord;
	// buffer containing the texture
	SDL_Surface* texdata;
	float speedX;
	float speedY;
public:

	TexturizationEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, const char* fileName, float speedX, float speedY);
	~TexturizationEffect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;

private:
	void DrawHole(int du, int dv);
};

