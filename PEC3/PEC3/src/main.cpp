#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>

#include "utils/Clock.h"

#include "utils/TextUtils.h"
#include "effects/EffectCave.h"


const int FONT_SIZE = 12;

const bool SHOW_FPS = true;
const bool SHOW_TITLE = false;
const bool SHOW_TIME = false;


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

int main(int argc, char* args[])
{
	int screenWidth = 640;
	int screenHeight = 480;

	//Start up SDL and create window
	if (!initSDL(screenWidth, screenHeight))
	{
		std::cout << "Failed to initialize!\n";
		return 1;
	}
	else
	{
		short board[10][10] = {
			1,1,1,1,1,1,1,1,1,1,
			1,0,0,0,1,0,0,0,0,1,
			1,0,0,0,1,0,0,0,0,1,
			1,0,1,0,1,0,0,0,0,1,
			1,0,0,0,0,3,0,1,0,1,
			1,0,0,0,0,0,0,1,0,1,
			1,0,0,1,0,0,0,1,0,1,
			1,0,2,1,0,0,0,1,0,1,
			1,0,0,1,0,0,0,1,4,1,
			1,1,1,1,1,1,1,1,1,1,
		};

		Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
		Mix_Init(MIX_INIT_OGG);
		/*
		Mix_Music* mySong;
		mySong = Mix_LoadMUS("pec1-assets/road-to-nowhere_long.mp3");
		if (!mySong)
		{
			std::cout << "Error loading Music: " << Mix_GetError() << std::endl;
			exit(1);
		}
		*/

		auxSurface = SDL_CreateRGBSurfaceWithFormat(0, screenWidth, screenHeight, 32, SDL_PIXELFORMAT_RGBA32);
		std::vector<EffectTemplate*> effects{
			new EffectCave(screenSurface, screenHeight, screenWidth, 0, "Cave", board)
		};
		
		//Main loop flag
		bool quit = false;

		for (EffectTemplate*& effect : effects)
		{
			effect->init();
		}
		
		//Event handler
		SDL_Event e;

		for (EffectTemplate*& effect : effects)
		{
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
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
			{
				quit = true;
			}

			if (e.type == SDL_KEYDOWN)
			{
				effect->onKeyPressed(e.key.keysym.scancode);
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
	TextUtils::drawText(screenSurface, titleText, FONT_SIZE, 0, 3 * FONT_SIZE, fg, bg);
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
	window = SDL_CreateWindow("PEC 3 - Cave of the Damned (Damn Cave)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);

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

	Mix_CloseAudio();
	//Destroy window
	SDL_DestroyWindow(window);
	//Quit SDL subsystems
	SDL_Quit();
}
