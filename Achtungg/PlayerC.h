#pragma once
#include "WindowC.h"
#include "ButtonC.h"
#include <fstream>
#include <vector>


class PlayerC
{
public: enum powerups { SpeedUp, SlowDown, ThickUp, ThickDown, ReverseControls, OpenWalls, Immortality, Manuverity };

private:

	std::string name;
	bool leftactive, rightactive;
	bool manuverleftactive, manuverrightactive;
	bool manuver;
	bool immortal;
	bool walls;
	bool jump;
	bool alive;
	int number;
	Uint32 manuvertimer;
	int jumpcount, jumpmax;
	int thick;
	int pkt, speed;
	float direction;
	double x, y;
	SDL_Keycode left, right,trueleft,trueright;
	SDL_Color color;
	
	static SDL_Rect* playRect;
	Uint32 Timer;

	struct PU {
		Uint32 time;
		powerups type;
	};
	struct coord {
		int x, y; 
	};
	coord last[20];
	std::vector<PU> activePowerUp;
	

	static WindowC* Window;
	static TextureC circle;

public:

	void operator+=(int pkt) {
		this->pkt += pkt;
	}
	friend std::ostream& operator<<(std::ostream& os, const PlayerC& player);
	friend std::istream& operator>>(std::istream& is, PlayerC& player);

	ButtonC* leftbutton, *rightbutton, *colorbutton, *namebutton;


	static void initTexture(TextureC c) { circle = c; }
	static void initWindow(WindowC * W) { Window = W; }
	static int ** board;
	static int ** makeboard(int x, int y);
	static void playRectinit(SDL_Rect* Rect) { playRect = Rect; }
	static void resetboard();

	void resetlastmoves();

	PlayerC(int x = 0, int y = 0, int dir = 0, SDL_Keycode l = SDL_SCANCODE_A, SDL_Keycode r = SDL_SCANCODE_D, SDL_Color c = { 255,255,255,255 }, int num = 0);
	void move();
	void collision();
	static int getthicknesslevel(int thick);
	static int getthicknessfromlevel(int lvl);
	void getcollisionpixel(int & cx, int & cy, float dir);
	bool pixelcollision();
	bool outofmap();

	void changedir(char znak);
	void reset(int x, int y);
	void resetpkt() { pkt = 0; }
	void setjumpcount(int x) { jumpcount = x; }

	void draw();
	void drawErase(SDL_Color c);
	void redraw(int thick,int x,int y);

	int getx() { return x; }
	int gety() { return y; }
	int getthick() { return thick; }
	float getdir() { return direction; }
	int getpkt() { return pkt; }
	bool getjump() { return jump; }
	bool isalive() { return alive; }
	SDL_Keycode getleftb() { return left; }
	SDL_Keycode getrightb() { return right; }
	SDL_Color getcolor() { return color; }
	std::string getname() { return name; }

	void changeleftb(SDL_Keycode button) { trueleft = left = button; }
	void changerightb(SDL_Keycode button) { trueright = right = button; }
	void changecolor(SDL_Color c) { color = c; }
	void changename(std::string name) { PlayerC::name = name; }
	void addpkt(int a) { pkt += a; }
	void changespeed(int a) { speed += a; }

	void PowerUp(powerups type);
	void prolongPowerUp(powerups type, int time);
	void handlepowerups();
	void swapcontrols();
	void handle(SDL_Event & e);
	void handlebuttons(SDL_Event & e) {
		namebutton->handle(e);
		leftbutton->handle(e);
		rightbutton->handle(e);
		colorbutton->handle(e);
	}
	void drawbuttons(int mx, int my) {
		namebutton->check(mx,my);
		leftbutton->check(mx, my);
		rightbutton->check(mx, my);
		colorbutton->check(mx, my);
		namebutton->draw();
		leftbutton->draw();
		rightbutton->draw();
		colorbutton->draw();
	}
};

