//Using SDL, SDL OpenGL, GLAD, standard IO, and strings
#include <SDL.h>
#include <glad/glad.h>
#include "SDL_opengl.h"

#include "Shader.h"

#include <string>
#include <iostream>
#include <fstream>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

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
#define NUM_OBJ 2
GLuint gVAO[NUM_OBJ];
GLuint gVBO[NUM_OBJ];
GLuint gEBO[NUM_OBJ];

Shader coloredShader;

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

				coloredShader.init("colored");
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
	GLfloat vertexData[2][15] =
	{ {
		// x,y, r,g,b
		-0.5f,  0.5f, 1.0, 0.0, 0.0,
		 0.0f, -0.5f, 0.0, 1.0, 0.0,
		-1.0f, -0.5f, 0.0, 0.0, 1.0
	}, {
		 0.5f,  0.5f, 1.0, 1.0, 0.0,
		 0.0f, -0.5f, 1.0, 0.0, 1.0,
		 1.0f, -0.5f, 0.0, 1.0, 1.0
	} };

	//EBO data
	GLuint indexData[2][3] =
	{ {
		0, 1, 2
	}, {
		0, 1, 2
	} };

	for (int i = 0; i < NUM_OBJ; i++) {
		// Create VAO
		glGenVertexArrays(1, &gVAO[i]);
		glBindVertexArray(gVAO[i]);
		{
			//Create VBO
			glGenBuffers(1, &gVBO[i]);
			glBindBuffer(GL_ARRAY_BUFFER, gVBO[i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData[i]), vertexData[i], GL_STATIC_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(2 * sizeof (GLfloat)));
			//Create IBO
			glGenBuffers(1, &gEBO[i]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gEBO[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData[i]), indexData[i], GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
		}
		glBindVertexArray(0);
	}
}

void update()
{
	//No per frame update needed
}

void render()
{
	//Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	//Bind program
	coloredShader.Use();
	//Set VAO
	glBindVertexArray(gVAO[0]);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//Draw VAO
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, NULL);

	//Set VAO
	glBindVertexArray(gVAO[1]);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//Draw VAO
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, NULL);

	//Diable VAO
	glBindVertexArray(0);
	//Unbind program
	glUseProgram(NULL);
}

void close()
{
	//Deallocate program
	coloredShader.deleteProgram();

	//Destroy data in GPU
	glDeleteVertexArrays(sizeof(gVAO), gVAO);
	glDeleteBuffers(sizeof(gVBO), gVBO);
	glDeleteBuffers(sizeof(gEBO), gEBO);

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