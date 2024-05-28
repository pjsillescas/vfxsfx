#include <SDL.h>
#include <iostream>
#include <vector>

#include "Clock.h"

#include "EffectTemplate.h"
#include "Pec2Effect.h"

//The window we'll be rendering to
SDL_Window* window = NULL;

bool initSDL(int screenWidth, int screenHeight);

void close();

void runEffect(EffectTemplate* effect, bool& quit, SDL_Event& e);

//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

int main(int argc, char* args[])
{
	int screenWidth = SCREEN_WIDTH;
	int screenHeight = SCREEN_HEIGHT;

	//Start up SDL and create window
	if (!initSDL(screenWidth, screenHeight))
	{
		std::cout << "Failed to initialize!\n";
		return 1;
	}
	else
	{
		std::vector<EffectTemplate*> effects{
			(new Pec2Effect(NULL, screenHeight, screenWidth, 215, "PEC2 Fire", window)),
		};

		//Main loop flag
		bool quit = false;

		//Event handler
		SDL_Event e;

		for (EffectTemplate*& effect : effects)
		{
			effect->init();
			effect->start();

			// display effect
			runEffect(effect, quit, e);

			if (quit)
			{
				// don't start the remaining effects
				break;
			}
		}

		for (EffectTemplate*& effect : effects)
		{
			delete effect;
		}

	}

	//Free resources and close SDL
	close();

	return 0;
}

void runEffect(EffectTemplate* effect, bool& quit, SDL_Event& e)
{
	std::cout << "running effect " << effect->getTitle() << std::endl;
	int startTime = Clock::getInstance().getCurrentTime();
	int timeout = effect->getTimeout();
	int elapsedTime = 0;
	bool isEnded = effect->isEnded() || timeout > 0 && elapsedTime > timeout;

	//While application is running
	while (!quit && !isEnded)
	{
		// updates all
		quit = effect->update((float)Clock::getInstance().getDeltaTime());

		//Render
		effect->render();

		int effectTime = timeout - (Clock::getInstance().getCurrentTime() - startTime) / 1000;

		//Update the surface
		SDL_GL_SwapWindow(window);
		Clock::getInstance().waitFrame();
		int elapsedTime = (Clock::getInstance().getCurrentTime() - startTime) / 1000;
		isEnded = effect->isEnded() || timeout > 0 && elapsedTime > timeout;
	}
}

bool initSDL(int screenWidth, int screenHeight)
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "SDL could not initialize! SDL_Error: %s\n" << SDL_GetError() << std::endl;
		return false;
	}

	//Create window
	window = SDL_CreateWindow("PEC 2 - Fuego en el shader", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	if (window == NULL)
	{
		std::cout << "Window could not be created! SDL_Error: %s\n" << SDL_GetError();
		return false;
	}
	return true;
}

void close()
{
	//Destroy window
	SDL_DestroyWindow(window);
	//Quit SDL subsystems
	SDL_Quit();
}
