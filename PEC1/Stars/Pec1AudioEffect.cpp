#include "Pec1AudioEffect.h"


#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <cmath>
#include <vector>

#include "Clock.h"
#include "TextUtils.h"
#include "FlockingEffect.h"
#include "PlasmaPec1Effect.h"
#include "FractalPec1Effect.h"
#include "DistortionPec1Effect.h"
#include "SpyralEffect.h"
#include "BarsEffect.h"
#include "WhirlpoolEffect.h"
#include "BlackScreenEffect.h"
#include "FlashEffect.h"
#include "EffectWithTransition.h"

FlockingEffect *flockingEffect = NULL;
PlasmaPec1Effect* plasmaPec1Effect = NULL;
FractalPec1Effect* fractalPec1Effect = NULL;
DistortionPec1Effect* distortionPec1Effect = NULL;
SpyralEffect* spyralEffect = NULL;
BarsEffect* barsEffect = NULL;
WhirlpoolEffect* whirlpoolEffect = NULL;
BlackScreenEffect* blackScreenEffect = NULL;
FlashEffect* flashEffect = NULL;

Pec1AudioEffect::Pec1AudioEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title) : EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
	// load the texture
	flashTexture = loadImage("uoc.png");

	flockingEffect = new FlockingEffect(surface,screenHeight,screenWidth,timeout, "flocking");
}

