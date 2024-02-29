#include <SDL.h>
#include <iostream>

#include "Clock.h"

#include "EffectTemplate.h"
#include "StarsEffect.h"
#include "StarsEffectChallenge2.h"
#include "PlasmaEffect.h"
#include "PlasmaEffectChallenge3.h"
#include "FireEffect.h"
#include "DistortionEffect.h"
#include "BumpmapEffect.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int FULL_SCREEN_WIDTH = 1920;
const int FULL_SCREEN_HEIGHT = 1080;

//const int FULL_SCREEN_WIDTH = 800;
//const int FULL_SCREEN_HEIGHT = 480;

//The window we'll be rendering to
SDL_Window* window = NULL;
//The surface contained by the window
SDL_Surface* screenSurface = NULL;

bool initSDL(int, int);

void render(EffectTemplate* effect);

void close();

int main(int argc, char* args[])
{
	int screenWidth = SCREEN_WIDTH;
	int screenHeight = SCREEN_HEIGHT;
	
	//int screenWidth = FULL_SCREEN_WIDTH;
	//int screenHeight = FULL_SCREEN_HEIGHT;

	EffectTemplate* effect = NULL;

	//Start up SDL and create window
	if (!initSDL(screenWidth, screenHeight))
	{
		std::cout << "Failed to initialize!\n";
		return 1;
	}
	else
	{
		//effect = new StarsEffect(screenSurface, screenHeight, screenWidth);
		//effect = new StarsEffectChallenge2(screenSurface, screenHeight, screenWidth);
		//effect = new PlasmaEffect(screenSurface, screenHeight, screenWidth);
		//effect = new PlasmaEffectChallenge3(screenSurface, screenHeight, screenWidth);
		//effect = new FireEffect(screenSurface, screenHeight, screenWidth);
		effect = new DistortionEffect(screenSurface, screenHeight, screenWidth);
		//effect = new BumpmapEffect(screenSurface, screenHeight, screenWidth);
		effect->init();

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
				if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
				{
					quit = true;
				}
				
				//User requests quit
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
			}

			// updates all
			effect->update(Clock::getInstance().getDeltaTime());

			//Render
			render(effect);

			//Update the surface
			SDL_UpdateWindowSurface(window);
			Clock::getInstance().waitTime();
		}
	}

	if (effect != NULL)
	{
		delete effect;
	}

	//Free resources and close SDL
	close();

	return 0;
}


bool initSDL(int screenWidth, int screenHeight) {

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "SDL could not initialize! SDL_Error: %s\n" << SDL_GetError();
		return false;
	}
	//Create window
	window = SDL_CreateWindow("SDL Stars Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);

	if (window == NULL)
	{
		std::cout << "Window could not be created! SDL_Error: %s\n" << SDL_GetError();
		return false;
	}
	//Get window surface
	screenSurface = SDL_GetWindowSurface(window);
	return true;
}

void render(EffectTemplate* effect) {
	//Fill with black
	SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0, 0, 0));

	effect->render();
}

void close() {

	//Destroy window
	SDL_DestroyWindow(window);
	//Quit SDL subsystems
	SDL_Quit();
}
