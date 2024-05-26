#pragma once

#include <SDL.h>

class Clock
{
private:
	const float FPS = 60.f;

	int lastTime = 0;
	int currentTime = 0;
	int deltaTime = 0;
	float msFrame;

	int framesDrawn;
	Uint64 fpsCounter;
	float fps;
	Uint64 prevTicks;

	Clock();

public:
	static Clock& getInstance()
	{
		static Clock instance;

		return instance;
	}
	
	Clock(Clock const&) = delete;
	void operator=(Clock const&) = delete;

	void waitFrame();

	int getCurrentTime() const;
	int getDeltaTime() const;

	float getFPS() const;
private:
	void updateFPS();
};

