#include <SDL.h>
#include <SDL_image.h>
#include "SDL_mixer.h"
#include <iostream>
#include <string>

using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Sound Buffer size
#define CHUCK_SIZE 2048 // 2048 4096 

void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to set */
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp) {
	case 1:
		*p = pixel;
		break;

	case 2:
		*(Uint16 *)p = pixel;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			p[0] = (pixel >> 16) & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = pixel & 0xff;
		}
		else {
			p[0] = pixel & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = (pixel >> 16) & 0xff;
		}
		break;

	case 4:
		*(Uint32 *)p = pixel;
		break;
	}
}

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gDrawSurface = NULL;

// make a passthru processor function that draw pixels
void miEfecto(void *udata, Uint8 *stream, int len)
{
	short *data;
	// len is 16 bits & Stereo. buffer are 4096 bytes, then we need   
	//    4096 * 2 (int 16 bits) * 2 stereo Channels.
	//    In this exemple we only use one channel and copy from stream to data
	data = (short*)malloc(len / 4);
	int pos = 0;
	for (int i = 0; i < len ; i+=8) {
		short *source = (short*)(stream + i);
		*(data+pos) = *(source); // Bytes to shorts
		pos++;
	}

	SDL_FillRect(gDrawSurface, NULL, 0x00000000);
	float lastHeight = SCREEN_HEIGHT / 2;
	for (int i = 0;  i < SCREEN_WIDTH; i++) {
		float height = (float)(SCREEN_HEIGHT * (*(data + i*((CHUCK_SIZE /2)/SCREEN_WIDTH)))/(65535));
		do {
			int OnePixel = (int)lastHeight + (SCREEN_HEIGHT / 2);
			//cout << OnePixel  << " ";
			putpixel(gDrawSurface, i, OnePixel, 0xFF00FF00);
			if (lastHeight > height)
				lastHeight --;
			if (lastHeight < height)
				lastHeight ++;
		} while (lastHeight != height);
	}
	free(data);
}


void init()
{



}

void render() {
	// Clear Background
	SDL_FillRect(gScreenSurface, NULL, 0xFF00007F);

	//Apply pixels image draw from sound's data
	SDL_BlitSurface(gDrawSurface, NULL, gScreenSurface, NULL);
	//Update the surface
	SDL_UpdateWindowSurface(gWindow);
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	//Initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING);
	//Create window
	gWindow = SDL_CreateWindow("UOC Reto 3 - Sonido", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	//Get window surface
	gScreenSurface = SDL_GetWindowSurface(gWindow);

	// Create an empty surface
	gDrawSurface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	//Main loop flag
	bool quit = false;

	//Event handler
	SDL_Event e;

	Mix_Chunk *song;

	Mix_Init(MIX_INIT_OGG);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, CHUCK_SIZE);
	//Load song.
	song = Mix_LoadWAV("Assets/music.ogg");

	// register noEffect as a postmix processor
	Mix_SetPostMix(miEfecto, NULL);

	// play song
	int songChannel = Mix_PlayChannel(-1, song, 0);
	
	//While application is running
	while (!quit)
	{
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
					quit = true;
				}
			}
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
		}

		//Render quad
		render();
	}

	Mix_HaltChannel(songChannel);
	Mix_SetPostMix(NULL, NULL);

	//Free resources and close SDL
	//Free surface to paint wave sound
	SDL_FreeSurface(gDrawSurface);
	gDrawSurface = NULL;

	Mix_FreeChunk(song);

	//Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	Mix_CloseAudio();
	SDL_Quit();

	return 0;
}