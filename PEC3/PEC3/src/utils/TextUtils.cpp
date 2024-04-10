#include "TextUtils.h"

#include <iostream>

TTF_Font* font = NULL;

TTF_Font* TextUtils::getFont(int size)
{
	if (font == NULL)
	{
		// Remember to call TTF_Init(), TTF_Quit(), before/after using this function.
		font = TTF_OpenFont("assets/fonts/LEMONMILK-Regular.otf", size);
		if (!font) {
			std::cout << "[ERROR] TTF_OpenFont() Failed with: " << TTF_GetError() << std::endl;
			exit(2);
		}
	}

	return font;
}

void TextUtils::drawText(SDL_Surface* screen, const char* string, int size, int x, int y, SDL_Color fgC, SDL_Color bgC)
{
	// Remember to call TTF_Init(), TTF_Quit(), before/after using this function.
	TTF_Font* font = getFont(size);
	TTF_SetFontStyle(font, TTF_STYLE_BOLD);
	//SDL_Surface* textSurface = TTF_RenderText_Solid(font, string, fgC);     // aliased glyphs
	SDL_Surface* textSurface = TTF_RenderText_Shaded(font, string, fgC, bgC);   // anti-aliased glyphs
	SDL_Rect textLocation = { x, y, 0, 0 };
	SDL_BlitSurface(textSurface, NULL, screen, &textLocation);
	SDL_FreeSurface(textSurface);
}

void TextUtils::close()
{
	TTF_CloseFont(font);
}
