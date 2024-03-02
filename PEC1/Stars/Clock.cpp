#include "Clock.h"

Clock::Clock()
{
	lastTime = 0, currentTime, deltaTime;
	msFrame = 1 / (FPS / 1000.0f);

	// fps
	framesDrawn = 0;
	fpsCounter = 0;
	fps = 0.0f;
	prevTicks = SDL_GetTicks64();
}

void Clock::waitFrame() {
	currentTime = SDL_GetTicks();
	deltaTime = currentTime - lastTime;
	if (deltaTime < (int)msFrame) {
		SDL_Delay((int)msFrame - deltaTime);
	}
	lastTime = currentTime;

	updateFPS();
}

int Clock::getCurrentTime() const
{
	return currentTime;
}

int Clock::getDeltaTime() const
{
	return deltaTime;
}

void Clock::updateFPS()
{
	framesDrawn++;
	Uint64 ticksNow = SDL_GetTicks64();
	Uint64 diff = ticksNow - prevTicks;
	fpsCounter += diff;
	prevTicks = ticksNow;

	if (fpsCounter >= 1000) {
		fps = (float)framesDrawn / (float)(fpsCounter / 1000.0f);
		framesDrawn = 0;
		fpsCounter = 0;
	}
}

float Clock::getFPS() const
{
	return fps;
}
