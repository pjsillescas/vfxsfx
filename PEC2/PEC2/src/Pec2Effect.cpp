#include "Pec2Effect.h"

//Using SDL, SDL OpenGL, GLAD, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL_opengl.h>

#include "Camera3D.h"
#include "PlayerController.h"
#include "WaterObject.h"
#include "FlameObject.h"
#include "LavaObject.h"
#include "SceneRenderer.h"
#include "Shader.h"

#include <iostream>
//#include "Clock.h"

Pec2Effect::Pec2Effect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, SDL_Window* window)
	: EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
	this->window = window;

	waterShader = new Shader();
	textureMatrixColorShader = new Shader();
	textureMatrixColorShaderBackground = new Shader();
	flameShader = new Shader();
	lavaShader = new Shader();
}

Pec2Effect::~Pec2Effect()
{
	delete playerController;
	delete scene;
	delete flameObject;
	delete lavaObject;
	delete camera;
	
	delete waterShader;
	delete textureMatrixColorShader;
	delete textureMatrixColorShaderBackground;
	delete flameShader;
	delete lavaShader;
}

void Pec2Effect::init()
{
	//Create context
	gContext = SDL_GL_CreateContext(window);
	if (gContext == NULL)
	{
		printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
	}

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	//Use Vsync
	if (SDL_GL_SetSwapInterval(1) < 0)
	{
		std::cout << "Warning: Unable to set VSync! SDL Error: " << SDL_GetError() << std::endl;
		return;
	}

	SDL_SetRelativeMouseMode(SDL_TRUE);

	// initgl
	camera = new Camera3D();
	camera->init(this->screenWidth, this->screenHeight);
	playerController = new PlayerController(camera);

	waterShader->init("Water4");
	textureMatrixColorShader->init("TextureMatrixColorClip");
	textureMatrixColorShaderBackground->init("TextureMatrixColorClip");
	flameShader->init("Flame");
	lavaShader->init("Lava");

	//Initialize clear color
	glClearColor(0.f, 0.f, 0.f, 1.f);

	//Zbuffer enable
	glEnable(GL_DEPTH_TEST);

	// Back face culling
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);

	//Generate Objects
	glm::vec3 cubePositionsArray[] = {
		glm::vec3(2.0f,  5.0f, -3.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -2.0f),
		glm::vec3(3.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, 7.5f),
		glm::vec3(1.3f, -2.0f, -3.5f),
		glm::vec3(2.5f,  2.0f, 2.5f),
		glm::vec3(-2.5f,  2.5f, -2.0f),
		glm::vec3(-1.3f,  1.0f, 1.5f),
		glm::vec3(0.f, 4.f, -2.5f),
	};

	std::vector<glm::vec3> cubePositions;

	for (int i = 0; i < 10; i++)
	{
		cubePositions.push_back(cubePositionsArray[i]);
	}

	SceneRendererConfig sceneConfig{
		camera,
		textureMatrixColorShader,
		{
			// background
			textureMatrixColorShaderBackground,
			camera,
			"Assets/BackgroundPlane.txt",
			"Assets/textures/desert-unsplash.png",
			glm::vec3(0, 0, -10),
		},
		{
			// cube
			"Assets/Cube.txt",
			"Assets/textures/texture.png",
		},
		cubePositions,
		{
			// underwater plane
			"Assets/Pool.txt",
			"Assets/textures/floor.jpg",
			glm::vec3(0.0, -4.0, 0.0),
		},
		// Burning cube
		glm::vec3(0.f, 0.f, -1.f),
		glm::vec3(7.f, 1.f, 1.f),
	};
	scene = new SceneRenderer(sceneConfig);

	FlameObjectConfig flameConfig
	{
		// Flame
		camera,
		flameShader,
		REFRACTION_WIDTH,
		REFRACTION_HEIGHT,
		this->screenWidth,
		this->screenHeight,
		"Assets/FlamePlane.txt",
		glm::vec3(0.0f, 4.f, -1.0f),
		"Assets/textures/maskFlameTex.png",
		"Assets/textures/waterDUDV.png",
	};
	flameObject = new FlameObject(flameConfig, scene);
	
	const float LAVA_SPEED = 0.0001f;
	LavaConfig lavaConfig{
		camera,
		lavaShader,
		REFLECTION_WIDTH, REFLECTION_HEIGHT,
		REFRACTION_WIDTH, REFRACTION_HEIGHT,
		this->screenWidth,
		this->screenHeight,
		"Assets/WaterPlane.txt",
		"Assets/textures/waterDUDV.png",
		"Assets/textures/normaltexture.jpg",
		LAVA_SPEED,
	};
	lavaObject = new LavaObject(lavaConfig, scene, flameObject);
}

bool Pec2Effect::update(float deltaTime)
{
	bool quit = playerController->handleInput();
	playerController->update();

	return quit;
}

void Pec2Effect::render()
{
	camera->update();

	// Enable Clip distance
	glEnable(GL_CLIP_DISTANCE0);

	// Flame
	flameObject->renderFrameBuffer();

	glDisable(GL_CLIP_DISTANCE0);

	glm::vec4 clipPlane(0, 1, 0, 0);
	scene->render(clipPlane);
	flameObject->render();

	flameObject->renderFrameBuffer();
	flameObject->bindFrameBuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	flameObject->unbindFrameBuffer();

	// Render fire
	flameObject->render();

	//Render Water with Reflection and refraction Textures
	lavaObject->renderFrameBuffers();
	lavaObject->render();
}
