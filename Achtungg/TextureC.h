#pragma once
#include <SDL.h>
struct TextureC {
	SDL_Texture* Texture;
	int h, w;
};

enum textures {
	Achtung,
	Name,
	Left,
	Right,
	Color,
	playercircle,
	Made_by_KC,
	maxtextures
};