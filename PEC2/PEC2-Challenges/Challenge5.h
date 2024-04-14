#pragma once

struct SDL_Window;
class Triangle;

class Challenge5
{
public:
	const int NUM_TRIANGLES = 10;
	int run(SDL_Window* gWindow);
	Triangle* triangles[10];

private:
	void render();
	void close();

	const int PERIOD = 1000;
	const float FREQ = 1.f / PERIOD;
	int nFrames = 0;

	bool init();
	void handleKeys(unsigned char key, int x, int y);
};

