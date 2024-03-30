#pragma once

#include "EffectTemplate.h"

class PlasmaPec1Effect : public EffectTemplate
{
private:
	const int MAX_PALETTE = 256;

	struct RGBColor { unsigned char R, G, B; };

	RGBColor* palette;
	unsigned char* plasma1;
	unsigned char* plasma2;

	long src1;
	long src2;

public:

	PlasmaPec1Effect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title);
	~PlasmaPec1Effect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;

private:
	void buildPalette();

};
