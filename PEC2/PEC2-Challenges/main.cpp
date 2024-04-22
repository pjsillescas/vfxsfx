#include <SDL.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "src/stb/stb_image.h"

#include "Challenge2.h"
#include "Challenge3.h"
#include "Challenge32.h"
#include "Challenge5.h"
#include "Challenge6.h"
#include "Challenge7.h"
#include "Challenge8.h"
#include "Challenge8Camera.h"
#include "Challenge9.h"
#include "Challenge10.h"

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;


//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//OpenGL context
SDL_GLContext gContext;

void close()
{
	//Destroy window	
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();

}

int main(int argc, char* argv[])
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		return false;
	}


	//SDL_GL_LoadLibrary(NULL);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

	//Create window
	gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
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


	//int code = (new Challenge2())->run(gWindow);
	//int code = (new Challenge3())->run(gWindow);
	//int code = (new Challenge32())->run(gWindow);
	//int code = (new Challenge5())->run(gWindow);
	//int code = (new Challenge6())->run(gWindow);
	//int code = (new Challenge7())->run(gWindow);
	//int code = (new Challenge8())->run(gWindow);
	//int code = (new Challenge8Camera())->run(gWindow);
	//int code = (new Challenge9())->run(gWindow);
	int code = (new Challenge10())->run(gWindow);

	close();

	return code;

}