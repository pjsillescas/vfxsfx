#pragma once

// Video https://www.youtube.com/watch?v=NF8UIcpk6zY

#include "EffectTemplate.h"

class WhirlpoolEffect : public EffectTemplate
{
private:
	SDL_Surface* image;

	int tiling;
	float offset;
	float speed;
	bool applyNoise;

public:

	WhirlpoolEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, const char* texture, int tiling, float offset, float speed, bool applyNoise);
	~WhirlpoolEffect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;

private:
	Uint8* getImagePosition(int i, int j, float tiling, float offset);
	int noise(int k, int n) const;
};

