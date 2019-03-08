#pragma once


template<class T>
class SliderC
{
private:
	T* var;
	float percent;
	bool focusedRect;
	bool focused;
	bool focusedx;
	bool lclicked;
	SDL_Rect Rect;
	SDL_Rect Thumb;
	int max, min;
	typedef void (SliderC::*Drawingfunc)(WindowC* Window, SDL_Color* color);
	typedef T& (*Returnfunc)(T& v, float perc);

	Drawingfunc drawfun;
	Returnfunc retfun;
public:
	

	SliderC() {}
	SliderC(int x, int y, int w, int h)
	{
		percent = 0.5;
		focusedRect = false;
		focused = false;
		lclicked = false;
		Rect.x = x;
		Rect.y = y;
		Rect.w = w;
		Rect.h = h;
		Thumb.w = 2 * Rect.w / 100;
		Thumb.h = 11 * Rect.h / 10;
		Thumb.x = Rect.x + Rect.w / 2 - Thumb.w / 2;
		Thumb.y = Rect.y - 5 * Rect.h / 100;
		min = Rect.x - Thumb.w / 2 + 1;
		max = Rect.x + Rect.w - Thumb.w / 2 - 1;
	}

	~SliderC()
	{
	}

	void init(T *var,Drawingfunc f1,Returnfunc f2 ) {
		this->var = var;
		drawfun = f1;
		retfun = f2;
	}

	void set() {
			*var = retfun(*var, percent);
	}

	void draw(WindowC* Window, SDL_Color *color)
	{
		(this->*drawfun)(Window, color);
		Window->drawRect(Rect.x - 1, Rect.y - 1, Rect.w + 2, Rect.h + 2, WindowC::Rec::empty, { 0,0,0,0 });
		Window->drawRect(Rect.x, Rect.y, Rect.w, Rect.h, WindowC::Rec::empty);
		Window->drawRect(Thumb.x, Thumb.y, Thumb.w, Thumb.h);
		Window->drawRect(Thumb.x - 1, Thumb.y - 1, Thumb.w + 2, Thumb.h + 2, WindowC::Rec::empty, { 0,0,0,0 });
	}


	bool isfocused(int mousex, int mousey)
	{
		focusedx = true;
		focused = true;
		if (mousex > Thumb.x + Thumb.w) { focused = false, focusedx = false; }
		else if (mousex < Thumb.x) { focused = false; focusedx = false; }
		else if (mousey < Thumb.y) focused = false;
		else if (mousey > Thumb.y + Thumb.h) focused = false;
		return focused;
	}

	bool isfocusedonRect(int mousex, int mousey)
	{
		focusedRect = true;
		if (mousex > Rect.x + Rect.w - 1) focusedRect = false;
		else if (mousex < Rect.x + 1) focusedRect = false;
		else if (mousey < Rect.y) focusedRect = false;
		else if (mousey > Rect.y + Rect.h) focusedRect = false;
		return focusedRect;
	}

