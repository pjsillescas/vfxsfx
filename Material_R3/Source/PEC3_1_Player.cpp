#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 854;
const int SCREEN_HEIGHT = 480;

#define PI 3.14159265

//GLOBAL
SDL_Renderer* gMyRenderer = NULL;

void renderTexture(SDL_Texture* origin, SDL_Rect* _rect, int X, int Y, double angle = 0){
		SDL_Rect source,target;
		source.x = _rect->x;
		source.y = _rect->y;
		source.w = _rect->w;
		source.h = _rect->h;
		target.x = X;
		target.y = Y;
		target.w = _rect->w;
		target.h = _rect->h;
		SDL_RenderCopyEx(gMyRenderer,origin,&source, &target,angle,NULL,SDL_FLIP_NONE); 
}

SDL_Texture* loadTexture( std::string path)
{
	//The final texture image
	SDL_Texture*  finalTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		cout << "Unable to load image " << path.c_str() <<"! SDL_image Error: " << IMG_GetError() << endl;
	}
	else
	{
		//Convert surface to renderer format
		finalTexture = SDL_CreateTextureFromSurface(gMyRenderer,loadedSurface);
		if( finalTexture == NULL )
		{
			cout << "Unable to optimize image " << path.c_str() << "! SDL Error: " << SDL_GetError() << endl;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	return finalTexture;
}

int main( int argc, char* args[] )
{
	SDL_Window* gWindow = NULL;

	//Player
	SDL_Texture* sPlayer = NULL;
	SDL_Rect	Player_rect = {0,0,854,480};
	int	Player_x = 0;
	int Player_y = 0;
	//Font
	SDL_Texture* sFont = NULL;
	SDL_Rect	Font_rect = {0,0,30,50};
	int	Font_x = 570;
	int Font_y = 304;
	//Audios
	vector<Mix_Chunk*> audios;

	//Mouse
	int	mouseX=0,mouseY=0;

	//Audio Control
	int Track = 1;

	bool	exit = false;

//INIT	
	//Initialize SDL
	SDL_Init( SDL_INIT_EVERYTHING );
	//Create window
	gWindow = SDL_CreateWindow( "UOC Reto 1 - Sonido", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
	//Initialize PNG loading
	IMG_Init( IMG_INIT_PNG );
	//Get window renderer
	gMyRenderer = SDL_CreateRenderer(gWindow,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	//Sound audio active
	Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,1024);

	//Load PNG surface
	sPlayer = loadTexture( "Assets/playeraudio.png");
	sFont = loadTexture ( "Assets/playerfont.png");
	//Load Audios
	Mix_Chunk* loadSound;
	loadSound = Mix_LoadWAV("Assets/track01.ogg");
	audios.push_back(loadSound);
	loadSound = Mix_LoadWAV("Assets/track02.ogg");
	audios.push_back(loadSound);
	loadSound = Mix_LoadWAV("Assets/track03.ogg");
	audios.push_back(loadSound);

	while(!exit){

// UPDATE
		SDL_Event test_event;
		SDL_Scancode tecla;
		while (SDL_PollEvent(&test_event)) {
			switch (test_event.type) {
				case SDL_KEYDOWN:
					tecla = test_event.key.keysym.scancode;
					if (tecla == SDL_SCANCODE_ESCAPE){
						exit = true;
					}
				// Key Changes
					//BACK
					if (tecla == SDL_SCANCODE_Z){
						Track --;
						if (Track < 1){
							Track = 1;
						}
						if (Mix_Playing(1)){
							Mix_HaltChannel(1);
							Mix_PlayChannel(1,audios[Track-1],0);
						}
					}
					//PLAY
					if (tecla == SDL_SCANCODE_X){
						if (!Mix_Playing(1)){
							Mix_HaltChannel(1);
							Mix_PlayChannel(1,audios[Track-1],0);
						}
					}
					//PAUSE
					if (tecla == SDL_SCANCODE_C){
						if (Mix_Paused(1)){
							Mix_Resume(1);
						}else{
							Mix_Pause(1);
						}
						
					}
					//STOP
					if (tecla == SDL_SCANCODE_V){
						Mix_HaltChannel(1);
					}
					//NEXT
					if (tecla == SDL_SCANCODE_B){
						Track ++;
						if (Track > 3){
							Track = 3;
						}
						if (Mix_Playing(1)){
							Mix_HaltChannel(1);
							Mix_PlayChannel(1,audios[Track-1],0);
						}
					}
				break;

				case SDL_QUIT:
					exit = true;
      			break;
				case SDL_MOUSEMOTION:
					mouseX = test_event.motion.x;
					mouseY = test_event.motion.y;
					break;
			}
		}

//RENDER
		// Clear Background
		//Apply the PNG image
		renderTexture(sPlayer,&Player_rect,Player_x,Player_y);
		renderTexture(sFont,&Font_rect,Font_x,Font_y);
		SDL_Rect temp = Font_rect;
		temp.x = Track*temp.w;
		renderTexture(sFont,&temp,Font_x+temp.w+5,Font_y);
		//Update the surface
		SDL_RenderPresent( gMyRenderer);
	}



	//Free loaded image
	SDL_DestroyTexture( sPlayer );
	SDL_DestroyTexture( sFont );

	//Destroy window
	SDL_DestroyRenderer(gMyRenderer);
	gMyRenderer = NULL;
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	//Quit SDL subsystems
	Mix_CloseAudio();
	IMG_Quit();
	SDL_Quit();

	return 0;
}