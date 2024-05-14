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
#include "WaterFrameBuffers.h"
#include "WaterObj.h"
#include "FireObj.h"
#include "FireFrameBuffers.h"
#include "PlayerController.h"

#include <string>
#include <iostream>
#include <fstream>

#include "global_defines.h"

//Starts up SDL, creates window, and initializes OpenGL
bool init();

//Initializes rendering program and clear color
void initGL();

//Per frame update
void update();

//Renders quad to the screen
void render();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//OpenGL context
SDL_GLContext gContext;

int UniformViewM;
int UniformProjectionM;
int UniformPlaneM;

Shader WaterShader;
Shader TextureMatrixColorShader;
Shader FireShader;
Shader FlameShader;
Camera3D FirstCamera;
PlayerController* playerController;
Object3D* burningCube;
Object3D* burningCube2;

WaterObj MyQuad;
std::vector<Object3D*> MyCubes;
Object3D underWaterPlane;
FireObj* firePlane;
Object3D* flamePlane;

// FBO for Water
WaterFrameBuffers *WaterFBO;
FireFrameBuffers* FireFBO;
// Clip Plane
glm::vec4 clipPlane;


glm::vec3 cubePositions[] = {
	glm::vec3(2.0f,  5.0f, 5.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -2.0f),
	glm::vec3(3.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, 7.5f),
	glm::vec3(1.3f, -2.0f, -3.5f),
	glm::vec3(2.5f,  2.0f, 2.5f),
	glm::vec3(-2.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, 1.5f)
};


bool init()
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
		printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
	}

	initGL();
	playerController = new PlayerController(&FirstCamera);

	SDL_SetRelativeMouseMode(SDL_TRUE);

	return success;
}

static Object3D* createCube(glm::vec3 position)
{
	Object3D* object = new Object3D();
	object->loadObjFromDisk("Assets/Cube.txt");
	object->loadTextureFromDisk("Assets/textures/texture.png");
	object->setShader(&TextureMatrixColorShader);
	object->setPosition(position);

	return object;
}

void initGL()
{
	WaterShader.init("Water4");
	TextureMatrixColorShader.init("TextureMatrixColorClip");
	
	//Initialize clear color
	glClearColor(0.f, 0.f, 0.f, 1.f);

	//Zbuffer enable
	glEnable(GL_DEPTH_TEST);

	// Back face culling
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);

	//Generate Objects
	MyQuad.loadObjFromDisk("Assets/WaterPlane.txt");
	MyQuad.setShader(&WaterShader);
	MyQuad.loadTextureFromDisk("Assets/textures/waterDUDV.png");
	MyQuad.setTexture3(MyQuad.getTexture()); // Load texture and change ID to texture 3;
	MyQuad.loadTextureFromDisk("Assets/textures/normaltexture.jpg");
	MyQuad.setTexture4(MyQuad.getTexture()); // Load texture and change ID to texture 4;

	for (unsigned int i = 0; i < 9; i++)
	{
		//MyCubes.push_back(createCube(cubePositions[i]));
	}

	burningCube = createCube(glm::vec3(0.f,0.f,-1.f));
	burningCube2 = createCube(glm::vec3(0.f, 0.2f, -2.5f));

	underWaterPlane.loadObjFromDisk("Assets/Pool.txt");
	underWaterPlane.setShader(&TextureMatrixColorShader);
	underWaterPlane.loadTextureFromDisk("Assets/textures/floor.jpg");
	underWaterPlane.setPosition(glm::vec3(0.0, -4.0, 0.0));

	FireShader.init("Fire");
	firePlane = new FireObj();
	firePlane->loadObjFromDisk("Assets/FirePlane.txt");
	firePlane->setShader(&FireShader);
	//firePlane->loadTextureFromDisk("Assets/textures/floor.jpg");
	firePlane->setPosition(glm::vec3(0.0f, 5.f, -1.0f));
	firePlane->setRotationAxis(glm::vec3(1,0,-1));

	firePlane->loadTextureFromDisk("Assets/textures/waterDUDV.png");
	firePlane->setTexture3(firePlane->getTexture()); // Load texture and change ID to texture 3;
	firePlane->loadTextureFromDisk("Assets/textures/normaltexture.jpg");
	firePlane->setTexture4(firePlane->getTexture()); // Load texture and change ID to texture 4;

	FlameShader.init("Flame");
	flamePlane = new Object3D();
	flamePlane->loadObjFromDisk("Assets/FlamePlane.txt");
	flamePlane->setShader(&FlameShader);
	flamePlane->setPosition(glm::vec3(0.0f, 2.f, -1.0f));
	
	// Create Frame Buffer Objects (FBO)
	WaterFBO = new WaterFrameBuffers();
	FireFBO = new FireFrameBuffers();
}

void update()
{
	playerController->update();
}

float time1 = 0;

