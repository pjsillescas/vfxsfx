#pragma once
#include "EffectTemplate.h"

class PlasmaEffectChallenge3 : public EffectTemplate
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

	PlasmaEffectChallenge3(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout);
	~PlasmaEffectChallenge3();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;

private:
	void buildPalette();

};
