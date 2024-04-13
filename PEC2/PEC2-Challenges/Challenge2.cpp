#include "Challenge2.h"

//Using SDL, SDL OpenGL, standard IO, and strings
#include <SDL.h>
//#include <glad/glad.h>
#include <SDL_opengl.h>
#include <GL\GLU.h>
#include <stdio.h>
#include <string>
#include <iostream>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

bool Challenge2::init()
{
	//Initialization flag
	bool success = true;

	//Use Vsync
	if (SDL_GL_SetSwapInterval(1) < 0)
	{
		printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
	}

	//Initialize OpenGL
	/*
	if (!initGL())
	{
		printf("Unable to initialize OpenGL!\n");
		success = false;
	}
	*/

	return success;
}

bool Challenge2::initGL()
{
	bool success = true;
	GLenum error = GL_NO_ERROR;

	//Initialize Projection Matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//Check for error
	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		printf("Error initializing OpenGL! %s\n", gluErrorString(error));
		success = false;
	}

	//Initialize Modelview Matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Check for error
	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		printf("Error initializing OpenGL! %s\n", gluErrorString(error));
		success = false;
	}

	//Initialize clear color
	glClearColor(0.f, 0.f, 0.f, 1.f);

	//Check for error
	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		printf("Error initializing OpenGL! %s\n", gluErrorString(error));
		success = false;
	}

	return success;
}

void Challenge2::handleKeys(unsigned char key, int x, int y)
{
	//Toggle quad
	if (key == 'q')
	{
		gRenderQuad = !gRenderQuad;
	}
}

const int PERIOD = 1000;
const float FREQ = 1.f / PERIOD;
static int nFrames = 0;

void Challenge2::render()
{
	nFrames = (nFrames + 1) % PERIOD;

	//Clear color buffer
	float r = sinf(FREQ * nFrames * 2.f * M_PI);
	float g = 0.2f;
	float b = 0.5f;
	std::cout << r << std::endl;
	glClearColor(r*r, g, b, 1.f);


	glClear(GL_COLOR_BUFFER_BIT);
}

int Challenge2::run(SDL_Window* gWindow)
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");

		return 0;
	}
	
	//Main loop flag
	bool quit = false;

	//Event handler
	SDL_Event e;

	//Enable text input
	SDL_StartTextInput();

	//While application is running
	while (!quit)
	{
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			//Handle keypress with current mouse position
			else if (e.type == SDL_TEXTINPUT)
			{
				int x = 0, y = 0;
				SDL_GetMouseState(&x, &y);
				handleKeys(e.text.text[0], x, y);
			}
		}

		//Render quad
		render();

		//Update screen
		SDL_GL_SwapWindow(gWindow);
	}

	//Disable text input
	SDL_StopTextInput();

	return 0;
}