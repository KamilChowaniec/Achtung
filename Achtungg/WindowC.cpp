#include "WindowC.h"

WindowC::WindowC(char *s, int width, int height) : s_width(width), s_height(height), name(s) {
	Window = NULL;
	Renderer = NULL;
	init(); 
}

bool WindowC::init() {
	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
			success = false;
		}

		Window = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, s_width, s_height, SDL_WINDOW_SHOWN); 
		if (Window == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
			if (Renderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor(Renderer, 0xFF, 0xFF, 0xFF, 0xFF);

				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
				else Font = TTF_OpenFont("ScribbletasticBrush.ttf", 14);
			}
		}
	}

	return success;
}

void WindowC::changefont(const char *path,int size)
{
	TTF_CloseFont(Font);
	Font = TTF_OpenFont(path, size);
}

TextureC WindowC::loadTexture(const char * path, SDL_Color colorkey)
{
	TextureC newTexture = { NULL };

	SDL_Surface* loadedSurface = IMG_Load(path);
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
	}
	else
	{
		if (colorkey.r != 1 || colorkey.g != 2 || colorkey.b != 3 || colorkey.a != 4) {
			SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, colorkey.r, colorkey.g, colorkey.b));
		}
		newTexture.Texture = SDL_CreateTextureFromSurface(Renderer, loadedSurface);
		if (newTexture.Texture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
		}
		newTexture.w = loadedSurface->w;
		newTexture.h = loadedSurface->h;

		SDL_FreeSurface(loadedSurface);
	}
	return newTexture;
}

TextureC WindowC::loadTextTexture(const char *text, SDL_Color textColor)
{
	TextureC newTexture = { NULL };
	SDL_Surface* loadedTextSurface = TTF_RenderText_Blended(Font, text, textColor);

	if (loadedTextSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", text, IMG_GetError());
	}
	else
	{
		newTexture.Texture = SDL_CreateTextureFromSurface(Renderer, loadedTextSurface);
		if (newTexture.Texture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", text, SDL_GetError());
		}
		newTexture.w = loadedTextSurface->w;
		newTexture.h = loadedTextSurface->h;

		SDL_FreeSurface(loadedTextSurface);
	}

	return newTexture;
}

bool WindowC::loadMedia(TextureC* textures) {
	
	bool success = true;
	SDL_Color textColor = { 255, 255, 255,255 };
	changefont("ScribbletasticBrush.ttf", 44);
	textures[Achtung] = loadTextTexture("Achtung",textColor);
	changefont("ScribbletasticBrush.ttf", 10);
	textures[Name] = loadTextTexture("Name", textColor);
	textures[Left] = loadTextTexture("Left", textColor);
	textures[Right] = loadTextTexture("Right", textColor);
	textures[Color] = loadTextTexture("Color", textColor);
	textures[Made_by_KC] = loadTextTexture("Made by Kamil Chowaniec", textColor);
	
	textures[playercircle] = loadTexture("textures/white.png");

	char *num[] = { "0","1","2","3","4","5","6","7","8","9","." };
	for (int i = 0; i < 11; i++) {
		number[i] = loadTextTexture(num[i], { 255,255,255,255 });
	}



	changefont("FreeSans.ttf", 14);

	return success;
}

void WindowC::renderTexture(TextureC texture, int x, int y,int w, int h, SDL_Color colormode, double angle) {
	SDL_Rect Rectangle = {
		x,
		y,
		w?w:texture.w,
		h?h:texture.h
	};
	SDL_SetTextureColorMod(texture.Texture, colormode.r, colormode.g, colormode.b);
	SDL_RenderCopyEx(Renderer, texture.Texture, NULL, &Rectangle,angle,NULL,SDL_FLIP_NONE);
}

void WindowC::renderSpriteTexture(TextureC texture, int x, int y, int w, int h,SDL_Rect onSprite, SDL_Color colormode, double angle) {
	SDL_Rect Rectangle = {
		x,
		y,
		w ? w : texture.w,
		h ? h : texture.h
	};
	SDL_SetTextureColorMod(texture.Texture, colormode.r, colormode.g, colormode.b);
	SDL_RenderCopyEx(Renderer, texture.Texture,&onSprite, &Rectangle, angle, NULL, SDL_FLIP_NONE);
}

void WindowC::screenUpdate() {
	SDL_RenderPresent(Renderer);
}

