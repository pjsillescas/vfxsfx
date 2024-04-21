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
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

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
GLuint gEBO;
GLuint texture;

Shader TextureMatrixShader;
int UniformModelM;
int UniformViewM;
int UniformProjectionM;

float posCam = 0.4f;

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
		gWindow = SDL_CreateWindow("OpenGL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
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

				TextureMatrixShader.init("TextureMatrix2D");
				initGL();

			}
		}
	}

	return success;
}

void initGL()
{
	//Success flag
	bool success = true;

	//Initialize clear color
	glClearColor(0.f, 0.f, 0.f, 1.f);

	//VBO data
	GLfloat vertexData[16] =
	{
		// x,y,			u,v(s,t)
		-0.5f,  0.5f,	0.0,1.0,
		 0.5f,  0.5f,   1.0,1.0,
		 0.5f, -0.5f,	1.0,0.0,
		-0.5f, -0.5f,	0.0,0.0
	};

	//EBO data
	GLuint indexData[6] =
	{
		0, 1, 2,   0, 2, 3
	};

	// Create VAO
	glGenVertexArrays(1, &gVAO);
	glBindVertexArray(gVAO);
	{
		//Create VBO
		glGenBuffers(1, &gVBO);
		glBindBuffer(GL_ARRAY_BUFFER, gVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
		//Create IBO
		glGenBuffers(1, &gEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

	}
	glBindVertexArray(0);

	UniformModelM = glGetUniformLocation(TextureMatrixShader.getID(), "model");
	UniformViewM = glGetUniformLocation(TextureMatrixShader.getID(), "view");
	UniformProjectionM = glGetUniformLocation(TextureMatrixShader.getID(), "projection");

	//Create Texture
	SDL_Surface* tempSurface;
	tempSurface = IMG_Load("Assets/textures/SW-E4.png");
	
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	GLint Mode = GL_RGB;
	if (tempSurface->format->BytesPerPixel == 4) Mode = GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, Mode, tempSurface->w, tempSurface->h, 0, Mode, GL_UNSIGNED_BYTE, tempSurface->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	SDL_FreeSurface(tempSurface);
}

void update()
{
	posCam -= 0.005f;
}

void render()
{
	glm::mat4 transformMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	float degrees;
	degrees = (SDL_GetTicks()/10.0f);


	//Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	//Bind program
	TextureMatrixShader.Use();

	//Sets Projection Matrix
	float screenWidth = SCREEN_WIDTH;
	float screenHeight = SCREEN_HEIGHT;
	projectionMatrix = glm::perspective(glm::radians(40.0f), screenWidth / screenHeight, 0.1f, 100.0f);
	glUniformMatrix4fv(UniformProjectionM, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	//Sets View Matrix (Camera)
	viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0, 0.0, posCam));
	glUniformMatrix4fv(UniformViewM, 1, GL_FALSE, glm::value_ptr(viewMatrix));


	//Sets texture
	glBindTexture(GL_TEXTURE_2D, texture);

	//Move figure
	//Do Matrix movements
	transformMatrix = glm::mat4();
	transformMatrix = glm::translate(transformMatrix, glm::vec3(0.0, -0.5, 0.0));
	transformMatrix = glm::rotate(transformMatrix, glm::radians(-70.0f), glm::vec3(1.0, 0.0, 0.0));
	glUniformMatrix4fv(UniformModelM, 1, GL_FALSE, glm::value_ptr(transformMatrix));

	//Set VAO
	glBindVertexArray(gVAO);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//Draw VAO
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

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
	glDeleteVertexArrays(sizeof(gVAO), &gVAO);
	glDeleteBuffers(sizeof(gVBO), &gVBO);
	glDeleteBuffers(sizeof(gEBO), &gEBO);

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
				}
				//User requests quit
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
			}

			update();

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