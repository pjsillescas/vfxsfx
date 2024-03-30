#pragma once

#include <SDL_ttf.h>

class TextUtils
{
public:
	static void drawText(SDL_Surface* screen, const char* string, int size, int x, int y, SDL_Color fgC, SDL_Color bgC);
	static void close();
private:
	static TTF_Font* getFont(int size);

};

