#pragma once

#include <glm/glm.hpp>

struct SDL_Window;

class Shader;
class Camera;

class Challenge10
{
private:
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 600;

	Shader* shader;
	Shader* screenShader;

	Camera* camera;

	unsigned int VBO, VAO, EBO;
	unsigned int texture1, texture2;
	unsigned int quadVAO, quadVBO;
	unsigned int framebuffer;
	unsigned int textureColorbuffer;

	float deltaTime;
	float lastFrame;


public:
	Challenge10();
	~Challenge10();

	int run(SDL_Window* gWindow);

private:
	bool init();
	void initData();
	void handleKeys(unsigned char key, int x, int y);
	void render();
	void close();
	glm::vec3 transformVector(glm::mat4 transformMatrix, glm::vec3 v);
	void draw();
};
