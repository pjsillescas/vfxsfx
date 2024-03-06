#pragma once

#include "EffectTemplate.h"

class FireEffect : public EffectTemplate
{
private:
	const int MAX_PALETTE = 256;
	struct RGBColor { unsigned char R, G, B; };
	RGBColor* palette;

	// two fire buffers
	unsigned char* fire1;
	unsigned char* fire2;
	// a temporary variable to swap the two previous buffers
	unsigned char* tmp;

public:

	FireEffect(SDL_Surface* surface, int screenHeight, int screenWidth);
	~FireEffect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;
	virtual bool isEnded() override;

private:
	void buildPalette();
	void Shade_Pal(int s, int e, int r1, int g1, int b1, int r2, int g2, int b2);
	void Heat(unsigned char* dst);
	void Blur_Up(unsigned char* src, unsigned char* dst);
};

