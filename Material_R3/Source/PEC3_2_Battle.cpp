#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>

using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 512;
const int SCREEN_HEIGHT = 512;

#define PI 3.14159265

#define MAX_TIME_SHIP_CROSS 5000 // in milisecons

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

	//Sound controls
	int global_deltaTime;
	int global_lastTime;

	int timeNextExplosion;
	int posExplosion;
	int distanceExplosion;

	int timeNextLaser;
	int posLaser;
	int distanceLaser;

	int timeNextShip;
	int posShip;
	int timeCrossing;
	int shipGoRight;
	int shipChannel;

	//Background
	SDL_Texture* sBG = NULL;
	SDL_Rect	BG_rect = {0,0,512,512};
	int	BG_x = 0;
	int BG_y = 0;
	//Explosion
	SDL_Texture* sBoom = NULL;
	SDL_Rect	Boom_rect = {0,0,32,32};
	int	Boom_x = -32;
	int Boom_y = -32;
	//Laser
	SDL_Texture* sLaser = NULL;
	SDL_Rect	Laser_rect = { 0,0,32,32 };
	int Laser_x = -32;
	int Laser_y = -32;
	//Ship
	SDL_Texture* sShip = NULL;
	SDL_Rect	Ship_rect = { 0,0,32,32 };
	int Ship_X = -32;
	int Ship_Y = 240;
	//Audios
	vector<Mix_Chunk*> audios;

	//Mouse
	int	mouseX=0,mouseY=0;

	bool	exit = false;

//INIT	

	srand(time(0));
	//Initialize SDL
	SDL_Init( SDL_INIT_EVERYTHING );
	//Create window
	gWindow = SDL_CreateWindow( "UOC Reto 2 - Sonido", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
	//Initialize PNG loading
	IMG_Init( IMG_INIT_PNG );
	//Get window renderer
	gMyRenderer = SDL_CreateRenderer(gWindow,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetHint("SDL_HINT_RENDER_VSYNC", "1");
	//Sound audio active
	Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,1024);

	//Load PNG surface
	sBG = loadTexture("Assets/space/background.png");
	sBoom = loadTexture ("Assets/space/boom.png");
	sLaser = loadTexture("Assets/space/laser.png");
	sShip = loadTexture("Assets/space/ship.png");
	//Load Audios
	Mix_Chunk* loadSound;
	loadSound = Mix_LoadWAV("Assets/space/explosion1.wav");
	audios.push_back(loadSound);
	loadSound = Mix_LoadWAV("Assets/space/explosion2.wav");
	audios.push_back(loadSound);
	loadSound = Mix_LoadWAV("Assets/space/tie_fighter.wav");
	audios.push_back(loadSound);
	loadSound = Mix_LoadWAV("Assets/space/xwing.wav");
	audios.push_back(loadSound);
	loadSound = Mix_LoadWAV("Assets/space/cannon.wav");
	audios.push_back(loadSound);
	loadSound = Mix_LoadWAV("Assets/space/laser.wav");
	audios.push_back(loadSound);

	// Init sound controls
	global_deltaTime = 0;
	global_lastTime = 0;
	timeNextExplosion = 2000 + (rand() % 5) * 1000;
	timeNextLaser = 100 + (rand() % 10) * 100;
	timeNextShip = 1000 + (rand() % 5) * 1000;
	shipGoRight = rand() % 2;

	while(!exit){
// GLOBAL TIMER
		global_deltaTime = SDL_GetTicks() - global_lastTime;
		global_lastTime = SDL_GetTicks();
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
				break;

				case SDL_QUIT:
					exit = true;
      			break;
			}
		}
// UPDATE SOUNDS
		timeNextExplosion -= global_deltaTime;
		timeNextLaser -= global_deltaTime;
		if (timeNextExplosion < 0) {
			timeNextExplosion = 2000 + (rand() % 5) * 1000;
			posExplosion = rand() % 360;
			distanceExplosion = rand() % 256;
			int Channel = Mix_PlayChannel(-1, audios[rand() % 2], 0);
			Mix_SetPosition(Channel, posExplosion, distanceExplosion);
			Boom_x = distanceExplosion * sin((posExplosion*PI) / 180.0) + 256;
			Boom_y = distanceExplosion * cos((posExplosion*PI) / 180.0) + 256;
			//cout << "Explosion Angle:" << posExplosion << " distance:" << distanceExplosion <<" Next in:" << timeNextExplosion << endl;
		}
		if (timeNextLaser < 0) {
			timeNextLaser = 100 + (rand() % 10) * 100;
			posLaser = rand() % 360;
			distanceLaser = rand() % 256;
			int Channel = Mix_PlayChannel(-1, audios[4 + rand() % 2], 0);
			Mix_SetPosition(Channel, posLaser, distanceLaser);
			Laser_x = distanceLaser * sin((posLaser*PI) / 180.0) + 256;
			Laser_y = distanceLaser * cos((posLaser*PI) / 180.0) + 256;
			//cout << "Laser Angle:" << posLaser << " distance:" << distanceLaser << " Next in:" << timeNextLaser << endl;
		}

		if (timeNextShip > 0) {
			timeNextShip -= global_deltaTime;
			if (timeNextShip < 0) {
				timeNextShip = 0;
				timeCrossing = MAX_TIME_SHIP_CROSS;
				shipChannel = Mix_PlayChannel(-1, audios[2+rand() % 2], 0);
			}
		}
		if (timeNextShip == 0) { // Starship is crossing
			timeCrossing -= global_deltaTime;
			if (timeCrossing > 0) {
				posShip = (255 * timeCrossing) / MAX_TIME_SHIP_CROSS;
				Uint8 leftShip;
				Uint8 rightShip;
				if (shipGoRight) {
					rightShip = 255 - posShip;
					leftShip = posShip;
				}
				else {
					rightShip = posShip;
					leftShip = 255 - posShip;
				}
				Mix_SetPanning(shipChannel, leftShip, rightShip);
				Ship_X = rightShip * 2;
				//cout << "Ship Pass:" << timeCrossing << " Left:" << (int)leftShip << " Right:" << (int)rightShip << endl;
			}
			if (timeCrossing < 0) {
				timeCrossing = 0;
				timeNextShip = 1000 + (rand() % 5) * 1000;
				shipGoRight = rand() % 2;
			}
		}
//RENDER
		// Clear Background
		//Apply the PNG image
		renderTexture(sBG,&BG_rect,BG_x,BG_y);
		renderTexture(sBoom,&Boom_rect,Boom_x,Boom_y);
		renderTexture(sLaser, &Laser_rect, Laser_x, Laser_y);
		renderTexture(sShip, &Ship_rect, Ship_X, Ship_Y);
		//Update the surface
		SDL_RenderPresent( gMyRenderer);
	}



	//Free loaded image
	SDL_DestroyTexture(sBG);
	SDL_DestroyTexture(sBoom);
	SDL_DestroyTexture(sLaser);
	SDL_DestroyTexture(sShip);

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