#pragma once
#include "PlayerC.h"
#include "WindowC.h"
#include "ButtonC.h"
#include "SliderC.h"
#include "TextureC.h"
#include "PowerUpC.h"
#include "TextboxC.h"
#include "ColorboxC.h"
#include <vector>
#include <time.h>
#include <fstream>
using namespace std;


class GameC
{
private:
	int WDecreasetimer;
	bool WDecrease;
	bool colorchange;
	bool textinput;
	bool gameloop;
	int playertextindex;
	int playercolorindex;

	Uint32 Timer;
	Uint32 PowerUpTimer;
	
	SDL_Rect playRect;
	SDL_Rect actualplayRect;
	int mx, my;
	const int s_width, s_height;
	
	enum gamestate { menu_state, start_state, options_state, credits_state, exit_state };
	gamestate state;
	
	SDL_Event e;
	WindowC Window;
	SDL_Color color;
	SDL_Color background;

	int* sortedplayers;
	SliderC<SDL_Color> slider[3];
	TextureC textures[maxtextures];
	int maxnamew;
	TextureC playernames[6];
	vector<int> playersalive;
	vector<PlayerC> player;
	vector<ButtonC> button;
	vector<TextboxC> Textbox;
	vector<PowerUpC> powerup;
public:
	//CONSTRUCTOR

	GameC(int w, int h);

	
	//INIT FUNCTIONS

	void init();
	void initButtons();
	void initSliders();


	//MAIN LOOP

	void Start();


	//MAINMENU

	void Mainmenu();


	//EVENT HANDLING FUNCTIONS

	void handleevents();
	void handlebuttons();
	void handlesliders();
	void handleplayers();


	//INGAME FUNCTIONS

	void ingame();
	void ingameClear();
	void flipanimation();
	void redraw(int x, int y, int r);
	void resetplayers();


	//BUTTON FUNCTIONS

	void addplayer();
	void deleteplayer();
	void buttonstart();
	void buttonoptions();
	void buttoncredits();
	void buttonexit();
	void savesettings();
	void loadsettings();
	void buttonchangename();
	void buttonchangeleftb();
	void buttonchangerightb();
	void buttonchangecolor();


	//POWERUP FUNCTIONS

	void addPowerUp();
	void PowerUpSpeedUpMe(int n);
	void PowerUpSpeedUpElse(int n);
	void PowerUpSlowDownMe(int n);
	void PowerUpSlowDownElse(int n);
	void PowerUpThickUpMe(int n);
	void PowerUpThickUpElse(int n);
	void PowerUpThickDownMe(int n);
	void PowerUpThickDownElse(int n);
	void PowerUpWindowDecrease(int n);
	void PowerUpOpenWalls(int n);
	void PowerUpImmortality(int n);
	void PowerUpManuverityMe(int n);
	void PowerUpManuverityElse(int n);
	void PowerUpClearBoard(int n);
	void PowerUpReverseControls(int n);


	//RANDOM FUNCTIONS

	void RGBtoHSV(SDL_Color color);
	int * sort();
	void drawvalue(SDL_Rect Rect, SDL_Color color);
};

