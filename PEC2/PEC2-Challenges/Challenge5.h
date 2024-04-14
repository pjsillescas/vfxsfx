#pragma once

struct SDL_Window;
class Shader;

class Challenge5
{
private:
	unsigned int VBO, VAO;
	Shader* shader;

public:
	int run(SDL_Window* gWindow);
private:
	void render();
	void close();

	const int PERIOD = 1000;
	const float FREQ = 1.f / PERIOD;
	int nFrames = 0;

	void loadShaders();
	void initTriangle();
	bool init();
	void handleKeys(unsigned char key, int x, int y);
};

