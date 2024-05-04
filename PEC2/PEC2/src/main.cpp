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
Camera3D FirstCamera;
WaterObj MyQuad;
std::vector<Object3D*> MyCubes;
Object3D underWaterPlane;
// FBO for Water
WaterFrameBuffers *WaterFBO;
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


enum direction { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT, 
	_DIR_LAST_ };
bool		movement[_DIR_LAST_];

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Use OpenGL 3.3 core
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		//Create window
		gWindow = SDL_CreateWindow("SDL + OpenGL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create context
			gContext = SDL_GL_CreateContext(gWindow);
			if (gContext == NULL)
			{
				printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
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

				FirstCamera.init();
				initGL();

			}
		}
	}

	// Init Movements
	for (int i = 0; i < _DIR_LAST_; i++)
	{
		movement[i] = false;
	}

	SDL_SetRelativeMouseMode(SDL_TRUE);

	return success;
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
		MyCubes.push_back(new Object3D());
		MyCubes[i]->loadObjFromDisk("Assets/Cube.txt");
		MyCubes[i]->loadTextureFromDisk("Assets/textures/texture.png");
		MyCubes[i]->setShader(&TextureMatrixColorShader);
		MyCubes[i]->setPosition(cubePositions[i]);
	}

	underWaterPlane.loadObjFromDisk("Assets/Pool.txt");
	underWaterPlane.setShader(&TextureMatrixColorShader);
	underWaterPlane.loadTextureFromDisk("Assets/textures/floor.jpg");
	underWaterPlane.setPosition(glm::vec3(0.0, -4.0, 0.0));
	// Create Frame Buffer Objects (FBO)
	WaterFBO = new WaterFrameBuffers;
}

void update()
{
	// Move Camera
	FirstCamera.update(movement);
}

void renderWater()
{
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
	glm::vec3 camPos = FirstCamera.getCameraPos();
	glUniform3f(MyQuad.getUniformCamPos(), camPos.x, camPos.y, camPos.z);
	// Give an imaginary Sun light position and color
	glm::vec3 lightPos = glm::vec3(-3.0, 10.0, -5.0);
	glm::vec3 lightColor = glm::vec3(1.0, 1.0, 1.0);
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

	for (unsigned int i = 0; i < MyCubes.size(); i++) {
		// Draw objects
		MyCubes[i]->render();
	}
	underWaterPlane.render();
}

void render()
{

	// Enable Clip distance
	glEnable(GL_CLIP_DISTANCE0);

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
	glDisable(GL_CLIP_DISTANCE0);
	renderScene(clipPlane);
	//Render Water with Reflection and refraction Textures
	renderWater();
}

void close()
{
	//Deallocate program
	WaterShader.deleteProgram();
	TextureMatrixColorShader.deleteProgram();

	//Destroy data in GPU
	MyQuad.clearGPU();
	for (unsigned int i = 0; i < MyCubes.size(); i++) {
		MyCubes[i]->clearGPU();
	}
	underWaterPlane.clearGPU();
	// Clear FBO Water
	WaterFBO->cleanUp();

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
	}
	else
	{
		printf("* Use WASD and Mouse to walk in 3d World *");
		//Main loop flag
		bool quit = false;

		//Event handler
		SDL_Event e;

		//While application is running
		while (!quit)
		{
			//Handle events on queue
			while (SDL_PollEvent(&e) != 0)
			{
				if (e.type == SDL_KEYDOWN)
				{
					if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
					{
						quit = true;
					}
					
					// Camera Move setting
					if ((e.key.keysym.scancode == SDL_SCANCODE_UP) ||
						(e.key.keysym.scancode == SDL_SCANCODE_W))
					{
						movement[DIR_UP] = true;
					}
					if ((e.key.keysym.scancode == SDL_SCANCODE_DOWN) ||
						(e.key.keysym.scancode == SDL_SCANCODE_S))
					{
						movement[DIR_DOWN] = true;
					}
					if ((e.key.keysym.scancode == SDL_SCANCODE_LEFT) ||
						(e.key.keysym.scancode == SDL_SCANCODE_A))
					{
						movement[DIR_LEFT] = true;
					}
					if ((e.key.keysym.scancode == SDL_SCANCODE_RIGHT) ||
						(e.key.keysym.scancode == SDL_SCANCODE_D))
					{
						movement[DIR_RIGHT] = true;
					}
				}

				if (e.type == SDL_KEYUP)
				{
					// Camera Move Stop
					if ((e.key.keysym.scancode == SDL_SCANCODE_UP) ||
						(e.key.keysym.scancode == SDL_SCANCODE_W))
					{
						movement[DIR_UP] = false;
					}
					if ((e.key.keysym.scancode == SDL_SCANCODE_DOWN) ||
						(e.key.keysym.scancode == SDL_SCANCODE_S))
					{
						movement[DIR_DOWN] = false;
					}
					if ((e.key.keysym.scancode == SDL_SCANCODE_LEFT) ||
						(e.key.keysym.scancode == SDL_SCANCODE_A))
					{
						movement[DIR_LEFT] = false;
					}
					if ((e.key.keysym.scancode == SDL_SCANCODE_RIGHT) ||
						(e.key.keysym.scancode == SDL_SCANCODE_D))
					{
						movement[DIR_RIGHT] = false;
					}
				}
				
				// Capture Mouse
				if (e.type == SDL_MOUSEMOTION)
				{
					FirstCamera.setHeadTurn(e.motion.xrel, e.motion.yrel);
				}
				
				// Capture Mouse wheel for Zoom
				if (e.type == SDL_MOUSEWHEEL)
				{
					FirstCamera.setAddZoom(e.wheel.y);
				}
				
				//User requests quit
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
			}

			//Update
			update();
			//Render
			render();

			//Update screen
			SDL_GL_SwapWindow(gWindow);
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}