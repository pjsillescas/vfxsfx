#pragma once

#include "EffectTemplate.h"
#include <SDL_mixer.h>

class SyncEffect : public EffectTemplate
{
private:
	Uint32 Backgroundcolor;
	SDL_Surface* flashTexture;

	Mix_Music* mySong;
	const float BPM_MUSIC = 128.f;
	const float MSEG_BPM = (60000.f / BPM_MUSIC);
	const int FLASH_MAX_TIME = 300;
	int flashtime;
	int MusicCurrentTime;
	int MusicCurrentTimeBeat;
	int MusicCurrentBeat;
	int MusicPreviousBeat;

public:

	SyncEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout);
	SyncEffect(SDL_Surface* surface, int screenHeight, int screenWidth, int timeout, const char* fileName);
	~SyncEffect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;

private:
	void renderFlash(SDL_Surface* surf, Uint8 alpha);
};

