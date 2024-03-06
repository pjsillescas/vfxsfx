#include "SyncEffect.h"


#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <cmath>

#include "Clock.h"

SyncEffect::SyncEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout) : EffectTemplate(surface, screenHeight, screenWidth, timeout)
{
}

void SyncEffect::init()
{
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	Mix_Init(MIX_INIT_OGG);
	mySong = Mix_LoadMUS("Blastculture-Gravitation.ogg");
	if (!mySong) {
		std::cout << "Error loading Music: " << Mix_GetError() << std::endl;
		exit(1);
	}
	Mix_PlayMusic(mySong, 0);
	flashtime = 0;
	MusicCurrentTime = 0;
	MusicCurrentTimeBeat = 0;
	MusicCurrentBeat = 0;
	MusicPreviousBeat = -1;
	Backgroundcolor = 0xFF000000 | ((rand() % 256) << 16) | ((rand() % 256) << 8) | (rand() % 256);
	// load the texture
	SDL_Surface* temp = IMG_Load("uoc.png");
	if (temp == NULL) {
		std::cout << "Image can be loaded! " << IMG_GetError();
		exit(1);
	}
	flashTexture = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_ARGB8888, 0);

}

void SyncEffect::update(float deltaTime)
{
	MusicCurrentTime += (int) deltaTime;
	MusicCurrentTimeBeat += (int) deltaTime;
	MusicPreviousBeat = MusicCurrentBeat;
	if (MusicCurrentTimeBeat >= MSEG_BPM) {
		MusicCurrentTimeBeat = 0;
		MusicCurrentBeat++;
		flashtime = FLASH_MAX_TIME;
	}
	if (flashtime > 0) {
		flashtime -= (int) deltaTime;
	}
	else {
		flashtime = 0;
	}
	if (!Mix_PlayingMusic()) {
		exit(0);
	}

}

void SyncEffect::render()
{
	std::cout << "Beat Time: " << MusicCurrentTimeBeat;
	std::cout << "\tBeat: " << MusicCurrentBeat;
	std::cout << "\tFlash Time: " << flashtime;
	std::cout << "\tMusic Time: " << MusicCurrentTime;
	std::cout << std::endl;

	if (MusicPreviousBeat != MusicCurrentBeat) {
		Backgroundcolor = 0xFF000000 | ((rand() % 256) << 16) | ((rand() % 256) << 8) | (rand() % 256);
	}
	SDL_FillRect(surface, NULL, Backgroundcolor);

	if (MusicCurrentTime > 9350) {
		if (flashtime > 0) {
			renderFlash(flashTexture, (Uint8)(255 * flashtime / (float)(FLASH_MAX_TIME)));
		}
	}
}

SyncEffect::~SyncEffect()
{
	Mix_HaltMusic();
	Mix_FreeMusic(mySong);
	Mix_Quit();
	Mix_CloseAudio();
}

void SyncEffect::renderFlash(SDL_Surface* surf, Uint8 alpha) {

	SDL_SetSurfaceAlphaMod(surf, alpha);
	SDL_BlitSurface(surf, NULL, surface, NULL);
	SDL_SetSurfaceAlphaMod(surf, 255);
}

