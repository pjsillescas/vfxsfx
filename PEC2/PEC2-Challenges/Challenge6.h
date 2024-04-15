#pragma once

struct SDL_Window;

class Shader;
class Challenge6
{
private:
	Shader* shader;

	unsigned int VBO, VAO, EBO;
	unsigned int texture1, texture2;

	float mix = 0.2f;

public:
	Challenge6();
	~Challenge6();

	int run(SDL_Window* gWindow);

private:
	bool init();
	void handleKeys(unsigned char key, int x, int y);
	void initTextures();
	void render();
	void close();
};

