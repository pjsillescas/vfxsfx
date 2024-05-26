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
	currentTime = (float) prevTicks;
}

void Clock::waitFrame()
{
	currentTime = (float) SDL_GetTicks64();
	deltaTime = currentTime - lastTime;
	if (deltaTime < (int)msFrame)
	{
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
	Uint64 diff = currentTime - prevTicks;
	fpsCounter += diff;
	prevTicks = currentTime;

	if (fpsCounter >= 1000)
	{
		fps = (float)framesDrawn / (float)(fpsCounter / 1000.0f);
		framesDrawn = 0;
		fpsCounter = 0;
	}
}

float Clock::getFPS() const
{
	return fps;
}
