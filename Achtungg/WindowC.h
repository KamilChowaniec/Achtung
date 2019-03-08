#pragma once
#include "TextureC.h"
#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <math.h>
#include <vector>
#include <sstream>
class WindowC
{
private:
	const char *name;
	const int s_width, s_height;
	SDL_Window* Window;
	SDL_Renderer* Renderer;

	TTF_Font *Font;

	TextureC number[11];

	TextureC loadTexture(const char *path, SDL_Color colorkey = { 1,2,3,4 });
public:
	TextureC loadTextTexture(const char *text, SDL_Color textColor);
	TextureC CreateTextureFromWindow();

	enum Rec { filled, empty };
	enum Line { horizontal, vertical };

	WindowC(char *s="", int width=800, int height=600);
	bool init();

	void changefont(const char* path,int size);
	bool loadMedia(TextureC* textures);

	void renderTexture(TextureC texture, int x, int y,int w=0,int h=0, SDL_Color colormode = { 255,255,255,255 }, double angle = 0);
	void renderSpriteTexture(TextureC texture, int x, int y, int w, int h,SDL_Rect onSprite, SDL_Color colormode = { 255,255,255,255 }, double angle=0);

	void screenUpdate();
	void screenClear(SDL_Color color = { 0,0,0,0 });

	void drawRect(int x, int y, int w, int h, Rec = filled,SDL_Color color = { 255,255,255,255 });
	void drawLine(int x, int y, int l,Line type, SDL_Color color );
	void drawPixel(int x, int y, SDL_Color color);
	void drawnumber(int x, int y, int n);
	void drawnumber(int x, int y, double n);
	int numlen(int n);

	int getscreenwidth() { return s_width; }
	int getscreenheight() { return s_height; }
};