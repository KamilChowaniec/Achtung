#include "ColorboxC.h"


ColorboxC::ColorboxC(SDL_Rect R, SDL_Color c)
{
	Rect = R;
	color = c;
	lclicked = false;
}

ColorboxC::~ColorboxC()
{
}

void ColorboxC::draw()
{
	Window->drawRect(Rect.x, Rect.y, Rect.w, Rect.h, WindowC::Rec::filled, color);
	Window->drawRect(Rect.x - 1, Rect.y - 1, Rect.w + 2, Rect.h + 2, WindowC::Rec::empty, { 0,0,0,0 });
	Window->drawRect(Rect.x - 2, Rect.y - 2, Rect.w + 4, Rect.h + 4, WindowC::Rec::empty);
}

void ColorboxC::check(int mousex, int mousey)
{
	if (isfocused(mousex, mousey)) {
		Window->drawRect(Rect.x - 1, Rect.y - 1, Rect.w + 2, Rect.h + 2, WindowC::Rec::empty, { 255,255,255,255 });
	}
}
