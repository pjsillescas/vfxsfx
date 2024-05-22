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
Object3D* burningCube;
Object3D* burningCube2;

WaterObj* waterPlane;
std::vector<Object3D*> cubes;
Object3D underWaterPlane;
FlameObj* flamePlane;

Object3D backgroundPlane;

// FBO for Water
FrameBuffer* waterReflectionFrameBuffer;
FrameBuffer* waterRefractionFrameBuffer;

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
	glm::vec3(-1.3f,  1.0f, 1.5f)
};

static Object3D* createCube(glm::vec3 position)
{
	Object3D* object = new Object3D();
	object->loadObjFromDisk("Assets/Cube.txt");
	object->loadTextureFromDisk("Assets/textures/texture.png");
	object->setShader(&textureMatrixColorShader);
	object->setPosition(position);

	return object;
}

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
	waterPlane = new WaterObj();
	waterPlane->loadObjFromDisk("Assets/WaterPlane.txt");
	waterPlane->setShader(&waterShader);
	waterPlane->setTexture3(TextureUtils::loadTextureFromDisk("Assets/textures/waterDUDV.png"));
	waterPlane->setTexture4(TextureUtils::loadTextureFromDisk("Assets/textures/normaltexture.jpg"));

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
	waterReflectionFrameBuffer = createFrameBuffer(REFLECTION_WIDTH, REFLECTION_HEIGHT);
	waterRefractionFrameBuffer = createFrameBuffer(REFRACTION_WIDTH, REFRACTION_HEIGHT);
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

static void fillMatrixData(GLuint id)
{
	int UniformViewM = glGetUniformLocation(id, "view");
	int UniformProjectionM = glGetUniformLocation(id, "projection");
	glm::mat4 mProjectionMatrix = camera->getUniformProjectionMatrix();
	glm::mat4 mViewMatrix = camera->getUniformViewMatrix();
	glUniformMatrix4fv(UniformProjectionM, 1, GL_FALSE, glm::value_ptr(mProjectionMatrix));
	glUniformMatrix4fv(UniformViewM, 1, GL_FALSE, glm::value_ptr(mViewMatrix));

}

static void renderFlame()
{
	flameShader.Use();
	glUniform1f(glGetUniformLocation(flameShader.getID(), "time"), time1);
	glUniform2f(glGetUniformLocation(flameShader.getID(), "iResolution"), 5, 5);
	time1 += 0.02f;
	fillMatrixData(flameShader.getID());

	flamePlane->render();
}

static void renderWater()
{
	//Bind program
	waterShader.Use();
	// Active Textures and Set them
	waterPlane->setTexture(waterReflectionFrameBuffer->getTexture());
	waterPlane->setTexture2(waterRefractionFrameBuffer->getTexture());
	
	fillMatrixData(waterShader.getID());

	// Give Camera Vector to Shader
	glm::vec3 camPos = camera->getCameraPos();
	glUniform3f(waterPlane->getUniformCamPos(), camPos.x, camPos.y, camPos.z);
	// Give an imaginary Sun light position and color
	glm::vec3 lightPos = glm::vec3(-3.0, 10.0, -5.0);
	glm::vec3 lightColor = glm::vec3(1.0, 1.0, 1.0);
	glUniform3f(waterPlane->getUniformLightPos(), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(waterPlane->getUniformLightColor(), lightColor.x, lightColor.y, lightColor.z);
	
	// Draw objects
	waterPlane->render();
}

static void renderBackground(glm::vec4 clipPlane)
{
	textureMatrixColorShaderBackground.Use();
	
	fillMatrixData(textureMatrixColorShaderBackground.getID());
	int UniformPlaneM = glGetUniformLocation(textureMatrixColorShaderBackground.getID(), "plane");
	// Clip Plane Set
	glUniform4f(UniformPlaneM, clipPlane.x, clipPlane.y, clipPlane.z, clipPlane.w);
	
	backgroundPlane.render();
}

static void renderScene(glm::vec4 PclipPlane)
{
	//Clear color buffer & Z buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderBackground(PclipPlane);
	
	//Bind program
	textureMatrixColorShader.Use();
	
	fillMatrixData(textureMatrixColorShaderBackground.getID());
	int UniformPlaneM = glGetUniformLocation(textureMatrixColorShader.getID(), "plane");
	// Clip Plane Set
	glUniform4f(UniformPlaneM, PclipPlane.x, PclipPlane.y, PclipPlane.z, PclipPlane.w);

	for (auto& cube : cubes)
	{
		cube->render();
	}
	
	underWaterPlane.render();

	// Fire render logic
	burningCube->render();
	burningCube2->render();

	renderFlame();
}

//Renders quad to the screen
static void render()
{
	// Enable Clip distance
	glEnable(GL_CLIP_DISTANCE0);
	
	// Water
	// Reflection texture render
	waterReflectionFrameBuffer->bind();
	// Recalculate Camera position for Render Reflection
	float distance = 2 * (camera->getCameraPos().y - waterPlane->getPosition().y);
	camera->setCameraPos(camera->getCameraPos().x, camera->getCameraPos().y - distance, camera->getCameraPos().z);
	camera->invertPitch();
	camera->update();
	glm::vec4 clipPlane = glm::vec4(0, 1, 0, 0); // 0 Height because water object ar on plane Y = 0
	renderScene(clipPlane);
	// Camera Recovery position
	camera->setCameraPos(camera->getCameraPos().x, camera->getCameraPos().y + distance, camera->getCameraPos().z);
	camera->invertPitch();
	camera->update();

	// Refraction texture render
	waterRefractionFrameBuffer->bind();
	clipPlane = glm::vec4(0, -1, 0, 0); // 0 Height because water object ar on plane Y = 0
	renderScene(clipPlane);
	
	waterRefractionFrameBuffer->unbind();

	// Fire
	fireFrameBuffer->bind();
	flameShader.Use();
	flamePlane->setRefractionTexture(fireFrameBuffer->getTexture());

	fillMatrixData(flameShader.getID());

	clipPlane = glm::vec4(0, 1, 0, 0); // 0 Height because water object ar on plane Y = 0
	renderScene(clipPlane);

	fireFrameBuffer->unbind();

	flameShader.Use();
	flamePlane->render();
	
	glDisable(GL_CLIP_DISTANCE0);
	renderScene(clipPlane);
	//Render Water with Reflection and refraction Textures
	renderWater();
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
	delete waterPlane;

	underWaterPlane.clearGPU();
	
	// Clear FBO Water
	delete waterReflectionFrameBuffer;
	delete waterRefractionFrameBuffer;
	
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