#pragma once

struct SDL_Window;

class Shader;

class Challenge8
{
private:
	Shader* shader;

	unsigned int VBO, VAO, EBO;
	unsigned int texture1, texture2;

public:
	Challenge8();
	~Challenge8();

	int run(SDL_Window* gWindow);

private:
	bool init();
	void initData();
	void handleKeys(unsigned char key, int x, int y);
	void render();
	void close();
};
