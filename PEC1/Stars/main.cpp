#include <SDL.h>
#include <SDL_ttf.h>
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
#include "FractalEffect.h"
#include "TunnelEffect.h"
#include "RotozoomEffect.h"
#include "ParticleEffect.h"
#include "C3DEffect.h"
#include "TerraEffect.h"
#include "SyncEffect.h"

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

void renderFPS();

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
		//effect = new DistortionEffect(screenSurface, screenHeight, screenWidth);
		//effect = new BumpmapEffect(screenSurface, screenHeight, screenWidth);
		//effect = new FractalEffect(screenSurface, screenHeight, screenWidth);
		//effect = new TunnelEffect(screenSurface, screenHeight, screenWidth);
		//effect = new RotozoomEffect(screenSurface, screenHeight, screenWidth);
		//effect = new ParticleEffect(screenSurface, screenHeight, screenWidth);
		//effect = new C3DEffect(screenSurface, screenHeight, screenWidth);
		//effect = new TerraEffect(screenSurface, screenHeight, screenWidth);
		effect = new SyncEffect(screenSurface, screenHeight, screenWidth);
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
			effect->update((float) Clock::getInstance().getDeltaTime());

			//Render
			render(effect);

			renderFPS();

			//Update the surface
			SDL_UpdateWindowSurface(window);
			Clock::getInstance().waitFrame();

			std::cout << "fps: " << Clock::getInstance().getFPS() << std::endl;
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
	drawText(screenSurface, fpsText, 12, 0, 0, fg, bg);
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
