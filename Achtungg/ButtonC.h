#pragma once
#include "TextureC.h"
#include "WindowC.h"

class GameC;

class ButtonC
{
protected:
	std::string sname;
	TextureC name;
	bool focused, lclicked;
	SDL_Rect Rect;
	SDL_Color color, clickedcolor, idlecolor, focusedcolor;
	bool isfocused(int mousex, int mousey);

public:
	static WindowC* Window;
	static GameC* f;

	enum ButtonType { plain, complex };
	ButtonType type;

	ButtonC() {};
	ButtonC(SDL_Rect Rect, std::string name, SDL_Color color, SDL_Color focused, SDL_Color clicked, ButtonType type);
	~ButtonC();



	typedef void (GameC::*funptr)(void);
	funptr fun;

	virtual void check(int mousex, int mousey);
	virtual void draw();
	virtual void handle(SDL_Event& e);
	virtual void clicked();

	void changexy(int x, int y) { Rect.x = x; Rect.y = y; }
	void changecolor(SDL_Color c) { idlecolor = c; color = c; focusedcolor = c; }
	void changeclickedcolor(SDL_Color c) { clickedcolor = c; }
	void changetexture(std::string name);

	int getx() { return Rect.x; }
	int gety() { return Rect.y; }
	int getw() { return Rect.w; }
	int geth() { return Rect.h; }
	SDL_Color getcolor() { return color; }
	bool isfocused() { return focused; }
	std::string getname() { return sname; }

};

