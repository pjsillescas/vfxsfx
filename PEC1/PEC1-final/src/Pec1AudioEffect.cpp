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
#include "Pec1FlashEffect.h"
#include "EffectWithTransition.h"
#include "TexturizationEffect.h"
#include "Pec1FlashEffect.h"

FlockingEffect *flockingEffect = NULL;
PlasmaPec1Effect* plasmaPec1Effect = NULL;
FractalPec1Effect* fractalPec1Effect = NULL;
DistortionPec1Effect* distortionPec1Effect = NULL;
SpyralEffect* spyralEffect = NULL;
BarsEffect* barsEffect = NULL;
WhirlpoolEffect* whirlpoolEffect = NULL;
BlackScreenEffect* blackScreenEffect = NULL;
Pec1FlashEffect* flashEffect = NULL;
TexturizationEffect* texturizationEffect = NULL;

Pec1AudioEffect::Pec1AudioEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, const char* fileName, bool showSection) : EffectTemplate(surface, screenHeight, screenWidth, timeout, title)
{
	this->showSection = showSection;
	
	float whirlpoolSpeed = screenWidth / 2.f * 1000.f / MSEG_BPM;

	float texSpeedX = 8.f / BPM_MUSIC;
	float texSpeedY = 4.f / BPM_MUSIC;

	float barSpeed = 1.f / BPM_MUSIC;

	// load the texture
	//flashTexture = loadImage(fileName);
	flockingEffect = new FlockingEffect(surface, screenHeight, screenWidth, timeout, "Flocking");
	plasmaPec1Effect = new PlasmaPec1Effect(surface, screenHeight, screenWidth, timeout, "Plasma Pec1");
	fractalPec1Effect = new FractalPec1Effect(surface, screenHeight, screenWidth, timeout, "Fractal Pec1");
	distortionPec1Effect = new DistortionPec1Effect(surface, screenHeight, screenWidth, timeout, "Distortion Pec1", "pec1-assets/orbit.jpg");
	spyralEffect = new SpyralEffect(surface, screenHeight, screenWidth, timeout, "Galaxy");
	barsEffect = new BarsEffect(surface, screenHeight, screenWidth, timeout, "Bars", "pec1-assets/castle.jpg", barSpeed);
	whirlpoolEffect = new WhirlpoolEffect(surface, screenHeight, screenWidth, timeout, "Whirlpool", "pec1-assets/gradient.png", 5, 0, whirlpoolSpeed, false);
	blackScreenEffect = new BlackScreenEffect(surface, screenHeight, screenWidth, timeout, "Black Screen");
	flashEffect = new Pec1FlashEffect(surface, screenHeight, screenWidth, timeout, "Black Screen", "pec1-assets/galaxy.png",FLASH_MAX_TIME);
	texturizationEffect = new TexturizationEffect(surface, screenHeight, screenWidth, 200, "Texturization", "pec1-assets/textureflower2.jpg", texSpeedX, texSpeedY);
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
	texturizationEffect->init();

	EffectTemplate* anacrusaEffect = blackScreenEffect;
	EffectTemplate* intro1Effect = plasmaPec1Effect;
	EffectTemplate* intro2Effect = whirlpoolEffect;
	EffectTemplate* percussionBeatEffect = blackScreenEffect;
	EffectTemplate* section1Effect = flockingEffect;
	EffectTemplate* section1BisEffect = spyralEffect;
	EffectTemplate* section2Effect = barsEffect;
	EffectTemplate* section3Effect = distortionPec1Effect;
	EffectTemplate* section3BisEffect = whirlpoolEffect;
	EffectTemplate* section4Effect = flashEffect;
	EffectTemplate* section4BisEffect = fractalPec1Effect;
	EffectTemplate* section1Effect2 = texturizationEffect;

	auxSurface = SDL_CreateRGBSurfaceWithFormat(0, screenWidth, screenHeight, 32, SDL_PIXELFORMAT_RGBA32);
	EffectWithTransition* effectWithTransition1 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition to Intro 1", anacrusaEffect, intro1Effect, auxSurface);
	EffectWithTransition* effectWithTransition2 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition to Intro 2", intro1Effect, intro2Effect, auxSurface);
	EffectWithTransition* effectWithTransition3 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition to Percussion beat", intro2Effect, percussionBeatEffect, auxSurface);
	EffectWithTransition* effectWithTransition4 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition to Section 1", percussionBeatEffect, section1Effect, auxSurface);
	EffectWithTransition* effectWithTransition5 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition to Section 1 bis", section1Effect, section1BisEffect, auxSurface);
	EffectWithTransition* effectWithTransition6 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition to Section 2", section1BisEffect, section2Effect, auxSurface);
	EffectWithTransition* effectWithTransition7 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition to Section 3", section2Effect, section3Effect, auxSurface);
	EffectWithTransition* effectWithTransition8 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition to Section 3 bis", section3Effect, section3BisEffect, auxSurface);
	EffectWithTransition* effectWithTransition9 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition to Percussion beat", section3BisEffect, percussionBeatEffect, auxSurface);
	EffectWithTransition* effectWithTransition10 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition to Section 1", percussionBeatEffect, section1Effect2, auxSurface);
	EffectWithTransition* effectWithTransition11 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition to Section 1 bis", section1Effect2, section1BisEffect, auxSurface);
	EffectWithTransition* effectWithTransition12 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition to Section 4", section1BisEffect, section4Effect, auxSurface);
	EffectWithTransition* effectWithTransition13 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition to Section 4 bis", section4Effect, section4BisEffect, auxSurface);
	EffectWithTransition* effectWithTransition14 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition to Percussion beat", section4BisEffect, percussionBeatEffect, auxSurface);
	EffectWithTransition* effectWithTransition15 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition to Section 1", percussionBeatEffect, section1Effect, auxSurface);
	EffectWithTransition* effectWithTransition16 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition to Section 1 bis", section1Effect, section1BisEffect, auxSurface);
	EffectWithTransition* effectWithTransition17 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition to Section 1", section1BisEffect, section1Effect2, auxSurface);
	EffectWithTransition* effectWithTransition18 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition to Section 1 bis", section1Effect2, intro1Effect, auxSurface);
	EffectWithTransition* effectWithTransition19 = new EffectWithTransition(surface, screenHeight, screenWidth, timeout, "Transition to Ending", intro1Effect, percussionBeatEffect, auxSurface);

	effects.insert(effects.begin() + 0, (EffectTemplate*)anacrusaEffect); // 0 Anacrusa
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
	
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	Mix_Init(MIX_INIT_OGG);
	mySong = Mix_LoadMUS("pec1-assets/road-to-nowhere_long.mp3");
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

	effects[currentSection]->render();

	if (showSection)
	{
		const int FONT_SIZE = 12;
		SDL_Color fg = { 0x00,0x00,0xff }, bg = { 0xff,0xff,0xff };      // Blue text on white background
		TextUtils::drawText(surface, sections[currentSection].name, FONT_SIZE, 10 * FONT_SIZE, 0, fg, bg);
	}
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
