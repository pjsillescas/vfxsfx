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
#include "Object3D.h"
#include "WaterObj.h"
#include "PlayerController.h"
#include "FrameBuffer.h"
#include "TextureUtils.h"
#include "FlameObj.h"
#include "Utils.h"
#include "WaterObject.h"

#include <iostream>
#include "stdlib.h"

struct BackgroundConfig
{
	Shader* textureMatrixColorShaderBackground;
	Camera3D* camera;
	std::string geometryFile;
	std::string textureFile;
};

class BackgroundObject
{
private:
	Shader* textureMatrixColorShaderBackground;
	Camera3D* camera;
	Object3D *backgroundPlane;

public:
	BackgroundObject(BackgroundConfig& backgroundConfig);
	~BackgroundObject();

	void render(glm::vec4 pClipPlane);
};

BackgroundObject::BackgroundObject(BackgroundConfig& backgroundConfig)
{
	this->camera = backgroundConfig.camera;
	this->textureMatrixColorShaderBackground = backgroundConfig.textureMatrixColorShaderBackground;
	
	backgroundPlane = new Object3D();
	backgroundPlane->loadObjFromDisk(backgroundConfig.geometryFile);
	backgroundPlane->setTexture(TextureUtils::loadTextureFromDisk(backgroundConfig.textureFile));
	backgroundPlane->setShader(textureMatrixColorShaderBackground);
	backgroundPlane->setPosition(glm::vec3(0, 0, -10));

}

BackgroundObject::~BackgroundObject()
{
	delete backgroundPlane;
}

void BackgroundObject::render(glm::vec4 clipPlane)
{
	textureMatrixColorShaderBackground->Use();

	glm::mat4 projectionMatrix = camera->getUniformProjectionMatrix();
	glm::mat4 viewMatrix = camera->getUniformViewMatrix();
	Utils::fillMatrixData(textureMatrixColorShaderBackground->getID(), projectionMatrix, viewMatrix);

	int UniformPlaneM = glGetUniformLocation(textureMatrixColorShaderBackground->getID(), "plane");
	// Clip Plane Set
	glUniform4f(UniformPlaneM, clipPlane.x, clipPlane.y, clipPlane.z, clipPlane.w);

	backgroundPlane->render();
}

struct CubeConfig
{
	std::string geometryFile;
	std::string textureFile;
};

struct SceneRendererConfig
{
	Camera3D* camera;
	Shader* textureMatrixColorShader;
	BackgroundConfig backgroundConfig;
	CubeConfig cubeConfig;
	int numCubes;
	std::vector<glm::vec3> cubePositions;
};

class SceneRenderer
{
private:
	Shader* textureMatrixColorShader;
	Camera3D* camera;

	BackgroundObject* backgroundObject;
	Object3D* burningCube;

	Object3D underWaterPlane;
	std::vector<Object3D*> cubes;

public:
	SceneRenderer(SceneRendererConfig& sceneConfig);
	~SceneRenderer();

	void render(glm::vec4 clip);
private:
	Object3D* createCube(CubeConfig& cubeConfig, glm::vec3 position) const;
};
Object3D* SceneRenderer::createCube(CubeConfig& cubeConfig, glm::vec3 position) const
{
	Object3D* object = new Object3D();
	object->loadObjFromDisk(cubeConfig.geometryFile);
	object->loadTextureFromDisk(cubeConfig.textureFile);
	object->setShader(textureMatrixColorShader);
	object->setPosition(position);

	return object;
}

