#include "TextboxC.h"

TextureC* TextboxC::vbar = nullptr;

TextboxC::TextboxC()
{
}

TextboxC::TextboxC(SDL_Rect Rect, std::string name, SDL_Color color)   {
	this->name = Window->loadTextTexture(name.c_str(), { 255,255,255,255 });
	this->Rect = Rect;
	this->color = color;
	lclicked = false;
	input = false;
}


void TextboxC::clicked()
{
	input = true;
	(f->*fun)();
	if (input) Timer = SDL_GetTicks() - 500;
}

void TextboxC::draw()
{
	Window->drawRect(Rect.x, Rect.y, Rect.w, Rect.h, WindowC::Rec::filled, color);
	Window->drawRect(Rect.x - 1, Rect.y - 1, Rect.w + 2, Rect.h + 2, WindowC::Rec::empty, { 0,0,0,0 });
	Window->drawRect(Rect.x - 2, Rect.y - 2, Rect.w + 4, Rect.h + 4, WindowC::Rec::empty);
	int w = name.w > Rect.w ? Rect.w : name.w;
	Window->renderTexture(name, Rect.x + Rect.w / 2 - w / 2, Rect.y + Rect.h / 2 - name.h / 2, w);
	if (input && SDL_GetTicks() - Timer > 500) {
		Window->renderTexture(*vbar, Rect.x + Rect.w / 2 - w / 2 + w, Rect.y + Rect.h / 2 - (*vbar).h / 2);
		if (SDL_GetTicks() - Timer>1000) Timer = SDL_GetTicks();
	}
}

void TextboxC::check(int mousex, int mousey)
{
	isfocused(mousex, mousey);
}

void TextboxC::initvbar()
{
	vbar = new TextureC;
	*vbar = Window->loadTextTexture("|", { 255,255,255,255 });
}

void TextboxC::handle(SDL_Event& e)
{
	if (input == true) {
		switch (e.type)
		{
		case SDL_MOUSEBUTTONDOWN:
			if (!focused) {
				input = false;
				SDL_StopTextInput();
			}
			break;
		case SDL_KEYDOWN:
			switch (e.key.keysym.sym)
			{
			case SDLK_KP_ENTER:
			case SDLK_RETURN:
				input = false;
				SDL_StopTextInput();
				break;
			case SDLK_BACKSPACE:
				if (sname.length() > 0) {
					Timer = SDL_GetTicks() - 500;
					sname.pop_back();
					changetexture(sname);
				}
				break;
			}
			break;
		case SDL_TEXTINPUT:
			if (sname.length() < 16) {
				Timer = SDL_GetTicks() - 500;
				sname.push_back(*e.text.text);
				changetexture(sname);
			}
			break;
		}
	}
	else {
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
				if (lclicked) {
					if (focused) clicked();
				
				}

				lclicked = false;
			}
			break;
		}
	}


}