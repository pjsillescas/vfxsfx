#pragma once
#include "EffectTemplate.h"
class TunnelEffect : public EffectTemplate
{
private:
	// buffer containing the (u,v) pairs at each pixel
	unsigned char* texcoord;
	// buffer containing the texture
	SDL_Surface* texdata;
public:

	TunnelEffect(SDL_Surface* surface, int screenHeight, int screenWidth);
	~TunnelEffect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;
	virtual bool isEnded() override;

private:
	float getXPos(float f);
	float getYPos(float f);
	float getRadius(float f);
	void DrawHole(int du, int dv);
};

