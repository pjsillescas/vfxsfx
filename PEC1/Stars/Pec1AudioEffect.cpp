#include "Pec1AudioEffect.h"


#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <cmath>

#include "Clock.h"
#include "TextUtils.h"
#include "FlockingEffect.h"
#include "PlasmaPec1Effect.h"
#include "FractalPec1Effect.h"
#include "DistortionPec1Effect.h"
#include "SpyralEffect.h"
#include "BarsEffect.h"

FlockingEffect *flockingEffect = NULL;
PlasmaPec1Effect* plasmaPec1Effect = NULL;
FractalPec1Effect* fractalPec1Effect = NULL;
DistortionPec1Effect* distortionPec1Effect = NULL;
SpyralEffect* spyralEffect = NULL;
BarsEffect* barsEffect = NULL;

Pec1AudioEffect::Pec1AudioEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title) : EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
	// load the texture
	flashTexture = loadImage("uoc.png");

	flockingEffect = new FlockingEffect(surface,screenHeight,screenWidth,timeout, "flocking");
}

Pec1AudioEffect::Pec1AudioEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, const char* fileName) : EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
	// load the texture
	flashTexture = loadImage(fileName);
	flockingEffect = new FlockingEffect(surface, screenHeight, screenWidth, timeout, "Flocking");
	plasmaPec1Effect = new PlasmaPec1Effect(surface, screenHeight, screenWidth, timeout, "Plasma Pec1");
	fractalPec1Effect = new FractalPec1Effect(surface, screenHeight, screenWidth, timeout, "Fractal Pec1");
	distortionPec1Effect = new DistortionPec1Effect(surface, screenHeight, screenWidth, timeout, "Distortion Pec1", "uoc.png");
	spyralEffect = new SpyralEffect(surface, screenHeight, screenWidth, timeout, "Galaxy");
	barsEffect = new BarsEffect(surface, screenHeight, screenWidth, timeout, "Bars");
}

Uint32 getRandomColor()
{
	return 0xFF000000 | ((rand() % 256) << 16) | ((rand() % 256) << 8) | (rand() % 256);
}

const int NUM_SECTIONS = 20;
struct Section
{
	int beats;
	const char* name;
};

SDL_Surface* blackSurface;

void Pec1AudioEffect::init()
{
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	Mix_Init(MIX_INIT_OGG);
	mySong = Mix_LoadMUS("road-to-nowhere_long.mp3");
	if (!mySong)
	{
		std::cout << "Error loading Music: " << Mix_GetError() << std::endl;
		exit(1);
	}
	Mix_PlayMusic(mySong, 0);
	flashtime = 0;
	MusicCurrentTime = 0;
	MusicCurrentTimeBeat = 0;
	MusicCurrentBeat = 0;
	MusicPreviousBeat = -1;
	Backgroundcolor = getRandomColor();

	blackSurface = SDL_CreateRGBSurfaceWithFormat(0,screenWidth,screenHeight,32, SDL_PIXELFORMAT_RGBA32);
	SDL_FillRect(blackSurface, NULL, SDL_MapRGB(blackSurface->format, 0, 0, 0));

	numBeats = 0;
	sections = new Section[NUM_SECTIONS]
	{
		{2,"Anacrusa"},
		{4 * 8, "Intro 1"},
		{4 * 8 - 1, "Intro 2"},
		{4, "Percussion beat"},
		{4 * 8,"Section 1"},
		{4 * 8 - 1,"Section 1 bis"},
		{4 * 8 - 1,"Section 2"},
		{4 * 8,"Section 3"},
		{4 * 8 - 1,"Section 3 bis"},
		{4, "Percussion beat"},
		{4 * 8,"Section 1"},
		{4 * 8 - 1,"Section 1 bis"},
		{4 * 8,"Section 4"},
		{4 * 8 - 1,"Section 4 bis"},
		{4, "Percussion beat"},
		{4 * 8,"Section 1"},
		{4 * 8 - 1,"Section 1 bis"},
		{4 * 8 - 1,"Section 1"},
		{4 * 8,"Section 1 bis 2"},
		{4 * 8,"Ending"},
	};
	currentSection = 0;
	currentBeatInSection = 0;

	flockingEffect->init();
	plasmaPec1Effect->init();
	fractalPec1Effect->init();
	distortionPec1Effect->init();
	spyralEffect->init();
	barsEffect->init();
}

