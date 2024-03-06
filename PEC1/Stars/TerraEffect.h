#pragma once

#include "EffectTemplate.h"

#include "vector.h"
#include "matrix.h"

class TerraEffect : public EffectTemplate
{
private:
	SDL_Surface* texture;
	// setup 3D data
	VECTOR A, B, C;

public:

	TerraEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout);
	~TerraEffect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;

private:
	void DrawPlane(VECTOR Bp, VECTOR Up, VECTOR Vp);
};

