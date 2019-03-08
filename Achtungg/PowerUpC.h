#pragma once
#include "TextureC.h"
#include "WindowC.h"
class GameC;
class PowerUpC
{
private:
	static TextureC circle;
	static GameC * game;
	static WindowC * Window;
	int x, y, r;
	bool taken;
	SDL_Color color;
	TextureC type;
public:
	static void initcircletexture(TextureC t) { circle = t; }
	static void initGameC(GameC * f) { game = f; }
	static void initWindowC(WindowC * f) { Window = f; }
	typedef void (GameC::*funptr)(int);
	funptr fun;
	PowerUpC() {}
	PowerUpC(int wr,int wx,int wy,SDL_Color c) {
		color = c;
		x = wx;
		y = wy;
		r = wr;
		taken = false;
	}
	void initTexture(char *s){
		Window->changefont("FreeSans.ttf", 10); 
		type = Window->loadTextTexture(s, { 255,255,255,255 });
		Window->changefont("FreeSans.ttf", 14);
	}
	void draw();
	bool check(int px, int py);
	void del(SDL_Color c);
	void active(int n) { (game->*fun)(n); }
	void changecolor(SDL_Color c) { color = c; }

	int getx() { return x; }
	int gety() { return y; }
	int getr() { return r; }
};

