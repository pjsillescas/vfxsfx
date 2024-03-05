#include "TransitionEffect.h"


#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <cmath>
#include "Clock.h"

static int currentTime = 0;
static int startTime = 0;
static bool useImageSrc = true;

TransitionEffect::TransitionEffect(SDL_Surface* surface, int screenHeight, int screenWidth, EffectTemplate* src, EffectTemplate* dst) : EffectTemplate(surface, screenHeight, screenWidth)
{
	this->src = src;
	this->dst = dst;
}

void TransitionEffect::init() {
	SDL_Surface* temp;
	
	temp = IMG_Load("uoc.png");
	if (temp == NULL) {
		std::cout << "Image can be loaded! " << IMG_GetError();
		exit(1);
	}
	imageSrc = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_ARGB8888, 0);

	temp = IMG_Load("uoc2.png");
	if (temp == NULL) {
		std::cout << "Image can be loaded! " << IMG_GetError();
		exit(1);
	}
	imageDst = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_ARGB8888, 0);

	startTime = Clock::getInstance().getCurrentTime();
	currentTime = Clock::getInstance().getCurrentTime();
}

void TransitionEffect::update(float deltaTime)
{
	copyImage();
}

void TransitionEffect::render() {
}

TransitionEffect::~TransitionEffect()
{
	SDL_FreeSurface(imageSrc);
	SDL_FreeSurface(imageDst);
}


static int pixelNum = 0;


Uint8* TransitionEffect::getImageBuffer(int i, int j, Uint8* imageBufferSrc, Uint8* imageBufferDst)
{
	if (j > pixelNum)
	{
		//std::cout << "change (" << i << "," << j << ")" << std::endl;
		return imageBufferSrc;
	}
	else
	{
		return 	imageBufferDst;
	}
}



/*
*   copy an image to the screen with added distortion.
*   no bilinear filtering.
*/
void TransitionEffect::copyImage()
{
	/*
	currentTime = Clock::getInstance().getCurrentTime();
	if (currentTime - startTime > 5000)
	{
		startTime = Clock::getInstance().getCurrentTime();
		useImageSrc = !useImageSrc;
	}
	*/

	// setup the offsets in the buffers
	Uint8* dst;
	Uint8* initbuffer = (Uint8*)surface->pixels;
	int bpp = surface->format->BytesPerPixel;
	
	Uint8* imagebuffer;
	int bppImage;
	int pitch;

	Uint8* imageBufferSrc = (Uint8*)imageSrc->pixels;
	Uint8* imageBufferDst = (Uint8*)imageDst->pixels;
	bppImage = imageSrc->format->BytesPerPixel;
	pitch = imageSrc->pitch;

	/*
	if (useImageSrc)
	{
		imagebuffer = (Uint8*)imageSrc->pixels;
		bppImage = imageSrc->format->BytesPerPixel;
		pitch = imageSrc->pitch;
	}
	else
	{
		imagebuffer = (Uint8*)imageDst->pixels;
		bppImage = imageDst->format->BytesPerPixel;
		pitch = imageDst->pitch;
	}
	*/

	SDL_LockSurface(surface);
	// loop for all lines
	for (int j = 0; j < screenHeight; j++)
	{
		dst = initbuffer + j * surface->pitch;
		
		// for all pixels
		for (int i = 0; i < screenWidth; i++)
		{
			//imagebuffer = (j * screenWidth + i > pixelNum) ? imageBufferDst : imageBufferSrc;

			imagebuffer = getImageBuffer(i, j, imageBufferSrc, imageBufferDst);
			// copy it to the screen
			Uint8* p = (Uint8*)imagebuffer + j * pitch + i * bppImage;
			*(Uint32*)dst = *(Uint32*)p;
			
			// next pixel
			dst += bpp;
		}
	}
	SDL_UnlockSurface(surface);
	
	pixelNum+= 2;
	if (pixelNum >= screenHeight * screenWidth)
	{
		pixelNum = 0;
	}

}
