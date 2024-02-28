#include "Clock.h"

#include <SDL.h>

Clock::Clock()
{
	lastTime = 0, currentTime, deltaTime;
	msFrame = 1 / (FPS / 1000.0f);
}

void Clock::waitTime() {
	currentTime = SDL_GetTicks();
	deltaTime = currentTime - lastTime;
	if (deltaTime < (int)msFrame) {
		SDL_Delay((int)msFrame - deltaTime);
	}
	lastTime = currentTime;
}

int Clock::getCurrentTime()
{
	return currentTime;
}

int Clock::getDeltaTime()
{
	return deltaTime;
}
