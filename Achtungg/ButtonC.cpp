#include "GameC.h"
#include "ButtonC.h"

WindowC *ButtonC::Window = nullptr;
GameC* ButtonC::f = nullptr;

ButtonC::ButtonC(SDL_Rect R, std::string name, SDL_Color c, SDL_Color focused,SDL_Color clicked, ButtonType type) : name(Window->loadTextTexture(name.c_str(), { 255,255,255,255 })) {
	ButtonC::type = type;
	Rect = R;
	idlecolor = c;
	color = c;
	clickedcolor = clicked;
	focusedcolor = focused;
	lclicked = false;
}

ButtonC::~ButtonC()
{
}

bool ButtonC::isfocused(int mousex, int mousey)
{
	if (mousex > Rect.x + Rect.w) focused = false;
	else if (mousex < Rect.x) focused = false;
	else if (mousey < Rect.y) focused = false;
	else if (mousey > Rect.y + Rect.h) focused = false;
	else focused = true;
	return focused;
}



void ButtonC::clicked()
{
	(f->*fun)();
}

void ButtonC::check(int mousex, int mousey)
{
	if (isfocused(mousex, mousey))
	{
		color = focusedcolor;
	}
	else color = idlecolor;
	if (lclicked)color = clickedcolor;
}


void ButtonC::draw()
{
	if (type == complex) {
		Window->drawRect(Rect.x - 4, Rect.y - 4, Rect.w, Rect.h, WindowC::Rec::empty);
		Window->drawRect(Rect.x + 4, Rect.y + 4, Rect.w, Rect.h, WindowC::Rec::empty);
	}
	Window->drawRect(Rect.x, Rect.y, Rect.w, Rect.h, WindowC::Rec::filled, color);
	Window->drawRect(Rect.x - 1, Rect.y - 1, Rect.w + 2, Rect.h + 2, WindowC::Rec::empty, { 0,0,0,0 });
	Window->drawRect(Rect.x - 2, Rect.y - 2, Rect.w + 4, Rect.h + 4, WindowC::Rec::empty);
	int w = name.w > Rect.w ? Rect.w : name.w;
	Window->renderTexture(name, Rect.x + Rect.w / 2 - w / 2, Rect.y + Rect.h / 2 - name.h / 2, w);
}

void ButtonC::handle(SDL_Event& e)
{
	switch (e.type)
	{
	case SDL_MOUSEBUTTONDOWN:
		if (e.button.button == SDL_BUTTON_LEFT && focused) {
			lclicked = true;
		}
		break;
	case SDL_MOUSEBUTTONUP:
		if (e.button.button == SDL_BUTTON_LEFT)
		{
			if (lclicked && focused)  clicked();
			lclicked = false;
		}
		break;
	}
}

void ButtonC::changetexture(std::string name)
{
	SDL_DestroyTexture(ButtonC::name.Texture);
	ButtonC::name = Window->loadTextTexture(name.c_str(), { 255,255,255,255 });
	sname = name;
}