	void handle(SDL_Event& e)
	{
		switch (e.type)
		{
		case SDL_MOUSEBUTTONDOWN:
			if (e.button.button == SDL_BUTTON_LEFT) {
				if (focusedRect) {
					Thumb.x = e.motion.x - Thumb.w / 2;
					percent = (float(Thumb.x + Thumb.w / 2 - min) / float(max - min));
					lclicked = true;
				}
				else if (focused) lclicked = true;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if (e.button.button == SDL_BUTTON_LEFT) lclicked = false;
			break;
		case SDL_MOUSEMOTION:
			if (lclicked && focusedx) {
				Thumb.x += e.motion.xrel;
				if (Thumb.x < min) Thumb.x = min;
				else if (Thumb.x > max) Thumb.x = max;
				percent = (float(Thumb.x + Thumb.w / 2 - min) / float(max - min));
			}
			break;
		}
	}

	void check(int mousex, int mousey) {
		isfocused(mousex, mousey);
		isfocusedonRect(mousex, mousey);
	}

	void setthumb(float percent)
	{
		Thumb.x = (-percent) * min + min + max*percent - Thumb.w / 2;
		Thumb.x = Thumb.x > max ? max - 1 : Thumb.x;
		Thumb.x = Thumb.x < min ? min + 1 : Thumb.x;

		this->percent = (float(Thumb.x + Thumb.w / 2 - min) / float(max - min));
	}

	void changexy(int x, int y, int w = 0, int h = 0)
	{
		Rect.x = x < 0 ? Rect.x : x;
		Rect.y = y < 0 ? Rect.y : y;
		Rect.w = w ? w : Rect.w;
		Rect.h = h ? h : Rect.h;
		Thumb.w = 2 * Rect.w / 100;
		Thumb.h = 11 * Rect.h / 10;
		Thumb.x = Rect.x + Rect.w / 2 - Thumb.w / 2;
		Thumb.y = Rect.y - 5 * Rect.h / 100;
		min = Rect.x - Thumb.w / 2 + 1;
		max = Rect.x + Rect.w - Thumb.w / 2 - 1;
	}

	void drawhue(WindowC* Window, SDL_Color *C) {
		float r, g, b;
		SDL_Color color = { r = 255,g = 0,b = 0,255 };
		const float c = 255 / (float(Rect.w) / 6);
		for (int i = Rect.x + 1; i < Rect.x + Rect.w; i++) {
			float angle = 360 * (float(i - min) / float(max - min));
			Window->drawLine(i, Rect.y + 1, Rect.h - 2, WindowC::Line::vertical, color);
			if (angle < 60) {
				g += c;
				if (g > 255) g = 255;
				color.g = g;
			}
			else if (angle < 120) {
				r -= c;
				if (r < 0) r = 0;
				color.r = r;
			}
			else if (angle < 180) {
				b += c;
				if (b > 255) b = 255;
				color.b = b;
			}
			else if (angle < 240) {
				g -= c;
				if (g < 0) g = 0;
				color.g = g;
			}
			else if (angle < 300) {
				r += c;
				if (r > 255) r = 255;
				color.r = r;
			}
			else if (angle < 360) {
				b -= c;
				if (b < 0) b = 0;
				color.b = b;;
			}
		}
	}

	void drawsaturation(WindowC * Window, SDL_Color *C)
	{
		SDL_Color color = *C;
		float r = color.r, g = color.g, b = color.b;
		const float rrate = (255 - r) / float(Rect.w), grate = (255 - g) / float(Rect.w), brate = (255 - b) / float(Rect.w);
		for (int i = Rect.x + Rect.w - 1; i >= Rect.x + 1; i--) {
			float perc = (float(i - min) / float(max - min));
			Window->drawLine(i, Rect.y + 1, Rect.h - 2, WindowC::Line::vertical, color);
			r += rrate;
			g += grate;
			b += brate;
			r > 255 ? 255 : r;
			g > 255 ? 255 : g;
			b > 255 ? 255 : b;
			color.r = r;
			color.g = g;
			color.b = b;
		}
	}

	void drawvalue(WindowC * Window, SDL_Color* C)
	{
		SDL_Color color = *C;
		float r = color.r, g = color.g, b = color.b;
		const float rrate = r / float(Rect.w), grate = g / float(Rect.w), brate = b / float(Rect.w);
		for (int i = Rect.x + Rect.w - 1; i >= Rect.x + 1; i--) {
			float perc = (float(i - min) / float(max - min));
			Window->drawLine(i, Rect.y + 1, Rect.h - 2, WindowC::Line::vertical, color);
			r -= rrate;
			g -= grate;
			b -= brate;
			r < 0 ? 0 : r;
			g < 0 ? 0 : g;
			b < 0 ? 0 : b;
			color.r = r;
			color.g = g;
			color.b = b;
		}
	}

};