//Using SDL, SDL OpenGL, GLAD, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL_opengl.h>

#include "Shader.h"
#include "Camera3D.h"
#include "PlayerController.h"
#include "WaterObject.h"
#include "FlameObject.h"
#include "SceneRenderer.h"
#include <iostream>



//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

// Reflection and refraction dimension constants
const int REFLECTION_WIDTH = 320;
const int REFLECTION_HEIGHT = 180;

const int REFRACTION_WIDTH = 1280;
const int REFRACTION_HEIGHT = 720;


//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//OpenGL context
SDL_GLContext gContext;

Shader waterShader;
Shader textureMatrixColorShader;
Shader textureMatrixColorShaderBackground;
Shader flameShader;

Camera3D *camera;
PlayerController* playerController;

WaterObject* waterObject;
SceneRenderer* scene;
FlameObject* flameObject;

//Initializes rendering program and clear color
static void initGL()
{
	camera = new Camera3D();
	camera->init(SCREEN_WIDTH, SCREEN_HEIGHT);
	playerController = new PlayerController(camera);

	waterShader.init("Water4");
	textureMatrixColorShader.init("TextureMatrixColorClip");
	textureMatrixColorShaderBackground.init("TextureMatrixColorClip");
	flameShader.init("Flame");

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
	
	std::vector<glm::vec3> cubePositions;// (cubePositionsArray, cubePositionsArray + sizeof(glm::vec3));
	
	for (int i = 0; i < 10; i++)
	{
		cubePositions.push_back(cubePositionsArray[i]);
	}

	SceneRendererConfig sceneConfig{
		camera,
		&textureMatrixColorShader,
		{
			// background
			&textureMatrixColorShaderBackground,
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
		/*{
			// Flame
			camera,
			&flameShader,
			REFRACTION_WIDTH,
			REFRACTION_HEIGHT,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			"Assets/FlamePlane.txt",
			glm::vec3(0.0f, 4.f, -1.0f),
			"Assets/textures/maskFlameTex.png",
			"Assets/textures/waterDUDV.png",
		},*/
	};
	scene = new SceneRenderer(sceneConfig);

	FlameObjectConfig flameConfig
	{
		// Flame
		camera,
		&flameShader,
		REFRACTION_WIDTH,
		REFRACTION_HEIGHT,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		"Assets/FlamePlane.txt",
		glm::vec3(0.0f, 4.f, -1.0f),
		"Assets/textures/maskFlameTex.png",
		"Assets/textures/waterDUDV.png",
	};
	flameObject = new FlameObject(flameConfig, scene);
	
	WaterConfig waterConfig{ camera, &waterShader,
	REFLECTION_WIDTH, REFLECTION_HEIGHT,
	REFRACTION_WIDTH, REFRACTION_HEIGHT,
	SCREEN_WIDTH, SCREEN_HEIGHT,
	"Assets/WaterPlane.txt",
	"Assets/textures/waterDUDV.png",
	"Assets/textures/normaltexture.jpg"
	};

	waterObject = new WaterObject(waterConfig, scene, flameObject);
}

//Starts up SDL, creates window, and initializes OpenGL
static bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	//Use OpenGL 3.3 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	//Create window
	gWindow = SDL_CreateWindow("SDL + OpenGL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (gWindow == NULL)
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	
	//Create context
	gContext = SDL_GL_CreateContext(gWindow);
	if (gContext == NULL)
	{
		printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		success = false;
	}

	//Use Vsync
	if (SDL_GL_SetSwapInterval(1) < 0)
	{
		std::cout << "Warning: Unable to set VSync! SDL Error: " << SDL_GetError() << std::endl;
	}

	initGL();

	SDL_SetRelativeMouseMode(SDL_TRUE);

	return success;
}


//Per frame update
static void update()
{
	playerController->update();
}

//Renders quad to the screen
static void render()
{
	// Enable Clip distance
	glEnable(GL_CLIP_DISTANCE0);
	
	// Water
	waterObject->renderFrameBuffers();
	flameObject->renderFrameBuffer();

	glDisable(GL_CLIP_DISTANCE0);

	glm::vec4 clipPlane(0,1,0,0);
	scene->render(clipPlane);
	flameObject->render();
	// Fire
	
	flameObject->renderFrameBuffer();
	flameObject->bindFrameBuffer();
	waterObject->render();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	flameObject->unbindFrameBuffer();

	// Render fire
	flameObject->render();

	//Render Water with Reflection and refraction Textures
	waterObject->render();
}

//Frees media and shuts down SDL
static void close()
{
	delete playerController;
	delete scene;
	delete waterObject;
	delete flameObject;
	delete camera;

	//Destroy window	
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		std::cout << "Failed to initialize!" << std::endl;
		return 1;
	}
	
	std::cout << "* Use WASD and Mouse to walk in 3d World *" << std::endl;

	//Main loop flag
	bool quit = false;

	//While application is running
	while (!quit)
	{
		quit = playerController->handleInput();

		update();
		render();

		//Update screen
		SDL_GL_SwapWindow(gWindow);
	}

	//Free resources and close SDL
	close();

	return 0;
}