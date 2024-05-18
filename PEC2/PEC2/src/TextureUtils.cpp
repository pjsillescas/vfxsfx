#include "TextureUtils.h"

#include <SDL.h>
#include <SDL_image.h>

#include <iostream>

GLuint TextureUtils::loadTextureFromDisk(std::string file)
{
	GLuint texture = -1;
	
	//Create Texture
	SDL_Surface* tempSurface;
	tempSurface = IMG_Load(file.c_str());
	if (tempSurface == NULL)
	{
		std::cout << "Texture:" << file.c_str() << " not load!" << std::endl;
		return -1;
	}
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	GLint Mode = GL_RGB;
	if (tempSurface->format->BytesPerPixel == 4) Mode = GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, Mode, tempSurface->w, tempSurface->h, 0, Mode, GL_UNSIGNED_BYTE, tempSurface->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	SDL_FreeSurface(tempSurface);

	return texture;
}
