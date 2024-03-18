#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>

#include "Clock.h"

#include "TextUtils.h"
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
#include "ParticleEffectOptimized.h"
#include "FlowEffect.h"
#include "VortexEffect.h"
#include "Pec1AudioEffect.h"

const int FONT_SIZE = 12;
const int TIME_TO_DISPLAY_EFFECT = 10;

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
TTF_Font* getFont(int size);

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
			//new VortexEffect(screenSurface, screenHeight, screenWidth, 100, "Vortex"),
			//new FlowEffect(screenSurface, screenHeight, screenWidth, 100, "Flow"),
			/*
			new StarsEffect(screenSurface, screenHeight, screenWidth, 10, "Stars"),
			new StarsEffectChallenge2(screenSurface, screenHeight, screenWidth, 10, "Stars Reto 2"),
			
			new PlasmaEffect(screenSurface, screenHeight, screenWidth, 10, "Plasma"),
			new PlasmaEffectChallenge3(screenSurface, screenHeight, screenWidth, 10, "Plasma Reto 3"),
			new FireEffect(screenSurface, screenHeight, screenWidth, 10, "Fire"),
			new DistortionEffect(screenSurface, screenHeight, screenWidth, 10, "Distortion", uocFileName),
			new BumpmapEffect(screenSurface, screenHeight, screenWidth, 10, "Bumpmap", wallFileName, bumpFileName),
			new FractalEffect(screenSurface, screenHeight, screenWidth, 10, "Fractal"),
			new TunnelEffect(screenSurface, screenHeight, screenWidth, 10, "Tunnel"),
			new RotozoomEffect(screenSurface, screenHeight, screenWidth, 10, "Rotozoom"),
			(new ParticleEffect(screenSurface, screenHeight, screenWidth, 10, "Particles"))->setIsLateInit(true),
			
			//(new ParticleEffectOptimized(screenSurface, screenHeight, screenWidth, 10))->setIsLateInit(true),
			
			new C3DEffect(screenSurface, screenHeight, screenWidth, 10, "C3D"),
			new TerraEffect(screenSurface, screenHeight, screenWidth, 10, "Terra"),
			(new SyncEffect(screenSurface, screenHeight, screenWidth, 143, "Synch", uocFileName))->setIsLateInit(true),
			*/
			(new Pec1AudioEffect(screenSurface, screenHeight, screenWidth, 215, "PEC1 Audio", uocFileName))->setIsLateInit(true),
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
		render(effect);

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

/*
static void drawText(SDL_Surface* screen, char* string, int size, int x, int y, SDL_Color fgC, SDL_Color bgC)
{
	// Remember to call TTF_Init(), TTF_Quit(), before/after using this function.
	TTF_Font* font = getFont(size);
	TTF_SetFontStyle(font, TTF_STYLE_BOLD);
	//SDL_Surface* textSurface = TTF_RenderText_Solid(font, string, fgC);     // aliased glyphs
	SDL_Surface* textSurface = TTF_RenderText_Shaded(font, string, fgC, bgC);   // anti-aliased glyphs
	SDL_Rect textLocation = { x, y, 0, 0 };
	SDL_BlitSurface(textSurface, NULL, screen, &textLocation);
	SDL_FreeSurface(textSurface);
	TTF_CloseFont(font);
}
*/

void renderFPS()
{
	SDL_Color fg = { 0x00,0x00,0xff }, bg = { 0xff,0xff,0xff };      // Blue text on white background
	char fpsText[100];

	sprintf_s(fpsText, "FPS: %.2f", Clock::getInstance().getFPS());
	TextUtils::drawText(screenSurface, fpsText, FONT_SIZE, 0, 0, fg, bg);
}

void renderTitle(const char* title)
{
	SDL_Color fg = { 0x00,0x00,0xff }, bg = { 0xff,0xff,0xff };      // Blue text on white background
	char titleText[100];

	sprintf_s(titleText, "%s", title);
	TextUtils::drawText(screenSurface, titleText, FONT_SIZE, 0, 3*FONT_SIZE, fg, bg);
}

void renderTime(int time)
{
	if (time > 0)
	{
		SDL_Color fg = { 0x00,0x00,0xff }, bg = { 0xff,0xff,0xff };      // Blue text on white background
		char timeText[100];

		sprintf_s(timeText, "%d", time);
		TextUtils::drawText(screenSurface, timeText, FONT_SIZE, 0, 5 * FONT_SIZE, fg, bg);
	}
}
/*
TTF_Font* getFont(int size)
{
	// Remember to call TTF_Init(), TTF_Quit(), before/after using this function.
	TTF_Font* font = TTF_OpenFont("LEMONMILK-Regular.otf", size);
	if (!font) {
		std::cout << "[ERROR] TTF_OpenFont() Failed with: " << TTF_GetError() << std::endl;
		exit(2);
	}

	return font;
}
*/
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

	TextUtils::close();
	TTF_Quit();

	SDL_FreeSurface(auxSurface);

	//Destroy window
	SDL_DestroyWindow(window);
	//Quit SDL subsystems
	SDL_Quit();
}
