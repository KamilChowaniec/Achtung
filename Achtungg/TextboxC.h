#pragma once
#include "ButtonC.h"
class TextboxC : public ButtonC
{
private:
	Uint32 Timer;
	bool input;
	static TextureC* vbar;
public:
	static void initvbar();
	TextboxC();
	TextboxC(SDL_Rect Rect, std::string name, SDL_Color color);

	void clicked();
	void draw();
	void check(int mousex, int mousey);
	void handle(SDL_Event & e);
};

