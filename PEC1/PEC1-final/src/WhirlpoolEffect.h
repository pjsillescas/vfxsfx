#pragma once

// Video https://www.youtube.com/watch?v=NF8UIcpk6zY

#include "EffectTemplate.h"

class WhirlpoolEffect : public EffectTemplate
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

	SDL_Surface* image;

	int tiling;
	float offset;
	float speed;
	bool applyNoise;

public:

	WhirlpoolEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, int tiling, float offset, float speed, bool applyNoise);
	~WhirlpoolEffect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;

private:
	void seed(unsigned char* dst);
	void sharpen(unsigned char* src, unsigned char* dst);

	Uint8* getImagePosition(int i, int j, float tiling, float offset);
	int noise(int k, int n);
};

