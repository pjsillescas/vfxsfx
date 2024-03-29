#pragma once

#include "EffectTemplate.h"
#include <SDL_mixer.h>
#include <vector>

struct Section;
class EffectWithTransition;

class Pec1AudioEffect : public EffectTemplate
{
private:
	Uint32 Backgroundcolor;
	SDL_Surface* flashTexture;

	Mix_Music* mySong;
	const float BPM_MUSIC = 140.f;
	const float MSEG_BPM = (60000.f / BPM_MUSIC);
	const int FLASH_MAX_TIME = MSEG_BPM; // 300;
	const float FADEOUT_TIME = 2.f * MSEG_BPM * 4.f;

	bool showSection;
	int flashtime;
	int MusicCurrentTime;
	int MusicCurrentTimeBeat;
	int MusicCurrentBeat;
	int MusicPreviousBeat;

	int numBeats;
	Section* sections;
	int currentSection;
	int currentBeatInSection;
	std::vector<EffectTemplate*> effects;
	SDL_Surface* auxSurface;

	EffectWithTransition* effectWithTransition1;
	EffectWithTransition* effectWithTransition2;
	EffectWithTransition* effectWithTransition3;
	EffectWithTransition* effectWithTransition4;
	EffectWithTransition* effectWithTransition5;
	EffectWithTransition* effectWithTransition6;
	EffectWithTransition* effectWithTransition7;
	EffectWithTransition* effectWithTransition8;
	EffectWithTransition* effectWithTransition9;
	EffectWithTransition* effectWithTransition10;
	EffectWithTransition* effectWithTransition11;
	EffectWithTransition* effectWithTransition12;
	EffectWithTransition* effectWithTransition13;
	EffectWithTransition* effectWithTransition14;
	EffectWithTransition* effectWithTransition15;
	EffectWithTransition* effectWithTransition16;
	EffectWithTransition* effectWithTransition17;
	EffectWithTransition* effectWithTransition18;
	EffectWithTransition* effectWithTransition19;


public:

	Pec1AudioEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, const char* fileName, bool showSection);
	~Pec1AudioEffect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;
	virtual void onBeat();

};