Pec1AudioEffect::Pec1AudioEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, const char* fileName) : EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
	//float whirlpoolSpeed = 500.f;
	
	float whirlpoolSpeed = screenWidth / 2.f * 1000.f / MSEG_BPM;
	
	// load the texture
	flashTexture = loadImage(fileName);
	flockingEffect = new FlockingEffect(surface, screenHeight, screenWidth, timeout, "Flocking");
	plasmaPec1Effect = new PlasmaPec1Effect(surface, screenHeight, screenWidth, timeout, "Plasma Pec1");
	fractalPec1Effect = new FractalPec1Effect(surface, screenHeight, screenWidth, timeout, "Fractal Pec1");
	distortionPec1Effect = new DistortionPec1Effect(surface, screenHeight, screenWidth, timeout, "Distortion Pec1", "uoc.png");
	spyralEffect = new SpyralEffect(surface, screenHeight, screenWidth, timeout, "Galaxy");
	barsEffect = new BarsEffect(surface, screenHeight, screenWidth, timeout, "Bars");
	whirlpoolEffect = new WhirlpoolEffect(surface, screenHeight, screenWidth, timeout, "Whirlpool", 5, 0, whirlpoolSpeed);
	blackScreenEffect = new BlackScreenEffect(surface, screenHeight, screenWidth, timeout, "Black Screen");
	flashEffect = new FlashEffect(surface, screenHeight, screenWidth, timeout, "Black Screen", "uoc.png",FLASH_MAX_TIME);
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
	whirlpoolEffect->init();
	blackScreenEffect->init();
	flashEffect->init();
	/*
	effects.insert(effects.begin() + 0, (EffectTemplate*) blackScreenEffect); // 0 Anacrusa
	effects.insert(effects.begin() + 1, (EffectTemplate*)plasmaPec1Effect); // 1 Intro 1
	effects.insert(effects.begin() + 2, (EffectTemplate*)flashEffect); // 2 Intro 2
	effects.insert(effects.begin() + 3, (EffectTemplate*) blackScreenEffect); // 3 Percussion beat
	effects.insert(effects.begin() + 4, (EffectTemplate*)flockingEffect); // 4 Section 1
	effects.insert(effects.begin() + 5, (EffectTemplate*)fractalPec1Effect); // 5 Section 1 bis
	effects.insert(effects.begin() + 6, (EffectTemplate*) barsEffect); //6 Section 2
	effects.insert(effects.begin() + 7, (EffectTemplate*) distortionPec1Effect); // 7 Section 3
	effects.insert(effects.begin() + 8, (EffectTemplate*) flashEffect); // 8 Section 3 bis
	effects.insert(effects.begin() + 9, (EffectTemplate*)blackScreenEffect); // 9 Percussion beat
	effects.insert(effects.begin() + 10, (EffectTemplate*) flockingEffect); // 10 Section 1
	effects.insert(effects.begin() + 11, (EffectTemplate*) fractalPec1Effect); // 11 Section 1 bis
	effects.insert(effects.begin() + 12, (EffectTemplate*) spyralEffect); // 12 Section 4
	effects.insert(effects.begin() + 13, (EffectTemplate*) whirlpoolEffect); // 13 Section 4 bis
	effects.insert(effects.begin() + 14, (EffectTemplate*)blackScreenEffect); // 14 Percussion beat
	effects.insert(effects.begin() + 15, (EffectTemplate*) flockingEffect); // 15 Section 1
	effects.insert(effects.begin() + 16, (EffectTemplate*) fractalPec1Effect); // 16 Section 1 bis
	effects.insert(effects.begin() + 17, (EffectTemplate*) flockingEffect); // 17 Section 1
	effects.insert(effects.begin() + 18, (EffectTemplate*) plasmaPec1Effect); // 18 Section 1 bis
	effects.insert(effects.begin() + 19, (EffectTemplate*) blackScreenEffect); // 19 Ending
	*/

	auxSurface = SDL_CreateRGBSurfaceWithFormat(0, screenWidth, screenHeight, 32, SDL_PIXELFORMAT_RGBA32);
	EffectWithTransition* effectWithTransition1 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition 1", blackScreenEffect, plasmaPec1Effect, auxSurface);
	EffectWithTransition* effectWithTransition2 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition 2", plasmaPec1Effect, flashEffect, auxSurface);
	EffectWithTransition* effectWithTransition3 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition 3", flashEffect, blackScreenEffect, auxSurface);
	EffectWithTransition* effectWithTransition4 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition 4", blackScreenEffect, flockingEffect, auxSurface);
	EffectWithTransition* effectWithTransition5 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition 5", flockingEffect, fractalPec1Effect, auxSurface);
	EffectWithTransition* effectWithTransition6 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition 6", fractalPec1Effect, barsEffect, auxSurface);
	EffectWithTransition* effectWithTransition7 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition 7", barsEffect, distortionPec1Effect, auxSurface);
	EffectWithTransition* effectWithTransition8 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition 8", distortionPec1Effect, flashEffect, auxSurface);
	EffectWithTransition* effectWithTransition9 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition 9", flashEffect, blackScreenEffect, auxSurface);
	EffectWithTransition* effectWithTransition10 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition 10", blackScreenEffect, flockingEffect, auxSurface);
	EffectWithTransition* effectWithTransition11 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition 11", flockingEffect, fractalPec1Effect, auxSurface);
	EffectWithTransition* effectWithTransition12 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition 12", fractalPec1Effect, spyralEffect, auxSurface);
	EffectWithTransition* effectWithTransition13 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition 13", spyralEffect, whirlpoolEffect, auxSurface);
	EffectWithTransition* effectWithTransition14 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition 14", whirlpoolEffect, blackScreenEffect, auxSurface);
	EffectWithTransition* effectWithTransition15 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition 15", blackScreenEffect, flockingEffect, auxSurface);
	EffectWithTransition* effectWithTransition16 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition 16", flockingEffect, fractalPec1Effect, auxSurface);
	EffectWithTransition* effectWithTransition17 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition 17", fractalPec1Effect, flockingEffect, auxSurface);
	EffectWithTransition* effectWithTransition18 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition 18", flockingEffect, plasmaPec1Effect, auxSurface);
	EffectWithTransition* effectWithTransition19 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition 19", plasmaPec1Effect, blackScreenEffect, auxSurface);
	/*
	effectWithTransition1->init();
	effectWithTransition2->init();
	effectWithTransition3->init();
	effectWithTransition1->init();
	effectWithTransition1->init();
	effectWithTransition1->init();
	effectWithTransition1->init();
	effectWithTransition1->init();
	effectWithTransition1->init();
	effectWithTransition1->init();
	*/
	effects.insert(effects.begin() + 0, (EffectTemplate*)blackScreenEffect); // 0 Anacrusa
	effects.insert(effects.begin() + 1, (EffectTemplate*)effectWithTransition1); // 1 Intro 1
	effects.insert(effects.begin() + 2, (EffectTemplate*)effectWithTransition2); // 2 Intro 2
	effects.insert(effects.begin() + 3, (EffectTemplate*)effectWithTransition3); // 3 Percussion beat
	effects.insert(effects.begin() + 4, (EffectTemplate*)effectWithTransition4); // 4 Section 1
	effects.insert(effects.begin() + 5, (EffectTemplate*)effectWithTransition5); // 5 Section 1 bis
	effects.insert(effects.begin() + 6, (EffectTemplate*)effectWithTransition6); //6 Section 2
	effects.insert(effects.begin() + 7, (EffectTemplate*)effectWithTransition7); // 7 Section 3
	effects.insert(effects.begin() + 8, (EffectTemplate*)effectWithTransition8); // 8 Section 3 bis
	effects.insert(effects.begin() + 9, (EffectTemplate*)effectWithTransition9); // 9 Percussion beat
	effects.insert(effects.begin() + 10, (EffectTemplate*)effectWithTransition10); // 10 Section 1
	effects.insert(effects.begin() + 11, (EffectTemplate*)effectWithTransition11); // 11 Section 1 bis
	effects.insert(effects.begin() + 12, (EffectTemplate*)effectWithTransition12); // 12 Section 4
	effects.insert(effects.begin() + 13, (EffectTemplate*)effectWithTransition13); // 13 Section 4 bis
	effects.insert(effects.begin() + 14, (EffectTemplate*)effectWithTransition14); // 14 Percussion beat
	effects.insert(effects.begin() + 15, (EffectTemplate*)effectWithTransition15); // 15 Section 1
	effects.insert(effects.begin() + 16, (EffectTemplate*)effectWithTransition16); // 16 Section 1 bis
	effects.insert(effects.begin() + 17, (EffectTemplate*)effectWithTransition17); // 17 Section 1
	effects.insert(effects.begin() + 18, (EffectTemplate*)effectWithTransition18); // 18 Section 1 bis
	effects.insert(effects.begin() + 19, (EffectTemplate*)effectWithTransition19); // 19 Ending

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
	
	/*
	switch (currentSection)
	{
	case 0: // Anacrusa
	case 3: // Percussion beat
	case 9: // Percussion beat
	case 14: // Percussion beat
		blackScreenEffect->update(deltaTime);
		break;
	case 1: // Intro 1
		plasmaPec1Effect->update(deltaTime);
		break;
	case 2: //Intro 2
		flashEffect->update(deltaTime);
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
		flashEffect->update(deltaTime);
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
		whirlpoolEffect->update(deltaTime);
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
		blackScreenEffect->update(deltaTime);
		break;
	}
	*/
	effects[currentSection]->update(deltaTime);

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
	switch (currentSection)
	{
	case 0: // Anacrusa
	case 3: // Percussion beat
	case 9: // Percussion beat
	case 14: // Percussion beat
		blackScreenEffect->render();
		break;
	case 1: // Intro 1
		plasmaPec1Effect->render();
		break;
	case 2: //Intro 2
		flashEffect->render();
		break;
	case 4: // Section 1
		flockingEffect->render();
		break;
	case 5: // Section 1 bis
		fractalPec1Effect->render();
		break;
	case 6: //Section 2
		barsEffect->render();
		break;
	case 7: // Section 3
		distortionPec1Effect->render();
		break;
	case 8: // Section 3 bis
		flashEffect->render();
		break;
	case 10: // Section 1
		flockingEffect->render();
		break;
	case 11: // Section 1 bis
		fractalPec1Effect->render();
		break;
	case 12: // Section 4
		spyralEffect->render();
		break;
	case 13: // Section 4 bis
		whirlpoolEffect->render();
		break;
	case 15: // Section 1
		flockingEffect->render();
		break;
	case 16: // Section 1 bis
		fractalPec1Effect->render();
		break;
	case 17: // Section 1
		flockingEffect->render();
		break;
	case 18: // Section 1 bis
		plasmaPec1Effect->render();
		break;
	case 19: // Ending
		blackScreenEffect->render();
		break;
	}
	*/
	effects[currentSection]->render();

	const int FONT_SIZE = 12;
	SDL_Color fg = { 0x00,0x00,0xff }, bg = { 0xff,0xff,0xff };      // Blue text on white background
	TextUtils::drawText(surface, sections[currentSection].name, FONT_SIZE, 10 * FONT_SIZE, 0, fg, bg);
}