void WindowC::screenClear(SDL_Color color)
{
	SDL_SetRenderDrawColor(Renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(Renderer);
	SDL_SetRenderDrawColor(Renderer, 255,255,255,255 );
}

void WindowC::drawRect(int x, int y, int w, int h,Rec type, SDL_Color color) {
	SDL_Rect Rect = { x,y,w,h };
	SDL_SetRenderDrawColor(Renderer, color.r, color.g, color.b, color.a);
	if(type==filled) SDL_RenderFillRect(Renderer, &Rect);
	else if(type==empty) SDL_RenderDrawRect(Renderer, &Rect);
	SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
}

void WindowC::drawLine(int x, int y, int l, Line type, SDL_Color color)
{
	SDL_SetRenderDrawColor(Renderer, color.r, color.g, color.b, color.a);
	if (type == vertical) SDL_RenderDrawLine(Renderer, x, y, x, y + l);
	else SDL_RenderDrawLine(Renderer, x, y, x+l, y);
	SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
}

void WindowC::drawPixel(int x, int y, SDL_Color color) {
	SDL_SetRenderDrawColor(Renderer, color.r, color.g, color.b, color.a);
	SDL_RenderDrawPoint(Renderer, x, y);
	SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
}

void WindowC::drawnumber(int x, int y, int n) {
	if(n==0) renderTexture(number[0], x , y);
	else {
		int *num = new int[numlen(n)];
		int i;
		for (i = 0; n > 0;i++) {
			num[i]=n % 10;
			n /= 10;
		}
		int widthsum = 0;
		for (int j = i-1; j >= 0; j--) {
			renderTexture(number[num[j]], x + widthsum, y);
			widthsum += number[num[j]].w;
		}
	}
}

void WindowC::drawnumber(int x, int y, double n) {
	char num1[20] = { 0 };
	snprintf(num1, 20, "%g", n);
	if (n == 0) renderTexture(number[0], x, y);
	else {
		int widthsum = 0;
		for (int i = 0; num1[i]; i++) {
			renderTexture(number[int(num1[i]) - 48 == -4 ? 10 : int(num1[i]) - 48], x + widthsum, y);
			widthsum += number[int(num1[i]) - 48 == -4 ? 10 : int(num1[i]) - 48].w;
		}
	}
}

int WindowC::numlen(int n) {
	if (n < 10) return 1;
	if (n < 1000) return 2;
	if (n < 10000) return 3;
	if (n < 100000) return 4;
	if (n < 1000000) return 5;
	if (n < 10000000) return 6;
	if (n < 100000000) return 7;
	if (n < 1000000000) return 8;
	if (n < 10000000000) return 9;
	if (n < 100000000000) return 10;
}

TextureC WindowC::CreateTextureFromWindow() {
	TextureC newTexture = { NULL,0,0 };
	SDL_Surface* saveSurface = NULL;
	SDL_Surface* infoSurface = NULL;
	infoSurface = SDL_GetWindowSurface(Window);
	if (infoSurface == NULL) {
		std::cout << "Failed to create info surface from window in saveScreenshotBMP(string), SDL_GetError() - " << SDL_GetError() << "\n";
	}
	else {
		unsigned char * pixels = new (std::nothrow) unsigned char[infoSurface->w * infoSurface->h * infoSurface->format->BytesPerPixel];
		if (pixels == 0) {
			std::cout << "Unable to allocate memory for screenshot pixel data buffer!\n";
		}
		else {
			if (SDL_RenderReadPixels(Renderer, &infoSurface->clip_rect, infoSurface->format->format, pixels, infoSurface->w * infoSurface->format->BytesPerPixel) != 0) {
				std::cout << "Failed to read pixel data from SDL_Renderer object. SDL_GetError() - " << SDL_GetError() << "\n";
				pixels = NULL;
			}
			else {
				saveSurface = SDL_CreateRGBSurfaceFrom(pixels, infoSurface->w, infoSurface->h, infoSurface->format->BitsPerPixel, infoSurface->w * infoSurface->format->BytesPerPixel, infoSurface->format->Rmask, infoSurface->format->Gmask, infoSurface->format->Bmask, infoSurface->format->Amask);
				if (saveSurface == NULL) {
					std::cout << "Couldn't create SDL_Surface from renderer pixel data. SDL_GetError() - " << SDL_GetError() << "\n";
				}
				else {
					newTexture.Texture = SDL_CreateTextureFromSurface(Renderer, saveSurface);
					newTexture.w = saveSurface->w;
					newTexture.h = saveSurface->h;
					SDL_FreeSurface(saveSurface);
					saveSurface = NULL;
				}
			}
			delete[] pixels;
		}
		SDL_FreeSurface(infoSurface);
		infoSurface = NULL;
	}
	return newTexture;
}