static void renderWater()
{
	glm::vec3 camPos;
	glm::vec3 lightPos;
	glm::vec3 lightColor;
	
	// fire
	//Bind program
	FireShader.Use();
	UniformViewM = glGetUniformLocation(FireShader.getID(), "view");
	UniformProjectionM = glGetUniformLocation(FireShader.getID(), "projection");
	// Active Textures and Set them
	firePlane->setTexture(FireFBO->getReflectionTexture());
	firePlane->setTexture2(FireFBO->getRefractionTexture());
	//Sets Projection Matrix
	FirstCamera.setUniformProjectionMatrix(SCREEN_WIDTH, SCREEN_HEIGHT, UniformProjectionM);
	//Sets View Matrix (Camera)
	FirstCamera.setUniformViewMatrix(UniformViewM);
	// Give Camera Vector to Shader
	camPos = FirstCamera.getCameraPos();
	glUniform3f(firePlane->getUniformCamPos(), camPos.x, camPos.y, camPos.z);
	// Give an imaginary Sun light position and color
	lightPos = glm::vec3(-3.0, 10.0, -5.0);
	lightColor = glm::vec3(1.0, 1.0, 1.0);
	glUniform3f(firePlane->getUniformLightPos(), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(firePlane->getUniformLightColor(), lightColor.x, lightColor.y, lightColor.z);

	// Draw objects
	firePlane->render();

	FlameShader.Use();
	glUniform1f(glGetUniformLocation(FlameShader.getID(), "time"), time1);
	glUniform2f(glGetUniformLocation(FlameShader.getID(), "iResolution"), SCREEN_WIDTH, SCREEN_HEIGHT);
	//time1 += 0.0000001f;
	time1 += 0.02f;
	UniformViewM = glGetUniformLocation(FlameShader.getID(), "view");
	UniformProjectionM = glGetUniformLocation(FlameShader.getID(), "projection");
	FirstCamera.setUniformProjectionMatrix(SCREEN_WIDTH, SCREEN_HEIGHT, UniformProjectionM);
	FirstCamera.setUniformViewMatrix(UniformViewM);

	flamePlane->render();
	// /fire

	//Bind program
	WaterShader.Use();
	UniformViewM = glGetUniformLocation(WaterShader.getID(), "view");
	UniformProjectionM = glGetUniformLocation(WaterShader.getID(), "projection");
	// Active Textures and Set them
	MyQuad.setTexture(WaterFBO->getReflectionTexture());
	MyQuad.setTexture2(WaterFBO->getRefractionTexture());
	//Sets Projection Matrix
	FirstCamera.setUniformProjectionMatrix(SCREEN_WIDTH, SCREEN_HEIGHT, UniformProjectionM);
	//Sets View Matrix (Camera)
	FirstCamera.setUniformViewMatrix(UniformViewM);
	// Give Camera Vector to Shader
	camPos = FirstCamera.getCameraPos();
	glUniform3f(MyQuad.getUniformCamPos(), camPos.x, camPos.y, camPos.z);
	// Give an imaginary Sun light position and color
	lightPos = glm::vec3(-3.0, 10.0, -5.0);
	lightColor = glm::vec3(1.0, 1.0, 1.0);
	glUniform3f(MyQuad.getUniformLightPos(), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(MyQuad.getUniformLightColor(), lightColor.x, lightColor.y, lightColor.z);
	// Draw objects
	MyQuad.render();
}

static void renderScene(glm::vec4 PclipPlane)
{
	//Clear color buffer & Z buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Bind program
	TextureMatrixColorShader.Use();
	UniformViewM = glGetUniformLocation(TextureMatrixColorShader.getID(), "view");
	UniformProjectionM = glGetUniformLocation(TextureMatrixColorShader.getID(), "projection");
	UniformPlaneM = glGetUniformLocation(TextureMatrixColorShader.getID(), "plane");
	// Clip Plane Set
	glUniform4f(UniformPlaneM, PclipPlane.x, PclipPlane.y, PclipPlane.z, PclipPlane.w);
	//Sets Projection Matrix
	FirstCamera.setUniformProjectionMatrix(SCREEN_WIDTH, SCREEN_HEIGHT, UniformProjectionM);
	//Sets View Matrix (Camera)
	FirstCamera.setUniformViewMatrix(UniformViewM);

	for (unsigned int i = 0; i < MyCubes.size(); i++)
	{
		// Draw objects
		MyCubes[i]->render();
	}
	underWaterPlane.render();

	// Fire render logic
	burningCube->render();
	burningCube2->render();
	//FireShader.Use();
	//firePlane->render();
	//std::cout << "render flame begin" << std::endl;
	//FlameShader.Use();
	//flamePlane->render();
	//std::cout << "render flame end" << std::endl;
}

static void renderFire()
{
	// Fire
	FireFBO->bindReflectionFrameBuffer();
	// float distance = 2 * (FirstCamera.getCameraPos().z - firePlane->getPosition().z);
	//glm::vec3 deltaPos(0, -2.5 * firePlane->getPosition().y,  0*distance);
	//glm::vec3 deltaPos(0,3 -FirstCamera.getCameraPos().y, 0 * distance);
	//glm::vec3 deltaPos = firePlane->getPosition() - glm::vec3(0, 0, -2);
	//glm::vec3 oldPosition = FirstCamera.getCameraPos();
	float distance = 2 * (FirstCamera.getCameraPos().y - firePlane->getPosition().y);
	FirstCamera.setCameraPos(FirstCamera.getCameraPos().x, FirstCamera.getCameraPos().y - distance, FirstCamera.getCameraPos().z);
	//glm::vec3 newPosition = oldPosition + deltaPos;
	//glm::vec3 newPosition = firePlane->getPosition() + glm::vec3(1.5, 1.5, 0);
	//glm::vec3 newPosition = firePlane->getPosition() + glm::vec3(0,0,1);
	//FirstCamera.setCameraPos(FirstCamera.getCameraPos().x, FirstCamera.getCameraPos().y - 2*firePlane->getPosition().y, FirstCamera.getCameraPos().z + distance);
	//FirstCamera.setCameraPos(newPosition.x, newPosition.y, newPosition.z);
	FirstCamera.update();
	clipPlane = glm::vec4(0, 0, -1, 0); // 0 Height because water object ar on plane Y = 0
	renderScene(clipPlane);
	//FireFBO->unbindCurrentFrameBuffer();
	/*
	FireFBO->bindRefractionFrameBuffer();
	clipPlane = glm::vec4(0, 0, -1, 0); // 0 Height because water object ar on plane Y = 0
	renderScene(clipPlane);
	*/
	//FirstCamera.setCameraPos(FirstCamera.getCameraPos().x, FirstCamera.getCameraPos().y + 2 * firePlane->getPosition().y, FirstCamera.getCameraPos().z - distance);
	//FirstCamera.setCameraPos(oldPosition.x, oldPosition.y, oldPosition.z);
	FirstCamera.setCameraPos(FirstCamera.getCameraPos().x, FirstCamera.getCameraPos().y + distance, FirstCamera.getCameraPos().z);

	FirstCamera.update();
	FireFBO->unbindCurrentFrameBuffer();
}

void render()
{

	// Enable Clip distance
	glEnable(GL_CLIP_DISTANCE0);

	//glDisable(GL_CLIP_DISTANCE0);

	//glEnable(GL_CLIP_DISTANCE0);
	// Water
	// Reflection texture render
	WaterFBO->bindReflectionFrameBuffer();
	// Recalculate Camera position for Render Reflection
	float distance = 2 * (FirstCamera.getCameraPos().y - MyQuad.getPosition().y);
	FirstCamera.setCameraPos(FirstCamera.getCameraPos().x, FirstCamera.getCameraPos().y - distance, FirstCamera.getCameraPos().z);
	FirstCamera.invertPitch();
	FirstCamera.update();
	clipPlane = glm::vec4(0, 1, 0, 0); // 0 Height because water object ar on plane Y = 0
	renderScene(clipPlane);
	// Camera Recovery position
	FirstCamera.setCameraPos(FirstCamera.getCameraPos().x, FirstCamera.getCameraPos().y + distance, FirstCamera.getCameraPos().z);
	FirstCamera.invertPitch();
	FirstCamera.update();

	// Refraction texture render
	WaterFBO->bindRefractionFrameBuffer();
	clipPlane = glm::vec4(0, -1, 0, 0); // 0 Height because water object ar on plane Y = 0
	renderScene(clipPlane);
	
	WaterFBO->unbindCurrentFrameBuffer();
	
	renderFire();

	
	glDisable(GL_CLIP_DISTANCE0);
	renderScene(clipPlane);
	//Render Water with Reflection and refraction Textures
	renderWater();
}

void close()
{
	delete playerController;
	delete firePlane;
	delete flamePlane;

	delete burningCube;
	delete burningCube2;

	//Deallocate program
	WaterShader.deleteProgram();
	TextureMatrixColorShader.deleteProgram();

	//Destroy data in GPU
	MyQuad.clearGPU();
	for (unsigned int i = 0; i < MyCubes.size(); i++)
	{
		MyCubes[i]->clearGPU();
	}
	underWaterPlane.clearGPU();
	// Clear FBO Water
	WaterFBO->cleanUp();
	delete WaterFBO;

	FireFBO->cleanUp();
	delete FireFBO;

	FireShader.deleteProgram();
	FlameShader.deleteProgram();
	
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
		printf("Failed to initialize!\n");
		return 1;
	}
	
	printf("* Use WASD and Mouse to walk in 3d World *");
	//Main loop flag
	bool quit = false;

	//While application is running
	while (!quit)
	{
		quit = playerController->handleInput();

		//Update
		update();
		//Render
		render();

		//Update screen
		SDL_GL_SwapWindow(gWindow);
	}

	//Free resources and close SDL
	close();

	return 0;
}