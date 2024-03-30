#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>

#include "Clock.h"

#include "TextUtils.h"
#include "EffectTemplate.h"
#include "TransitionEffect.h"
#include "RowTransitionEffect.h"
#include "ColumnTransitionEffect.h"
#include "CircleTransitionEffect.h"
#include "Pec1AudioEffect.h"

const int FONT_SIZE = 12;
const int TIME_TO_DISPLAY_EFFECT = 10;

const bool SHOW_FPS = false;
const bool SHOW_TITLE = false;
const bool SHOW_TIME = false;
const bool SHOW_SECTION = false;


//The window we'll be rendering to
SDL_Window* window = NULL;
//The surface contained by the window
SDL_Surface* screenSurface = NULL;

SDL_Surface* auxSurface = NULL;

bool initSDL(int, int);

void render(EffectTemplate* effect);

void close();

void renderFPS();
void renderTitle(const char* title);
void runEffect(EffectTemplate* effect, bool& quit, SDL_Event& e);
void renderTime(int time);

int transitionNum = 1;

static TransitionEffect* getNewTransition(SDL_Surface* screenSurface, int screenHeight, int screenWidth, EffectTemplate* oldEffect, EffectTemplate* effect) {
	
	transitionNum = (transitionNum + 1) % 3;
	
	switch (transitionNum) {
	case 1:
		return new ColumnTransitionEffect(screenSurface, screenHeight, screenWidth, oldEffect, effect);
		break;
	case 2:
		return new CircleTransitionEffect(screenSurface, screenHeight, screenWidth, oldEffect, effect);
		break;
	case 0:
	default:
		return new RowTransitionEffect(screenSurface, screenHeight, screenWidth, oldEffect, effect);
		break;
	}
}

//#define FULL_SCREEN

int main(int argc, char* args[])
{
#ifndef FULL_SCREEN

	int screenWidth = 640;
	int screenHeight = 480;
	const char* uocFileName = "uoc.png";
	const char* wallFileName = "wall.png";
	const char* bumpFileName = "bump.png";
#else
	int screenWidth = 1440;
	int screenHeight = 1080;
	const char* uocFileName = "uoc1920.png";
	const char* wallFileName = "wall1920.png";
	const char* bumpFileName = "bump1920.png";
#endif // FULL_SCREEN

	//Start up SDL and create window
	if (!initSDL(screenWidth, screenHeight))
	{
		std::cout << "Failed to initialize!\n";
		return 1;
	}
	else
	{
		auxSurface = SDL_CreateRGBSurfaceWithFormat(0, screenWidth, screenHeight, 32, SDL_PIXELFORMAT_RGBA32);
		std::vector<EffectTemplate*> effects{
			(new Pec1AudioEffect(screenSurface, screenHeight, screenWidth, 215, "PEC1 Audio", uocFileName, SHOW_SECTION))->setIsLateInit(true),
		};

		//Main loop flag
		bool quit = false;

		//Event handler
		SDL_Event e;

		for (EffectTemplate*& effect : effects)
		{
			if (!effect->isLateInit())
			{
				effect->init();
			}
		}

		EffectTemplate* oldEffect = NULL;

		for (EffectTemplate*& effect : effects)
		{
			if (effect->isLateInit())
			{
				effect->init();
			}

			effect->start();

			// Transition to the new effect
			if (oldEffect != NULL)
			{
				TransitionEffect* transition = getNewTransition(screenSurface, screenHeight, screenWidth, oldEffect, effect);
				
				oldEffect->setSurface(auxSurface);
				transition->init();
				transition->start();
				runEffect(transition, quit, e);

				delete transition;
			}

			// display effect
			runEffect(effect, quit, e);

			oldEffect = effect;

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

void runEffect(EffectTemplate* effect, bool& quit, SDL_Event&e)
{
	std::cout << "running effect " << effect->getTitle() << std::endl;
	int startTime = Clock::getInstance().getCurrentTime();
	int timeout = effect->getTimeout();
	int elapsedTime = 0;
	bool isEnded = effect->isEnded() || timeout > 0 && elapsedTime > timeout;
	
	//While application is running
	while (!quit && !isEnded)
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
		effect->update((float)Clock::getInstance().getDeltaTime());

		//Render
		effect->render();

		renderFPS();
		renderTitle(effect->getTitle());

		int effectTime = timeout - (Clock::getInstance().getCurrentTime() - startTime) / 1000;
		//std::cout << "effect " << effect->getTitle() << " " << effectTime << std::endl;
		renderTime(effectTime);

		//Update the surface
		SDL_UpdateWindowSurface(window);
		Clock::getInstance().waitFrame();
		int elapsedTime = (Clock::getInstance().getCurrentTime() - startTime) / 1000;
		isEnded = effect->isEnded() || timeout > 0 && elapsedTime > timeout;
	}
}

void renderFPS()
{
	if (!SHOW_FPS)
	{
		return;
	}

	SDL_Color fg = { 0x00,0x00,0xff }, bg = { 0xff,0xff,0xff };      // Blue text on white background
	char fpsText[100];

	sprintf_s(fpsText, "FPS: %.2f", Clock::getInstance().getFPS());
	TextUtils::drawText(screenSurface, fpsText, FONT_SIZE, 0, 0, fg, bg);
}

void renderTitle(const char* title)
{
	if (!SHOW_TITLE)
	{
		return;
	}

	SDL_Color fg = { 0x00,0x00,0xff }, bg = { 0xff,0xff,0xff };      // Blue text on white background
	char titleText[100];

	sprintf_s(titleText, "%s", title);
	TextUtils::drawText(screenSurface, titleText, FONT_SIZE, 0, 3*FONT_SIZE, fg, bg);
}

void renderTime(int time)
{
	if (!SHOW_TIME)
	{
		return;
	}

	if (time > 0)
	{
		SDL_Color fg = { 0x00,0x00,0xff }, bg = { 0xff,0xff,0xff };      // Blue text on white background
		char timeText[100];

		sprintf_s(timeText, "%d", time);
		TextUtils::drawText(screenSurface, timeText, FONT_SIZE, 0, 5 * FONT_SIZE, fg, bg);
	}
}

bool initSDL(int screenWidth, int screenHeight) {

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "SDL could not initialize! SDL_Error: %s\n" << SDL_GetError() << std::endl;
		return false;
	}

	if (TTF_Init() == -1) {
		std::cout << "[ERROR] TTF_Init() Failed with: " << TTF_GetError() << std::endl;
		return false;
	}

	//getFont(12);

	IMG_Init(IMG_INIT_PNG);

	//Create window
	window = SDL_CreateWindow("PEC 1 - Demoscene Gracia", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);

	if (window == NULL)
	{
		std::cout << "Window could not be created! SDL_Error: %s\n" << SDL_GetError();
		return false;
	}
	//Get window surface
	screenSurface = SDL_GetWindowSurface(window);
	return true;
}

void close() {

	TextUtils::close();
	TTF_Quit();

	SDL_FreeSurface(auxSurface);

	//Destroy window
	SDL_DestroyWindow(window);
	//Quit SDL subsystems
	SDL_Quit();
}