Pec1AudioEffect::~Pec1AudioEffect()
{
	delete effectWithTransition1;
	delete effectWithTransition2;
	delete effectWithTransition3;
	delete effectWithTransition4;
	delete effectWithTransition5;
	delete effectWithTransition6;
	delete effectWithTransition7;
	delete effectWithTransition8;
	delete effectWithTransition9;
	delete effectWithTransition10;
	delete effectWithTransition11;
	delete effectWithTransition12;
	delete effectWithTransition13;
	delete effectWithTransition14;
	delete effectWithTransition15;
	delete effectWithTransition16;
	delete effectWithTransition17;
	delete effectWithTransition18;
	delete effectWithTransition19;

	
	delete flashEffect;
	delete blackScreenEffect;
	delete whirlpoolEffect;
	delete barsEffect;
	delete spyralEffect;
	delete flockingEffect;
	delete fractalPec1Effect;
	delete plasmaPec1Effect;
	delete distortionPec1Effect;

	delete[] sections;
	SDL_FreeSurface(blackSurface);
	SDL_FreeSurface(auxSurface);

	Mix_HaltMusic();
	Mix_FreeMusic(mySong);
	Mix_Quit();
	Mix_CloseAudio();
}

/*
void Pec1AudioEffect::renderFlash(SDL_Surface* surf, Uint8 alpha)
{
	SDL_SetSurfaceAlphaMod(surf, alpha);
	SDL_BlitSurface(surf, NULL, surface, NULL);
	SDL_SetSurfaceAlphaMod(surf, 255);
}
*/

