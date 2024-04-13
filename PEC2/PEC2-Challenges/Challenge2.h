#pragma once

struct SDL_Window;

class Challenge2 {
private:
	//Render flag
	bool gRenderQuad = true;

public:
	int run(SDL_Window* gWindow);

private:
	void render();
	void handleKeys(unsigned char key, int x, int y);
	bool init();
	bool initGL();
};
