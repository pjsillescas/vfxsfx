#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>

#include "Clock.h"

#include "EffectTemplate.h"
#include "StarsEffect.h"
#include "StarsEffectChallenge2.h"
#include "PlasmaEffect.h"
#include "PlasmaEffectChallenge3.h"
#include "FireEffect.h"
#include "DistortionEffect.h"
#include "BumpmapEffect.h"
#include "FractalEffect.h"
#include "TunnelEffect.h"
#include "RotozoomEffect.h"
#include "ParticleEffect.h"
#include "C3DEffect.h"
#include "TerraEffect.h"
#include "SyncEffect.h"
#include "TransitionEffect.h"
#include "RowTransitionEffect.h"
#include "ColumnTransitionEffect.h"
#include "CircleTransitionEffect.h"

const int FONT_SIZE = 12;
const int TIME_TO_DISPLAY_EFFECT = 10;

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int FULL_SCREEN_WIDTH = 1920;
const int FULL_SCREEN_HEIGHT = 1080;

//The window we'll be rendering to
SDL_Window* window = NULL;
//The surface contained by the window
SDL_Surface* screenSurface = NULL;

SDL_Surface* auxSurface = NULL;

bool initSDL(int, int);

void render(EffectTemplate* effect);

void close();

void renderFPS();
void renderCountdown(int counter);

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

int main(int argc, char* args[])
{
	int screenWidth = SCREEN_WIDTH;
	int screenHeight = SCREEN_HEIGHT;
	
	//int screenWidth = FULL_SCREEN_WIDTH;
	//int screenHeight = FULL_SCREEN_HEIGHT;

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
			new StarsEffect(screenSurface, screenHeight, screenWidth, 5),
			new StarsEffectChallenge2(screenSurface, screenHeight, screenWidth, 5),
			new PlasmaEffect(screenSurface, screenHeight, screenWidth, 10),
			new PlasmaEffectChallenge3(screenSurface, screenHeight, screenWidth, 10),
			new FireEffect(screenSurface, screenHeight, screenWidth, 10),
			new DistortionEffect(screenSurface, screenHeight, screenWidth, 10),
			new BumpmapEffect(screenSurface, screenHeight, screenWidth, 10),
			new FractalEffect(screenSurface, screenHeight, screenWidth, 10),
			new TunnelEffect(screenSurface, screenHeight, screenWidth, 10),
			new RotozoomEffect(screenSurface, screenHeight, screenWidth, 10),
			new ParticleEffect(screenSurface, screenHeight, screenWidth, 10),
			new C3DEffect(screenSurface, screenHeight, screenWidth, 10),
			new TerraEffect(screenSurface, screenHeight, screenWidth, 10),
			new SyncEffect(screenSurface, screenHeight, screenWidth, 143),
		};

		//Main loop flag
		bool quit = false;

		//Event handler
		SDL_Event e;

		int startTime, currentTime;
		EffectTemplate* oldEffect = NULL;

		for (EffectTemplate*& effect : effects)
		{
			effect->start();
			
			// Transition to the new effect
			if (oldEffect != NULL)
			{
				//TransitionEffect* transition = new RowTransitionEffect(screenSurface, screenHeight, screenWidth, oldEffect, effect);
				TransitionEffect* transition = getNewTransition(screenSurface, screenHeight, screenWidth, oldEffect, effect);
				
				oldEffect->setSurface(auxSurface);

				transition->start();
				//While application is running
				while (!quit && !transition->isEnded())
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
					transition->update((float)Clock::getInstance().getDeltaTime());

					//Render
					render(transition);

					renderFPS();

					//Update the surface
					SDL_UpdateWindowSurface(window);
					Clock::getInstance().waitFrame();
				}

				delete transition;
			}

			// display effect
			startTime = Clock::getInstance().getCurrentTime();
			
			//While application is running
			while (!quit && !effect->isEnded())
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
				effect->updateFixed((float)Clock::getInstance().getDeltaTime());

				//Render
				render(effect);

				renderFPS();

				//Update the surface
				SDL_UpdateWindowSurface(window);
				Clock::getInstance().waitFrame();

				currentTime = Clock::getInstance().getCurrentTime();
				int countdown = (currentTime - startTime) / 1000;
				renderCountdown(countdown);
			}

			oldEffect = effect;
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

void drawText(SDL_Surface* screen, char* string, int size, int x, int y, SDL_Color fgC, SDL_Color bgC)
{
	// Remember to call TTF_Init(), TTF_Quit(), before/after using this function.
	TTF_Font* font = TTF_OpenFont("LEMONMILK-Regular.otf", size);
	if (!font) {
		std::cout << "[ERROR] TTF_OpenFont() Failed with: " << TTF_GetError() << std::endl;
		exit(2);
	}
	TTF_SetFontStyle(font, TTF_STYLE_BOLD);
	//SDL_Surface* textSurface = TTF_RenderText_Solid(font, string, fgC);     // aliased glyphs
	SDL_Surface* textSurface = TTF_RenderText_Shaded(font, string, fgC, bgC);   // anti-aliased glyphs
	SDL_Rect textLocation = { x, y, 0, 0 };
	SDL_BlitSurface(textSurface, NULL, screen, &textLocation);
	SDL_FreeSurface(textSurface);
	TTF_CloseFont(font);
}

void renderFPS()
{
	SDL_Color fg = { 0x00,0x00,0xff }, bg = { 0xff,0xff,0xff };      // Blue text on white background
	char fpsText[100];

	sprintf_s(fpsText, "FPS: %.2f", Clock::getInstance().getFPS());
	drawText(screenSurface, fpsText, FONT_SIZE, 0, 0, fg, bg);
}

void renderCountdown(int counter)
{
	SDL_Color fg = { 0x00,0x00,0xff }, bg = { 0xff,0xff,0xff };      // Blue text on white background
	char countdownText[100];

	// std::cout << "counter " << counter << std::endl;
	sprintf_s(countdownText, "%d", counter);
	drawText(screenSurface, countdownText, FONT_SIZE, 0, 2*FONT_SIZE, fg, bg);
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

	IMG_Init(IMG_INIT_PNG);

	//Create window
	window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);

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
	//SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0, 0, 0));

	effect->render();
}

void close() {

	SDL_FreeSurface(auxSurface);

	//Destroy window
	SDL_DestroyWindow(window);
	//Quit SDL subsystems
	SDL_Quit();
}