SceneRenderer::SceneRenderer(SceneRendererConfig& sceneConfig)
{
	this->textureMatrixColorShader = sceneConfig.textureMatrixColorShader;
	this->camera = sceneConfig.camera;

	backgroundObject = new BackgroundObject(sceneConfig.backgroundConfig);

	for (unsigned int i = 0; i < sceneConfig.cubePositions.size(); i++)
	{
		cubes.push_back(createCube(sceneConfig.cubeConfig, cubePositions[i]));
	}

	burningCube = createCube(sceneConfig.cubeConfig, glm::vec3(0.f, 0.f, -1.f));
	burningCube->setScale(glm::vec3(7.f, 1.f, 1.f));

	backgroundPlane.loadObjFromDisk("Assets/BackgroundPlane.txt");
	backgroundPlane.setTexture(TextureUtils::loadTextureFromDisk("Assets/textures/desert-unsplash.png"));
	backgroundPlane.setShader(&textureMatrixColorShaderBackground);
	backgroundPlane.setPosition(glm::vec3(0, 0, -10));

	underWaterPlane.loadObjFromDisk("Assets/Pool.txt");
	underWaterPlane.setShader(&textureMatrixColorShader);
	underWaterPlane.loadTextureFromDisk("Assets/textures/floor.jpg");
	underWaterPlane.setPosition(glm::vec3(0.0, -4.0, 0.0));

}

SceneRenderer::~SceneRenderer()
{
	delete backgroundObject;
}

void SceneRenderer::render(glm::vec4 pClipPlane)
{
	//Clear color buffer & Z buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	backgroundObject->render(pClipPlane);

	//Bind program
	textureMatrixColorShader->Use();

	glm::mat4 projectionMatrix = camera->getUniformProjectionMatrix();
	glm::mat4 viewMatrix = camera->getUniformViewMatrix();
	Utils::fillMatrixData(textureMatrixColorShader->getID(), projectionMatrix, viewMatrix);

	int UniformPlaneM = glGetUniformLocation(textureMatrixColorShader->getID(), "plane");
	// Clip Plane Set
	glUniform4f(UniformPlaneM, pClipPlane.x, pClipPlane.y, pClipPlane.z, pClipPlane.w);

	for (auto& cube : cubes)
	{
		cube->render();
	}

	underWaterPlane.render();

	// Fire render logic
	burningCube->render();

	renderFlame();
}


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

FlameObj* flamePlane;

// FBO for fire
FrameBuffer* fireFrameBuffer;

glm::vec3 cubePositions[] = {
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

std::vector<glm::vec3> cubePositionsVec(cubePositions, cubePositions + sizeof(glm::vec3));

static FrameBuffer* createFrameBuffer(int bufferWidth, int bufferHeight)
{
	FrameBuffer* buffer = new FrameBuffer(bufferWidth, bufferHeight, SCREEN_WIDTH, SCREEN_HEIGHT);
	buffer->init();

	return buffer;
}

//Initializes rendering program and clear color
static void initGL()
{
	waterShader.init("Water4");
	textureMatrixColorShader.init("TextureMatrixColorClip");
	textureMatrixColorShaderBackground.init("TextureMatrixColorClip");

	//Initialize clear color
	glClearColor(0.f, 0.f, 0.f, 1.f);

	//Zbuffer enable
	glEnable(GL_DEPTH_TEST);

	// Back face culling
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);

	//Generate Objects
	WaterConfig waterConfig{ camera, &waterShader,
		REFLECTION_WIDTH, REFLECTION_HEIGHT,
		REFRACTION_WIDTH, REFRACTION_HEIGHT,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		"Assets/WaterPlane.txt",
		"Assets/textures/waterDUDV.png",
		"Assets/textures/normaltexture.jpg"
	};

	waterObject = new WaterObject(waterConfig);

	CubeConfig cubeConfig{
		"Assets/Cube.txt",
		"Assets/textures/texture.png",
	};
	
	SceneRendererConfig sceneConfig{
	};
	scene = new SceneRenderer(sceneConfig);

	for (unsigned int i = 0; i < 9; i++)
	{
		cubes.push_back(createCube(cubePositions[i]));
	}

	burningCube = createCube(glm::vec3(0.f, 0.f, -1.f));
	burningCube->setScale(glm::vec3(7.f, 1.f, 1.f));
	burningCube2 = createCube(glm::vec3(0.f, 4.f, -2.5f));

	backgroundPlane.loadObjFromDisk("Assets/BackgroundPlane.txt");
	backgroundPlane.setTexture(TextureUtils::loadTextureFromDisk("Assets/textures/desert-unsplash.png"));
	backgroundPlane.setShader(&textureMatrixColorShaderBackground);
	backgroundPlane.setPosition(glm::vec3(0,0,-10));

	underWaterPlane.loadObjFromDisk("Assets/Pool.txt");
	underWaterPlane.setShader(&textureMatrixColorShader);
	underWaterPlane.loadTextureFromDisk("Assets/textures/floor.jpg");
	underWaterPlane.setPosition(glm::vec3(0.0, -4.0, 0.0));
	
	flameShader.init("Flame");
	flamePlane = new FlameObj();
	flamePlane->loadObjFromDisk("Assets/FlamePlane.txt");
	flamePlane->setShader(&flameShader);
	flamePlane->setPosition(glm::vec3(0.0f, 4.f, -1.0f));
	flamePlane->setMaskTexture(TextureUtils::loadTextureFromDisk("Assets/textures/maskFlameTex.png"));
	flamePlane->setDistortionTexture(TextureUtils::loadTextureFromDisk("Assets/textures/waterDUDV.png")); // Load texture and change ID to texture 3;

	// Create Frame Buffer Objects (FBO)
	fireFrameBuffer = createFrameBuffer(REFRACTION_WIDTH, REFRACTION_HEIGHT);
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
	camera = new Camera3D();
	camera->init(SCREEN_WIDTH, SCREEN_HEIGHT);
	playerController = new PlayerController(camera);

	SDL_SetRelativeMouseMode(SDL_TRUE);

	return success;
}