void Pec1AudioEffect::update(float deltaTime)
{
	MusicCurrentTime += (int)deltaTime;
	MusicCurrentTimeBeat += (int)deltaTime;
	MusicPreviousBeat = MusicCurrentBeat;
	if (MusicCurrentTimeBeat >= MSEG_BPM)
	{
		MusicCurrentTimeBeat = 0;
		MusicCurrentBeat++;
		flashtime = FLASH_MAX_TIME;
		onBeat();
	}

	if (flashtime > 0)
	{
		flashtime -= (int)deltaTime;
	}
	else
	{
		flashtime = 0;
	}
	
	switch (currentSection)
	{
	case 0: // Anacrusa
	case 3: // Percussion beat
	case 9: // Percussion beat
	case 14: // Percussion beat
		break;
	case 1: // Intro 1
		plasmaPec1Effect->update(deltaTime);
		break;
	case 2: //Intro 2
		break;
	case 4: // Section 1
		flockingEffect->update(deltaTime);
		break;
	case 5: // Section 1 bis
		fractalPec1Effect->update(deltaTime);
		break;
	case 6: //Section 2
		barsEffect->update(deltaTime);
		break;
	case 7: // Section 3
		distortionPec1Effect->update(deltaTime);
		break;
	case 8: // Section 3 bis
		break;
	case 10: // Section 1
		flockingEffect->update(deltaTime);
		break;
	case 11: // Section 1 bis
		fractalPec1Effect->update(deltaTime);
		break;
	case 12: // Section 4
		spyralEffect->update(deltaTime);
		break;
	case 13: // Section 4 bis
		break;
	case 15: // Section 1
		flockingEffect->update(deltaTime);
		break;
	case 16: // Section 1 bis
		fractalPec1Effect->update(deltaTime);
		break;
	case 17: // Section 1
		flockingEffect->update(deltaTime);
		break;
	case 18: // Section 1 bis
		plasmaPec1Effect->update(deltaTime);
		break;
	case 19: // Ending
		break;
	}

	if (!Mix_PlayingMusic())
	{
		//exit(0);
		setIsEnded(true);
	}

}

void Pec1AudioEffect::onBeat()
{
	numBeats++;
	currentBeatInSection++;
	if (currentBeatInSection >= sections[currentSection].beats)
	{
		currentBeatInSection = 0;
		currentSection++;
	}
}

//const int PHASE2_MSEC = 128 * 73;

void Pec1AudioEffect::changeBackgroundColor()
{
	if (MusicPreviousBeat != MusicCurrentBeat)
	{
		Backgroundcolor = getRandomColor();
	}

	SDL_FillRect(surface, NULL, Backgroundcolor);

}

