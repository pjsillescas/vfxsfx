#pragma once

#include "EffectTemplate.h"

// this record contains the information for one star
struct TStar
{
	float x, y;             // position of the star
	unsigned char plane;    // remember which plane it belongs to
};

class StarsEffect : public EffectTemplate
{
public:
	// Stars
	// change this to adjust the number of stars
	const int MAXSTARS = 256;
	
private:
	// this is a pointer to an array of stars
	TStar* stars = NULL;

	void putPixel(SDL_Surface* surface, int x, int y, Uint32 pixel) const;

public:

	StarsEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout);
	~StarsEffect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;

};
