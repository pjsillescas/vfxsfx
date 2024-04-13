#pragma once

struct SDL_Window;

class Challenge3 {
private:
	unsigned int VBO, VAO;

public:
	int run(SDL_Window* gWindow);
private:
	void render();
	void close();

	const int PERIOD = 1000;
	const float FREQ = 1.f / PERIOD;
	int nFrames = 0;

	unsigned int shaderProgram = 0;
	//Render flag
	bool gRenderQuad = true;

	void loadShaders();
	void initTriangle();
	bool init();
	void handleKeys(unsigned char key, int x, int y);

};