//Per frame update
static void update()
{
	playerController->update();
}

float time1 = 0;

static void renderFlame()
{
	flameShader.Use();
	glUniform1f(glGetUniformLocation(flameShader.getID(), "time"), time1);
	glUniform2f(glGetUniformLocation(flameShader.getID(), "iResolution"), 5, 5);
	time1 += 0.02f;

	glm::mat4 projectionMatrix = camera->getUniformProjectionMatrix();
	glm::mat4 viewMatrix = camera->getUniformViewMatrix();
	Utils::fillMatrixData(flameShader.getID(), projectionMatrix, viewMatrix);


	flamePlane->render();
}


//Renders quad to the screen
static void render()
{
	// Enable Clip distance
	glEnable(GL_CLIP_DISTANCE0);
	
	// Water
	waterObject->renderFrameBuffers();

	// Fire
	fireFrameBuffer->bind();
	flameShader.Use();
	flamePlane->setRefractionTexture(fireFrameBuffer->getTexture());

	glm::mat4 projectionMatrix = camera->getUniformProjectionMatrix();
	glm::mat4 viewMatrix = camera->getUniformViewMatrix();
	Utils::fillMatrixData(flameShader.getID(), projectionMatrix, viewMatrix);

	glm::vec4 clipPlane = glm::vec4(0, 1, 0, 0); // 0 Height because water object ar on plane Y = 0
	renderScene(clipPlane);

	fireFrameBuffer->unbind();

	flameShader.Use();
	flamePlane->render();
	
	glDisable(GL_CLIP_DISTANCE0);
	renderScene(clipPlane);
	//Render Water with Reflection and refraction Textures
	waterObject->render();
}

//Frees media and shuts down SDL
static void close()
{
	delete playerController;
	delete flamePlane;

	delete burningCube;
	delete burningCube2;

	//Deallocate program
	waterShader.deleteProgram();
	textureMatrixColorShader.deleteProgram();

	//Destroy data in GPU
	delete waterObject;

	underWaterPlane.clearGPU();
	
	// Clear FBO Fire
	delete fireFrameBuffer;

	//FireShader.deleteProgram();
	flameShader.deleteProgram();
	
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