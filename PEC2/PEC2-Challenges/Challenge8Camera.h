#pragma once

#include <glm/glm.hpp>
#include <SDL.h>

struct SDL_Window;

class Shader;

class Challenge8Camera
{
private:
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 600;

	const float CAMERA_HEIGHT = 1.f;
	const float MOVEMENT_SPEED = 0.005f;
	
	//const float TURNING_SPEED = 0.01f;
	const float TURNING_SPEED = 0.1f;

	const float SCALE_FACTOR = 10.f;
	const float SPEED = 0.0005f;
	const float HEIGHT = 1.f;

	Shader* shader;

	unsigned int VBO, VAO, EBO;
	unsigned int texture1, texture2;

	glm::vec3 lookingDirection;
	glm::vec3 position;
	float yaw;
	Uint64 ticksPrevious;

	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;

	// timing
	float deltaTicks;	// time between current frame and last frame


public:	
	Challenge8Camera();
	~Challenge8Camera();

	int run(SDL_Window* gWindow);

private:
	bool init();
	void initData();
	void handleKeys(unsigned char key, int x, int y, Uint64 ticks);
	void render();
	void close();
};
