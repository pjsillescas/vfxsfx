#pragma once

#include "EffectTemplate.h"

class BarsEffect : public EffectTemplate
{
private:
	const int MAX_PALETTE = 256;
	struct RGBColor { unsigned char R, G, B; };
	RGBColor* palette;

	// two fire buffers
	unsigned char* bars1;
	unsigned char* bars2;
	// a temporary variable to swap the two previous buffers
	unsigned char* tmp;

	int minValue;
	int maxValue;

	float speed;

public:

	BarsEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, const char* fileName, float speed);
	~BarsEffect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;

private:
	int clampIndex(int index, int min, int max);
	void buildPalette();
	void shadePal(int s, int e, int r1, int g1, int b1, int r2, int g2, int b2);
	void seed(unsigned char* dst);
	void sharpen(unsigned char* src, unsigned char* dst);
};

