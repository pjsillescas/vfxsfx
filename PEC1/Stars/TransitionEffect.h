#pragma once
#include "EffectTemplate.h"
class TransitionEffect : public EffectTemplate
{
protected:
	const int NUM_FRAMES_TRANSITION = 20;

private:
	EffectTemplate* src;
	EffectTemplate* dst;

	int currentTime = 0;
	int startTime = 0;

	bool isReverse;

public:

	TransitionEffect(SDL_Surface* surface, int screenHeight, int screenWidth, EffectTemplate* src, EffectTemplate* dst);
	~TransitionEffect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;

	virtual bool useSourceBuffer(int i, int j) = 0;
	virtual void prepareNextFrame() = 0;
private:
	void renderFrame();
protected:
	bool getIsReverse();
};
