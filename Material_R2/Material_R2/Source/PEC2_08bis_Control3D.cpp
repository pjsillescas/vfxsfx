//Using SDL, SDL OpenGL, GLAD, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <glad/glad.h>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "SDL_opengl.h"

#include "Shader.h"

#include <string>
#include <iostream>
#include <fstream>

//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

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

//Graphics data
GLuint gVAO;
GLuint gVBO;
GLuint texture;

Shader TextureMatrixShader;
int UniformModelM;
int UniformViewM;
int UniformProjectionM;

//Camera vectors and varbs
glm::vec3	cameraPos;
glm::vec3	cameraFront;
glm::vec3	cameraUp;
float		cameraSpeed = 0.05f;
enum direction { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT, 
	_DIR_LAST_ };
bool		movement[_DIR_LAST_];
float		yaw;
float		pitch;
float		fov;




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

				TextureMatrixShader.init("TextureMatrix");
				initGL();

			}
		}
	}

	// Init Camera Movements
	for (int i = 0; i < _DIR_LAST_; i++) {
		movement[i] = false;
	}
	yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
	pitch = 0.0f;
	fov = 45.0f;

	SDL_SetRelativeMouseMode(SDL_TRUE);

	return success;
}

void initGL()
{
	//Success flag
	bool success = true;

	//Initialize clear color
	glClearColor(0.f, 0.f, 0.f, 1.f);

	//Zbuffer enable
	glEnable(GL_DEPTH_TEST);

	//VBO data
	GLfloat vertexData[180] =
	{
		// x,y,z				u,v(s,t)
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,	1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,//
		 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,//
		 0.5f,  0.5f,  0.5f,	1.0f, 1.0f,//
		 0.5f,  0.5f,  0.5f,	1.0f, 1.0f,//
		-0.5f,  0.5f,  0.5f,	0.0f, 1.0f,//
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,//

		-0.5f,  0.5f,  0.5f,	1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,	1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,	1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,	0.0f, 1.0f,//
		 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,//
		 0.5f, -0.5f, -0.5f,	1.0f, 0.0f,//
		 0.5f, -0.5f, -0.5f,	1.0f, 0.0f,//
		 0.5f, -0.5f,  0.5f,	0.0f, 0.0f,//
		 0.5f,  0.5f,  0.5f,	0.0f, 1.0f,//

		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,	1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,	0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,	0.0f, 1.0f
	};


	// Create VAO
	glGenVertexArrays(1, &gVAO);
	glBindVertexArray(gVAO);
	{
		//Create VBO
		glGenBuffers(1, &gVBO);
		glBindBuffer(GL_ARRAY_BUFFER, gVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

	}
	glBindVertexArray(0);

	UniformModelM = glGetUniformLocation(TextureMatrixShader.getID(), "model");
	UniformViewM = glGetUniformLocation(TextureMatrixShader.getID(), "view");
	UniformProjectionM = glGetUniformLocation(TextureMatrixShader.getID(), "projection");

	//Create Texture
	SDL_Surface* tempSurface;
	tempSurface = IMG_Load("Assets/textures/texture.png");
	
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	GLint Mode = GL_RGB;
	if (tempSurface->format->BytesPerPixel == 4) Mode = GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, Mode, tempSurface->w, tempSurface->h, 0, Mode, GL_UNSIGNED_BYTE, tempSurface->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	SDL_FreeSurface(tempSurface);

	//Init Camera vectors
	cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
}

void update()
{
	//No per frame update needed
}

void render()
{
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(12.0f, 5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-12.8f, -2.0f, -12.3f),
		glm::vec3(4.4f, -0.4f, -3.5f),
		glm::vec3(-4.7f, 3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f, 2.0f, -2.5f),
		glm::vec3(1.5f, 0.2f, -1.5f),
		glm::vec3(-1.3f, 1.0f, -1.5f)
	};

	glm::mat4 transformMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	float degrees;

	//Clear color buffer & Z buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Bind program
	TextureMatrixShader.Use();

	//Sets Projection Matrix
	float screenWidth = SCREEN_WIDTH;
	float screenHeight = SCREEN_HEIGHT;
	projectionMatrix = glm::perspective(glm::radians(fov), screenWidth / screenHeight, 0.1f, 100.0f);
	glUniformMatrix4fv(UniformProjectionM, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	//Sets View Matrix (Camera)
	viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glUniformMatrix4fv(UniformViewM, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	//Sets texture
	glBindTexture(GL_TEXTURE_2D, texture);

	//Set VAO
	glBindVertexArray(gVAO);
	Uint32 ticktime = SDL_GetTicks();
	for (int i = 0; i < 10; i++) {
		//Move figure
		degrees = ( ticktime / ((i+1)*6.0f));
		//Do Matrix movements
		transformMatrix = glm::mat4();
		transformMatrix = glm::translate(transformMatrix, cubePositions[i]);
		transformMatrix = glm::rotate(transformMatrix, glm::radians(degrees), glm::vec3(0.5, 1.0, 1.0));
		glUniformMatrix4fv(UniformModelM, 1, GL_FALSE, glm::value_ptr(transformMatrix));

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//Draw VAO
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	//Diable VAO
	glBindVertexArray(0);
	//Unbind program
	glUseProgram(NULL);
}

void close()
{
	//Deallocate program
	TextureMatrixShader.deleteProgram();

	//Destroy data in GPU
	glDeleteVertexArrays(1, &gVAO);
	glDeleteBuffers(1, &gVBO);

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
				if (e.type == SDL_KEYDOWN) {
					if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
						quit = true;
					}
					// Camera Move setting
					if ((e.key.keysym.scancode == SDL_SCANCODE_UP) ||
						(e.key.keysym.scancode == SDL_SCANCODE_W)) {
						movement[DIR_UP] = true;
					}
					if ((e.key.keysym.scancode == SDL_SCANCODE_DOWN) ||
						(e.key.keysym.scancode == SDL_SCANCODE_S)) {
						movement[DIR_DOWN] = true;
					}
					if ((e.key.keysym.scancode == SDL_SCANCODE_LEFT) ||
						(e.key.keysym.scancode == SDL_SCANCODE_A)) {
						movement[DIR_LEFT] = true;
					}
					if ((e.key.keysym.scancode == SDL_SCANCODE_RIGHT) ||
						(e.key.keysym.scancode == SDL_SCANCODE_D)) {
						movement[DIR_RIGHT] = true;
					}
				}
				if (e.type == SDL_KEYUP) {
					// Camera Move Stop
					if ((e.key.keysym.scancode == SDL_SCANCODE_UP) ||
						(e.key.keysym.scancode == SDL_SCANCODE_W)) {
						movement[DIR_UP] = false;
					}
					if ((e.key.keysym.scancode == SDL_SCANCODE_DOWN) ||
						(e.key.keysym.scancode == SDL_SCANCODE_S)) {
						movement[DIR_DOWN] = false;
					}
					if ((e.key.keysym.scancode == SDL_SCANCODE_LEFT) ||
						(e.key.keysym.scancode == SDL_SCANCODE_A)) {
						movement[DIR_LEFT] = false;
					}
					if ((e.key.keysym.scancode == SDL_SCANCODE_RIGHT) ||
						(e.key.keysym.scancode == SDL_SCANCODE_D)) {
						movement[DIR_RIGHT] = false;
					}
				}
				// Capture Mouse
				if (e.type == SDL_MOUSEMOTION) {
					float xoffset = (float)e.motion.xrel;
					float yoffset = (float)e.motion.yrel;

					float sensitivity = 0.05f;
					xoffset *= sensitivity;
					yoffset *= sensitivity;

					yaw += xoffset;
					pitch += yoffset;

					if (pitch > 89.0f)
						pitch = 89.0f;
					if (pitch < -89.0f)
						pitch = -89.0f;

					glm::vec3 front;
					front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
					front.y = sin(glm::radians(pitch));
					front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
					cameraFront = glm::normalize(front);
				}
				// Capture Mouse Hweel for Zoom
				if (e.type == SDL_MOUSEWHEEL) {
					if (fov >= 1.0f && fov <= 45.0f)
						fov -= e.wheel.y;
					if (fov <= 1.0f)
						fov = 1.0f;
					if (fov >= 45.0f)
						fov = 45.0f;
				}
				//User requests quit
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
			}

			// Move Camera
			if (movement[DIR_UP]) {
				cameraPos += cameraSpeed * cameraFront;
			}
			if (movement[DIR_DOWN]) {
				cameraPos -= cameraSpeed * cameraFront;
			}
			if (movement[DIR_LEFT]) {
				cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
			}
			if (movement[DIR_RIGHT]) {
				cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
			}
			//Render quad
			render();

			//Update screen
			SDL_GL_SwapWindow(gWindow);
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}