void Pec1AudioEffect::render()
{
	/*
	std::cout << "Beat Time: " << MusicCurrentTimeBeat;
	std::cout << "\tBeat: " << MusicCurrentBeat;
	std::cout << "\tFlash Time: " << flashtime;
	std::cout << "\tMusic Time: " << MusicCurrentTime;
	std::cout << std::endl;
	*/

	/*
	if (currentSection > 0)
	{
		changeBackgroundColor();
	}

	if (currentSection == 2 || currentSection == 4)
	{
		if (flashtime > 0)
		{
			renderFlash(flashTexture, (Uint8)(255 * (1.f - flashtime / (float)(FLASH_MAX_TIME))));
		}
	}
	*/

	switch (currentSection)
	{
	case 0: // Anacrusa
	case 3: // Percussion beat
	case 9: // Percussion beat
	case 14: // Percussion beat
		SDL_FillRect(surface, NULL, 0);
		break;
	case 1: // Intro 1
		plasmaPec1Effect->render();
		//changeBackgroundColor();
		break;
	case 2: //Intro 2
		changeBackgroundColor();
		if (flashtime > 0)
		{
			renderFlash(flashTexture, (Uint8)(255 * (1.f - flashtime / (float)(FLASH_MAX_TIME))));
		}
		break;
	case 4: // Section 1
		//changeBackgroundColor();
		flockingEffect->render();
		break;
	case 5: // Section 1 bis
		fractalPec1Effect->render();
		/*
		changeBackgroundColor();
		if (flashtime > 0)
		{
			renderFlash(flashTexture, (Uint8)(255 * (1.f - flashtime / (float)(FLASH_MAX_TIME))));
		}
		*/
		break;
	case 6: //Section 2
		//changeBackgroundColor();
		barsEffect->render();
		break;
	case 7: // Section 3
		distortionPec1Effect->render();

		break;
	case 8: // Section 3 bis
		//changeBackgroundColor();
		changeBackgroundColor();
		if (flashtime > 0)
		{
			renderFlash(flashTexture, (Uint8)(255 * (1.f - flashtime / (float)(FLASH_MAX_TIME))));
		}

		break;
	case 10: // Section 1
		//changeBackgroundColor();
		flockingEffect->render();
		break;
	case 11: // Section 1 bis
		fractalPec1Effect->render();
		/*
		changeBackgroundColor();
		if (flashtime > 0)
		{
			renderFlash(flashTexture, (Uint8)(255 * (1.f - flashtime / (float)(FLASH_MAX_TIME))));
		}
		*/
		break;
	case 12: // Section 4
		//changeBackgroundColor();
		spyralEffect->render();
		break;
	case 13: // Section 4 bis
		changeBackgroundColor();
		if (flashtime > 0)
		{
			renderFlash(flashTexture, (Uint8)(255 * (1.f - flashtime / (float)(FLASH_MAX_TIME))));
		}
		break;
	case 15: // Section 1
		//changeBackgroundColor();
		flockingEffect->render();
		break;
	case 16: // Section 1 bis
		fractalPec1Effect->render();
		/*
		changeBackgroundColor();
		if (flashtime > 0)
		{
			renderFlash(flashTexture, (Uint8)(255 * (1.f - flashtime / (float)(FLASH_MAX_TIME))));
		}
		*/
		break;
	case 17: // Section 1
		//changeBackgroundColor();
		flockingEffect->render();
		break;
	case 18: // Section 1 bis
		plasmaPec1Effect->render();
		/*
		changeBackgroundColor();
		if (flashtime > 0)
		{
			renderFlash(flashTexture, (Uint8)(255 * (1.f - flashtime / (float)(FLASH_MAX_TIME))));
		}
		*/
		break;
	case 19: // Ending
		fadeOut();
		break;
	}

	const int FONT_SIZE = 12;
	SDL_Color fg = { 0x00,0x00,0xff }, bg = { 0xff,0xff,0xff };      // Blue text on white background
	TextUtils::drawText(surface, sections[currentSection].name, FONT_SIZE, 10 * FONT_SIZE, 0, fg, bg);
}

float fadeOutTime = -100000.f;

void Pec1AudioEffect::fadeOut()
{
	if (fadeOutTime < -90000.f)
	{
		fadeOutTime = FADEOUT_TIME;
	}
	
	if (fadeOutTime >= 0)
	{
		fadeOutTime -= Clock::getInstance().getDeltaTime();
		int alpha = (Uint8)255 * (1. - fadeOutTime / FADEOUT_TIME);
		//std::cout << fadeOutTime << " -- " << FADEOUT_TIME << " => " << alpha << std::endl;
		renderFlash(blackSurface, 255);
	}
}

Pec1AudioEffect::~Pec1AudioEffect()
{
	delete flockingEffect;
	delete fractalPec1Effect;
	delete plasmaPec1Effect;
	delete distortionPec1Effect;

	delete[] sections;
	SDL_FreeSurface(blackSurface);

	Mix_HaltMusic();
	Mix_FreeMusic(mySong);
	Mix_Quit();
	Mix_CloseAudio();
}

void Pec1AudioEffect::renderFlash(SDL_Surface* surf, Uint8 alpha)
{
	SDL_SetSurfaceAlphaMod(surf, alpha);
	SDL_BlitSurface(surf, NULL, surface, NULL);
	SDL_SetSurfaceAlphaMod(surf, 255);
}

