#pragma once

#include "EffectTemplate.h"
#include <SDL_mixer.h>

struct Section;

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

	int flashtime;
	int MusicCurrentTime;
	int MusicCurrentTimeBeat;
	int MusicCurrentBeat;
	int MusicPreviousBeat;

	int numBeats;
	Section* sections;
	int currentSection;
	int currentBeatInSection;

public:

	Pec1AudioEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title);
	Pec1AudioEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, std::string title, const char* fileName);
	~Pec1AudioEffect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;
	virtual void onBeat();

private:
	void renderFlash(SDL_Surface* surf, Uint8 alpha);
	void changeBackgroundColor();
	void fadeOut();

};