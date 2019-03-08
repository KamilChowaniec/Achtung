#pragma once
#include "ButtonC.h"
class ColorboxC :
	public ButtonC
{
public:
	ColorboxC() {}
	ColorboxC(SDL_Rect R, SDL_Color c);
	~ColorboxC();

	void draw();
	void check(int mousex, int mousey);
};

