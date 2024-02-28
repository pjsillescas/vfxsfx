#pragma once

class Clock
{
private:
	const float FPS = 60.f;

	//static Clock instance;

	int lastTime = 0;
	int currentTime = 0;
	int deltaTime = 0;
	float msFrame;

	Clock();

public:
	static Clock& getInstance()
	{
		static Clock instance;

		return instance;
	}
	
	Clock(Clock const&) = delete;
	void operator=(Clock const&) = delete;

	void waitTime();

	int getCurrentTime();
	int getDeltaTime();
};

