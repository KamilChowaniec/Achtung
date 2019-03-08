#include "PowerUpC.h"

TextureC PowerUpC::circle = { NULL,0,0 };
GameC * PowerUpC::game = nullptr;
WindowC * PowerUpC::Window = nullptr;

void PowerUpC::draw()
{
	Window->renderTexture(circle, x - r, y - r, 2 * r, 2 * r, color);
	Window->renderTexture(type, x - type.w / 2 + 1, y - type.h / 2 + 1, 0, 0, { 0,0,0,0 });
	Window->renderTexture(type, x - type.w / 2, y - type.h / 2);

}

bool PowerUpC::check(int px, int py)
{
	if ((px - x)*(px - x) + (py - y)*(py - y) < r*r) {
		return true;
	}
}

void PowerUpC::del(SDL_Color backgroundc)
{
	Window->renderTexture(circle, x - r - 3, y - r - 3, 2 * r + 6, 2 * r + 6, backgroundc);
	Window->renderTexture(type, x - type.w / 2, y - type.h / 2, 0, 0, backgroundc);
}
