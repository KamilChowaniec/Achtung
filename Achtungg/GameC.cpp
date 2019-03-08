#include "GameC.h"
SDL_Color& rgbfromval(SDL_Color& color, float percent);
SDL_Color& rgbfromhue(SDL_Color &color, float percent);
SDL_Color& rgbfromsat(SDL_Color& color, float percent);

GameC::GameC(int w, int h) :Window("Achtung", w, h), s_width(w), s_height(h) {
	init();
	srand(time(NULL));
	state = menu_state;
	gameloop = true;
	colorchange = false;
	textinput = false;
	WDecrease = false;
	WDecreasetimer = 0;
	mx = 0, my = 0;
	background = { 0,0,0,0 };
	resetplayers();
	player.reserve(6);
	loadsettings();
	Timer = SDL_GetTicks();
	for (int i = 0; i < 6; i++) {
		playernames[i].Texture = nullptr;
	}
}

void GameC::init() {
	playRect = { s_width - (s_height / 18) - (s_height - (2 * s_height / 18)) - s_width / 15,s_height / 18,s_height - (2 * s_height / 18),s_height - (2 * s_height / 18) };
	actualplayRect = playRect;
	PlayerC::makeboard(s_width, s_height);
	PlayerC::playRectinit(&playRect);
	PlayerC::initWindow(&Window);
	PowerUpC::initWindowC(&Window);
	PowerUpC::initGameC(this);
	initButtons();
	initSliders();
	Window.loadMedia(textures);
	PowerUpC::initcircletexture(textures[playercircle]);
	PlayerC::initTexture(textures[playercircle]);
	TextboxC::initvbar();
}

void GameC::initButtons()
{
	ButtonC::Window = &Window;
	ButtonC::f = this;
	char *name[] = { "START",
		"OPTIONS",
		"CREDITS",
		"EXIT" };
	int bw = s_width * (20 / (float)128);
	int bh = s_height * (5 / (float)72);
	int newx = s_width / 6 - bw / 2;
	int newh = 3 * s_height / 5;
	int newy = (s_height - newh) / 2;
	for (int i = 0; i < 4; i++) {
		button.emplace_back(ButtonC({ newx,newy - bh / 2 + i*(newh / 3) ,bw,bh }, name[i], { 125,125,125,255 }, { 105,105,105,255 }, { 90, 90, 90, 255 }, ButtonC::ButtonType::complex));
	}
	button[0].fun = &GameC::buttonstart;
	button[1].fun = &GameC::buttonoptions;
	button[2].fun = &GameC::buttoncredits;
	button[3].fun = &GameC::buttonexit;

	button.emplace_back(ButtonC({ s_width / 3 + int(0.8*(2 * s_width / 3)),10,20,20 }, "-", { 125,125,125,255 }, { 105,105,105,255 }, { 90,90,90,255 }, ButtonC::ButtonType::complex));
	button.emplace_back(ButtonC({ s_width - int(0.05*(2 * s_width / 3)) - 20,10,20,20 }, "+", { 125,125,125,255 }, { 105,105,105,255 }, { 90,90,90,255 }, ButtonC::ButtonType::complex));

	button[4].fun = &GameC::deleteplayer;
	button[5].fun = &GameC::addplayer;
	int x1 = button[4].getx(), w1 = button[4].getw(), x2 = button[5].getx();
	SDL_Rect pos;
	pos.x = x2 - (x2 - (x1 + w1)) / 2 - 30;
	pos.y = 10;
	pos.w = 60;
	pos.h = 20;

	button.emplace_back(ButtonC(pos, "Save", { 125,125,125,255 }, { 105,105,105,255 }, { 90,90,90,255 }, ButtonC::ButtonType::complex));
	button.back().fun = &GameC::savesettings;
	//button.emplace_back(ButtonC())
}


void GameC::initSliders() {
	for (int i = 0; i < 3; i++)
		slider[i] = SliderC<SDL_Color>(s_width / 2 - 100, s_height / 2 + 40 * i, 400, 20);

	slider[0].init(&color, &SliderC<SDL_Color>::drawhue, &rgbfromhue);
	slider[1].init(&color, &SliderC<SDL_Color>::drawsaturation, &rgbfromsat);
	slider[2].init(&color, &SliderC<SDL_Color>::drawvalue, &rgbfromval);
	for (int i = 0; i < 3; i++) 
		slider[i].changexy(s_width / 3 + 0.76*(2 * s_width / 3), -1, s_width - (s_width / 3 + 0.76*(2 * s_width / 3)) - 0.01*(2 * s_width / 3), 20);
}


void GameC::Start()
{
	while (gameloop) {
		SDL_GetMouseState(&mx, &my);
		if (state != start_state) Window.screenClear({ 0,0,0,0 });

		if (state == start_state) ingame();
		else Mainmenu();

		handleevents();
		Window.screenUpdate();

	}
}

void GameC::Mainmenu()
{
	Window.drawRect(0, 0, s_width / 3, s_height, WindowC::Rec::filled, { 40,40,40,255 });
	Window.drawLine(s_width / 3, 0, s_height, WindowC::Line::vertical, { 255,255,255,255 });
	for (int i = 0; i < 4; i++) {
		button[i].check(mx, my);
		button[i].draw();
	}

	switch (state) {
	case credits_state:
		Window.renderTexture(textures[Made_by_KC], s_width / 3 + 10, 10);
		break;
	case menu_state:																												//Narysuj Logo
		Window.renderTexture(textures[Achtung], 2 * s_width / 3 - textures[Achtung].w / 2 + 10, 3 * s_height / 8 - textures[Achtung].h / 2 + 5, 0, 0, { 100,100,100,0 });
		Window.renderTexture(textures[Achtung], 2 * s_width / 3 - textures[Achtung].w / 2, 3 * s_height / 8 - textures[Achtung].h / 2);
		break;
	case start_state:
		break;
	case options_state:
		for (int i = 0; i <= player.size(); i++) {
			Window.drawLine(s_width / 3, i*((s_height - (20 + textures[Name].h)) / 6) + 20 + textures[Name].h, 2 * s_width / 3, WindowC::Line::horizontal, { 255,255,255,255 });
		}
		Window.drawLine(s_width - 1, 0, player.size()*s_height / 6, WindowC::Line::vertical, { 255,255,255,255 });					//Rysowanie tabelki
		Window.drawLine(s_width / 3, 0, 2 * s_width / 3, WindowC::Line::horizontal, { 255,255,255,255 });							
		Window.drawLine(s_width / 3, 1, 2 * s_width / 3, WindowC::Line::horizontal, { 255,255,255,255 });

		for (int i = 4; i < button.size(); i++) {
			button[i].check(mx, my);
			button[i].draw();
		}

		for (int i = 0; i < player.size(); i++) {
			player[i].drawbuttons(mx, my);
		}
																																	//Rysowanie tabelki
		Window.renderTexture(textures[Name], s_width / 3 + 0.125*(2 * s_width / 3) - textures[Name].w / 2, 10);
		Window.drawLine(s_width / 3 + 0.25*(2 * s_width / 3), 0, player.size()*((s_height - (20 + textures[Name].h)) / 6) + 20 + textures[Name].h, WindowC::Line::vertical, { 255,255,255,255 });
		Window.renderTexture(textures[Left], s_width / 3 + 0.325*(2 * s_width / 3) - textures[Left].w / 2, 10);
		Window.drawLine(s_width / 3 + 0.4*(2 * s_width / 3), 0, player.size()*((s_height - (20 + textures[Name].h)) / 6) + 20 + textures[Name].h, WindowC::Line::vertical, { 255,255,255,255 });
		Window.renderTexture(textures[Right], s_width / 3 + 0.475*(2 * s_width / 3) - textures[Right].w / 2, 10);
		Window.drawLine(s_width / 3 + 0.55*(2 * s_width / 3), 0, player.size()*((s_height - (20 + textures[Name].h)) / 6) + 20 + textures[Name].h, WindowC::Line::vertical, { 255,255,255,255 });
		Window.renderTexture(textures[Color], s_width / 3 + 0.65*(2 * s_width / 3) - textures[Color].w / 2, 10);
		Window.drawLine(s_width / 3 + 0.75*(2 * s_width / 3), 0, player.size()*((s_height - (20 + textures[Name].h)) / 6) + 20 + textures[Name].h, WindowC::Line::vertical, { 255,255,255,255 });
		Window.drawLine(s_width / 3, 20 + textures[Name].h, 0.75*(2 * s_width / 3), WindowC::Line::horizontal, { 255,255,255,255 });

																																	//Zmiana koloru gracza
		if (colorchange) {																											//za pomoc¹ suwaków
			for (int i = 0; i < 3; i++) {
				slider[i].draw(&Window, &color);
				slider[i].check(mx, my);
				slider[i].set();
			}
			player[playercolorindex].changecolor(color);
			player[playercolorindex].colorbutton->changecolor(color);
		}
		break;

	case exit_state:
		gameloop = false;
		break;

	}
}

void GameC::handleevents() {
	while (SDL_PollEvent(&e) != 0) {
		switch (e.type)
		{
		case SDL_QUIT:
			exit(0);
			state = exit_state;
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (textinput == true && !player[playertextindex].namebutton->isfocused()) {
				textinput = false;
				player[playertextindex].changename(player[playertextindex].namebutton->getname());
			}
		case SDL_KEYDOWN:
			switch (e.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				if (state == start_state) state = menu_state;
				else state = exit_state;
				break;
			case SDLK_KP_ENTER:
			case SDLK_RETURN:
				if (textinput == true) {
					textinput = false;
					player[playertextindex].changename(player[playertextindex].namebutton->getname());
				}
				break;
			}
			break;
		}
		handlebuttons();
		handlesliders();
		handleplayers();
	}
}

void GameC::handlebuttons() {
	if (state != start_state) for (int i = 0; i < 4; i++)button[i].handle(e);
	if (state == options_state) {
		for (int i = 4; i < button.size(); i++) button[i].handle(e);
		for (int i = 0; i < player.size(); i++) player[i].handlebuttons(e);
	}
}

void GameC::handlesliders() {
	if (state == options_state) for (int i = 0; i < 3; i++) slider[i].handle(e);
}

void GameC::handleplayers() {
	if (state == start_state) for (int i = 0; i < playersalive.size(); i++) player[playersalive[i]].handle(e);
}

void GameC::ingame()
{

	if (WDecrease) {																							//PowerUp zmniejszania planszy
		WDecreasetimer++;
		if (WDecreasetimer % 2) {
			playRect.x++;
			playRect.y++;
			playRect.w -= 2;
			playRect.h -= 2;
			if (playRect.w < 0.35*actualplayRect.w) {
				WDecrease = false;
				WDecreasetimer = 0;
			}
		}
	}


	for (int i = 0; i < playersalive.size(); i++) {
		if (player[playersalive[i]].isalive()) {																//Je¿eli gracz jest ¿ywy
			for (int j = 0; j < powerup.size(); ) {
				if (powerup[j].check(player[playersalive[i]].getx(), player[playersalive[i]].gety())) {			//SprawdŸ kolizje z PowerUpem
					powerup[j].active(playersalive[i]);															//Aktywuj PowerUpa dla gracza
					powerup[j].del(background);
					redraw(powerup[j].getx(), powerup[j].gety(), powerup[j].getr() + 5);
					powerup.erase(powerup.begin() + j);
				}
				else j++;

			}
			if (player[playersalive[i]].getjump()) {															//Je¿eli gracz jest podczas skoku/niesmiertelnosci
				player[playersalive[i]].drawErase(background);													//wymazuj jego slad
				redraw(player[playersalive[i]].getx(), player[playersalive[i]].gety(), player[playersalive[i]].getthick());
			}
			player[playersalive[i]].move();
			player[playersalive[i]].draw();
		}
		else {																									//Je¿eli gracz zgin¹³
			player[playersalive[i]] += player.size() - playersalive.size();										//Dodaj punkty zale¿ne od kolejnoœci
			playersalive.erase(playersalive.begin() + i);														//Usuñ gracza z ¿ywych
			if (playersalive.size() == 1) player[playersalive.back()] += player.size() - playersalive.size();
		}
	}


	if (playersalive.size() < 2) {																				//Je¿eli zosta³ 1 gracz, zresetuj ture
		WDecreasetimer = 0;
		WDecrease = false;
		sortedplayers = sort();
		resetplayers();
		PlayerC::resetboard();
		flipanimation();
		powerup.clear();
	}

	if (SDL_GetTicks() > PowerUpTimer && rand() % 2) addPowerUp();												//Dodaj PowerUp

	ingameClear();																								//Wyczyœæ wszystko poza plansz¹ gry

	for (int i = 0; i < player.size(); i++) {																	//Drukuj nazwy i punkty graczy
		Window.drawRect(actualplayRect.x - maxnamew - s_width/20, actualplayRect.y + actualplayRect.h / 3 + i*(playernames[0].h + 10), actualplayRect.x-(actualplayRect.x - maxnamew - s_width / 20), playernames[0].h, WindowC::Rec::filled, player[sortedplayers[i]].getcolor());
		Window.drawnumber(actualplayRect.x - s_width / 25, actualplayRect.y + actualplayRect.h / 3 + i*(playernames[0].h + 10), player[sortedplayers[i]].getpkt());
		Window.renderTexture(playernames[sortedplayers[i]], actualplayRect.x - s_width / 20 -playernames[sortedplayers[i]].w, actualplayRect.y + actualplayRect.h / 3 + i*(playernames[0].h + 10) + 1, 0, 0, { 0,0,0,0 });
		Window.renderTexture(playernames[sortedplayers[i]], actualplayRect.x - s_width / 20 - playernames[sortedplayers[i]].w, actualplayRect.y + actualplayRect.h / 3 + i*(playernames[0].h + 10));
	}

	int delay = 15 - (SDL_GetTicks() - Timer);
	if (delay < 0)delay = 0;
	SDL_Delay(delay);
	Window.drawnumber(1, 1, int(SDL_GetTicks() - Timer));
	Timer = SDL_GetTicks();
}

void GameC::ingameClear() {
	Window.drawRect(playRect.x, playRect.y, playRect.w, playRect.h, WindowC::Rec::empty);
	Window.drawRect(0, 0, playRect.x, s_height, WindowC::Rec::filled, background);
	Window.drawRect(playRect.x, 0, s_width - playRect.x, playRect.y, WindowC::Rec::filled, background);
	Window.drawRect(playRect.x, playRect.y + playRect.h, s_width - playRect.x, s_height - playRect.y, WindowC::Rec::filled, background);
	Window.drawRect(playRect.x + playRect.w, playRect.y, s_width - (playRect.x + playRect.w), playRect.h, WindowC::Rec::filled, background);
}

void GameC::flipanimation() {
	TextureC texture = Window.CreateTextureFromWindow();
	if (texture.Texture == 0) {
		Window.drawRect(playRect.x, playRect.y, playRect.w, playRect.h, WindowC::Rec::filled, { 0,0,0,0 });
		Window.drawRect(playRect.x, playRect.y, playRect.w, playRect.h, WindowC::Rec::empty);
		return;
	}
	SDL_Rect onSprite = playRect;
	int x = playRect.x;
	int y = playRect.y;
	SDL_Rect animation = { x,y,onSprite.w,onSprite.h };
	while (animation.w > 0) {
		Window.renderSpriteTexture(texture, animation.x, animation.y, animation.w, animation.h, onSprite);
		animation.x++;
		animation.w -= 2;
		animation.y++;
		animation.h -= 2;
		SDL_Delay(2);
		Window.drawRect(animation.x, animation.y, animation.w, animation.h, WindowC::Rec::empty, { 255,255,255,255 });
		Window.screenUpdate();
		Window.drawRect(onSprite.x - 2, onSprite.y - 2, onSprite.w + 4, onSprite.h + 4, WindowC::Rec::filled, background);
	}
	playRect = { s_width - (s_height / 18) - (s_height - (2 * s_height / 18)) - s_width / 15,s_height / 18,s_height - (2 * s_height / 18),s_height - (2 * s_height / 18) };
	Window.drawRect(playRect.x, playRect.y, playRect.w, playRect.h, WindowC::Rec::filled, { 0,0,0,0 });
	Window.drawRect(playRect.x, playRect.y, playRect.w, playRect.h, WindowC::Rec::empty);
	for (int i = 0; i < playersalive.size(); i++) player[playersalive[i]].draw();
	SDL_DestroyTexture(texture.Texture);
	animation.w = 1;
	texture = Window.CreateTextureFromWindow();
	Window.drawRect(playRect.x, playRect.y, playRect.w, playRect.h, WindowC::Rec::filled, { 0,0,0,0 });
	onSprite = playRect;
	while (animation.x > onSprite.x) {
		Window.renderSpriteTexture(texture, animation.x, animation.y, animation.w, animation.h, onSprite);
		Window.drawRect(animation.x, animation.y, animation.w, animation.h, WindowC::Rec::empty);
		animation.x--;
		animation.w += 2;
		animation.y--;
		animation.h += 2;
		SDL_Delay(2);
		Window.drawRect(animation.x, animation.y, animation.w, animation.h, WindowC::Rec::empty, { 255,255,255,255 });
		Window.screenUpdate();
		Window.drawRect(onSprite.x - 2, onSprite.y - 2, onSprite.w + 4, onSprite.h + 4, WindowC::Rec::filled, background);
	}
	SDL_DestroyTexture(texture.Texture);
}

void GameC::resetplayers() {

	PowerUpTimer = SDL_GetTicks() + 4000;
	PlayerC::resetboard();
	playersalive.clear();
	for (int i = 0; i < player.size(); i++) {
		playersalive.push_back(i);
		player[i].reset(rand() % (playRect.w - playRect.w / 5) + playRect.x + playRect.w / 10, rand() % (playRect.h - playRect.h / 5) + playRect.y + playRect.h / 10);
	}
}

void GameC::redraw(int x, int y, int r) {
	int xmax = x + r > playRect.x + playRect.w ? playRect.x + playRect.w : x + r;
	int ymax = y + r > playRect.y + playRect.h ? playRect.y + playRect.h : y + r;
	for (int i = x - r < playRect.x ? playRect.x : x - r; i < xmax; i++) {
		for (int j = y - r < playRect.y ? playRect.y : y - r; j < ymax; j++) {
			if (PlayerC::board[i][j] > 0) {																		//Je¿eli na mapie by³ gracz drukuj go ponownie
				player[PlayerC::board[i][j] % 10 - 1].redraw(PlayerC::board[i][j] / 10, i, j);					//Wybierz numer gracza, oraz jego gruboœæ na podstawie mapy
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//																									//
//																									//
///								    BUTTON FUNCTIONS START										    //
//																									//
//																									//
//////////////////////////////////////////////////////////////////////////////////////////////////////


void GameC::addplayer() {
	if (player.size() < 6) {
		int x, y, w, h;
		player.emplace_back(PlayerC(0, 0, 0, SDLK_a, SDLK_d, { 255,255,255,255 }, player.size() + 1));
																											
		x = s_width / 3 + 0.125*(2 * s_width / 3);																//Inicjalizacja przyciskow w opcjach`  
		y = 20 + textures[Name].h + (player.size() - 1)*(s_height - (20 + textures[Name].h)) / 6 + (s_height - (20 + textures[Name].h)) / 12;
		w = textures[Name].w * 2;
		h = textures[Name].h * 4 / 3;
		player.back().namebutton = new TextboxC({ x - w / 2, y - h / 2, w, h }, "Name", { 0,0,0,0 });
		player.back().namebutton->fun = &GameC::buttonchangename;

		x = s_width / 3 + 0.325*(2 * s_width / 3);
		w = textures[Left].w / 2;
		h = textures[Left].w / 2;
		player.back().leftbutton = new ButtonC({ x - w / 2, y - h / 2, w, h }, "A", { 125,125,125,255 }, { 105,105,105,255 }, { 90,90,90,255 }, ButtonC::ButtonType::plain);
		player.back().leftbutton->fun = &GameC::buttonchangeleftb;

		x = s_width / 3 + 0.475*(2 * s_width / 3);
		w = textures[Left].w / 2;
		h = textures[Left].w / 2;
		player.back().rightbutton = new ButtonC({ x - w / 2, y - h / 2, w, h }, "D", { 125,125,125,255 }, { 105,105,105,255 }, { 90,90,90,255 }, ButtonC::ButtonType::plain);
		player.back().rightbutton->fun = &GameC::buttonchangerightb;

		x = s_width / 3 + 0.65*(2 * s_width / 3);
		w = textures[Color].w;
		h = textures[Color].w;
		player.back().colorbutton = new ColorboxC({ x - w / 2, y - h / 2, w, h }, player.back().getcolor());
		player.back().colorbutton->fun = &GameC::buttonchangecolor;
	}
}

void GameC::deleteplayer() {
	if (player.size() > 2) {
		if (playertextindex >= player.size() - 1) playertextindex--;

		if (playercolorindex >= player.size() - 1) {
			playercolorindex--;
			colorchange = false;
		}

		player.pop_back();

	}
}

void GameC::buttonchangename() {
	for (int i = 0; i < 6; i++) {
		if (my < (i + 1)*((s_height - (20 + textures[Name].h)) / 6) + 20 + textures[Name].h) {
			playertextindex = i;
			SDL_StartTextInput();
			player[i].namebutton->changetexture(player[i].namebutton->getname());
			textinput = true;
			break;
		}
	}
}

void GameC::buttonchangeleftb() {
	for (int i = 0; i < 6; i++) {
		if (my < (i + 1)*((s_height - (20 + textures[Name].h)) / 6) + 20 + textures[Name].h) {
			bool wait = true;
			Window.drawRect(player[i].leftbutton->getx(), player[i].leftbutton->gety(), player[i].leftbutton->getw(), player[i].leftbutton->geth(), WindowC::Rec::filled, player[i].leftbutton->getcolor());
			Window.screenUpdate();
			std::string scancode;
			while (SDL_WaitEvent(&e) && wait) {
				switch (e.type) {
				case SDL_KEYDOWN:
					player[i].changeleftb(e.key.keysym.sym);
					scancode = SDL_GetKeyName(player[i].getleftb());
					player[i].leftbutton->changetexture(scancode);
					wait = false;
					break;
				case SDL_MOUSEBUTTONDOWN:
					wait = false;
					break;
				}
			}
			break;
		}
	}
}


void GameC::buttonchangerightb() {
	for (int i = 0; i < 6; i++) {
		if (my < (i + 1)*((s_height - (20 + textures[Name].h)) / 6) + 20 + textures[Name].h) {
			bool wait = true;
			Window.drawRect(player[i].rightbutton->getx(), player[i].rightbutton->gety(), player[i].rightbutton->getw(), player[i].rightbutton->geth(), WindowC::Rec::filled, player[i].rightbutton->getcolor());
			Window.screenUpdate();
			std::string scancode;
			while (SDL_WaitEvent(&e) && wait) {
				switch (e.type) {
				case SDL_KEYDOWN:
					player[i].changerightb(e.key.keysym.sym);
					scancode = SDL_GetKeyName(player[i].getrightb());
					player[i].rightbutton->changetexture(scancode);
					wait = false;
					break;
				case SDL_MOUSEBUTTONDOWN:
					wait = false;
					break;
				}
			}
			break;
		}
	}
}

void GameC::buttonchangecolor() {
	for (int i = 0; i < 6; i++) {
		if (my < (i + 1)*((s_height - (20 + textures[Name].h)) / 6) + 20 + textures[Name].h) {
			if (playercolorindex != i)
			{
				int y = player[i].colorbutton->gety();
				int h = player[i].colorbutton->geth();
				slider[0].changexy(-1, y);
				slider[1].changexy(-1, y + h / 2 - 10);
				slider[2].changexy(-1, y + h - 20);

				RGBtoHSV(player[i].getcolor());
				color = player[i].getcolor();
				playercolorindex = i;
				colorchange = true;
			}
			break;
		}
	}
}

void GameC::buttonstart()
{
	PowerUpTimer = SDL_GetTicks() + 5000;
	maxnamew = 0;
	resetplayers();
	sortedplayers = sort();
	Window.changefont("ScribbletasticBrush.ttf", 10);
	playersalive.clear();
	for (int i = 0; i < player.size(); i++) {
		player[i].resetpkt();
		playersalive.push_back(i);
		SDL_DestroyTexture(playernames[i].Texture);
		playernames[i] = Window.loadTextTexture(player[i].getname().c_str(), { 255,255,255,255 });
		maxnamew = playernames[i].w > maxnamew ? playernames[i].w : maxnamew;
	}
	Window.changefont("FreeSans.ttf", 14);
	state = start_state;
	Window.screenClear();
	//player[0].setjumpcount(166000);
}

void GameC::buttonoptions()
{
	state = options_state;
}

void GameC::buttoncredits()
{
	state = credits_state;
}

void GameC::buttonexit()
{
	state = exit_state;
}

void GameC::savesettings() {
	fstream file("settings.txt", ios::out | ios::trunc);
	try {
		if (!file.is_open()) throw 1;
		file << player.size() << endl;
		for (int i = 0; i < player.size(); i++) {
			file << player[i] << endl;
		}
		file.close();
	}
	catch (int e) {
		SDL_ShowSimpleMessageBox(NULL, "Error", "Wystapil blad przy otwarciu pliku settings.txt\nOpcje nie zostaly zapisane", NULL);
		return;
	}
}

void GameC::loadsettings() {
	fstream file("settings.txt");
	int size;
	try {
		if (!file.is_open()) throw 1;
		file >> size;
		if (size <= 0 || size > 6) throw 2;
		for (size; size > 0; size--) {
			addplayer();
			file >> player.back();
		}
		file.close();
	}
	catch (int e) {
		switch (e) {
		case 1:
			SDL_ShowSimpleMessageBox(NULL, "Error", "Wystapil blad przy otwarciu pliku settings.txt\nOpcje nie zostaly wczytane", NULL);
			break;
		case 2:
			SDL_ShowSimpleMessageBox(NULL, "Error", "Wystapil blad. Liczba graczy podana w pliku jest bledna", NULL);
			break;
		}
		addplayer();
		addplayer();
		return;
	}
}




//////////////////////////////////////////////////////////////////////////////////////////////////////
//																									//
//																									//
///								    RANDOM FUNCTIONS START										    //
//																									//
//																									//
//////////////////////////////////////////////////////////////////////////////////////////////////////


void GameC::RGBtoHSV(SDL_Color color) {
	double r = color.r / 255.0, g = color.g / 255.0, b = color.b / 255.0;
	double Cmax = r > g ? r : g;
	Cmax = Cmax > b ? Cmax : b;
	double Cmin = r < g ? r : g;
	Cmin = Cmin < b ? Cmin : b;
	double delta = Cmax - Cmin;
	float percent;
	if (delta == 0) percent = 0;
	else if (Cmax == r) {
		percent = fmodf((g - b) / delta, 6) * 60 / 360.0;
	}
	else if (Cmax == g) {
		percent = (((b - r) / delta) + 2) * 60 / 360.0;
	}
	else if (Cmax == b) {
		percent = (((r - g) / delta) + 4) * 60 / 360.0;
	}
	if (percent < 0) percent += 1;
	slider[0].setthumb(percent);

	if (Cmax == 0) percent = 0;
	else percent = (delta / Cmax);
	slider[1].setthumb(percent);
	percent = Cmax;
	slider[2].setthumb(percent);

}

int * GameC::sort() {
	int * sorted = new int[100];
	for (int i = 0; i < player.size(); i++) {
		sorted[i] = 0;
	}
	int max = -1, j;
	bool tak = true;
	for (int i = 0; i < player.size(); i++)
	{
		for (j = 0; j < player.size(); j++)
		{
			for (int z = 0; z < i; z++) {
				if (j == sorted[z]) tak = false;
			}
			if (tak == true) {
				if (player[j].getpkt() > max)
				{
					max = player[j].getpkt();
					sorted[i] = j;
				}
			}
			tak = true;
		}
		max = -1;
	}
	return sorted;
}

void GameC::drawvalue(SDL_Rect Rect, SDL_Color color)
{
	float r = color.r, g = color.g, b = color.b;
	const float rrate = r / float(Rect.w), grate = g / float(Rect.w), brate = b / float(Rect.w);
	for (int i = Rect.x; i < Rect.x + Rect.w; i++) {
		Window.drawLine(i, Rect.y + 1, Rect.h - 2, WindowC::Line::vertical, color);
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

SDL_Color& rgbfromval(SDL_Color& color, float percent) {
	float r = color.r, g = color.g, b = color.b;
	const float rrate = r / float(100), grate = g / float(100), brate = b / float(100);
	float nperc = 1 - percent;
	if (nperc < 0)nperc = 0;
	r -= rrate*nperc * 100;
	if (r < 0)r = 0;
	g -= grate*nperc * 100;
	if (g < 0)g = 0;
	b -= brate*nperc * 100;
	if (b < 0)b = 0;
	color.r = r;
	color.g = g;
	color.b = b;
	return color;
}

SDL_Color& rgbfromhue(SDL_Color &color, float percent) {
	SDL_Color c;
	float r, g, b;
	float ang = 360.*percent;
	if (ang < 60) {
		r = 255;
		g = 255 * (ang / 60.);
		b = 0;
	}
	else if (ang < 120) {
		r = 255 - 255 * (fmod(ang, 60) / 60.);
		g = 255;
		b = 0;
	}
	else if (ang < 180) {
		r = 0;
		g = 255;
		b = 255 * (fmod(ang, 60) / 60.);
	}
	else if (ang < 240) {
		r = 0;
		g = 255 - 255 * (fmod(ang, 60) / 60.);
		b = 255;
	}
	else if (ang < 300) {
		r = 255 * (fmod(ang, 60) / 60.);
		g = 0;
		b = 255;
	}
	else if (ang < 360) {
		r = 255;
		g = 0;
		b = 255 - 255 * (fmod(ang, 60) / 60.);
	}
	else {
		r = 255;
		g = 0;
		b = 0;
	}
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = 255;
	return c;
}
SDL_Color& rgbfromsat(SDL_Color& color, float percent) {
	float r = color.r, g = color.g, b = color.b;
	const float rrate = (255 - r) / float(100), grate = (255 - g) / float(100), brate = (255 - b) / float(100);
	float nperc = 1 - percent;
	if (nperc < 0)nperc = 0;
	r += rrate*nperc * 100;
	if (r < 0)r = 0;
	g += grate*nperc * 100;
	if (g < 0)g = 0;
	b += brate*nperc * 100;
	if (b < 0)b = 0;
	color.r = r;
	color.g = g;
	color.b = b;
	return color;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//																									//
//																									//
///								    POWERUP FUNCTIONS START										    //
//																									//
//																									//
//////////////////////////////////////////////////////////////////////////////////////////////////////

void GameC::addPowerUp() {
	SDL_Color color;
	int type = rand() % 15;
	if (type < 6) color = { 0, 255, 0, 255 };
	else if (type < 12) color = { 255, 0, 0, 255 };
	else color = { 0, 0, 255, 255 };
	powerup.emplace_back(PowerUpC(32, rand() % (playRect.w - 64) + 32 + playRect.x, rand() % (playRect.h - 64) + 32 + playRect.y, color));
	switch (type) {
	case 0:
		powerup.back().fun = &GameC::PowerUpSpeedUpMe;
		powerup.back().initTexture("SpeedUp");
		break;
	case 1:
		powerup.back().fun = &GameC::PowerUpSlowDownMe;
		powerup.back().initTexture("SlowDown");
		break;
	case 2:
		powerup.back().fun = &GameC::PowerUpThickUpMe;
		powerup.back().initTexture("ThickUp");
		break;
	case 3:
		powerup.back().fun = &GameC::PowerUpThickDownMe;
		powerup.back().initTexture("ThickDown");
		break;
	case 4:
		powerup.back().fun = &GameC::PowerUpImmortality;
		powerup.back().initTexture("Immortality");
		break;
	case 5:
		powerup.back().fun = &GameC::PowerUpManuverityMe;
		powerup.back().initTexture("Manuverity");
		break;


	case 6:
		powerup.back().fun = &GameC::PowerUpThickDownElse;
		powerup.back().initTexture("ThickDown");
		break;
	case 7:
		powerup.back().fun = &GameC::PowerUpThickUpElse;
		powerup.back().initTexture("ThickUp");
		break;
	case 8:
		powerup.back().fun = &GameC::PowerUpSlowDownElse;
		powerup.back().initTexture("SlowDown");
		break;
	case 9:
		powerup.back().fun = &GameC::PowerUpSpeedUpElse;
		powerup.back().initTexture("SpeedUp");
		break;
	case 10:
		powerup.back().fun = &GameC::PowerUpReverseControls;
		powerup.back().initTexture("ReverseControls");
		break;
	case 11:
		powerup.back().fun = &GameC::PowerUpManuverityElse;
		powerup.back().initTexture("Manuverity");
		break;


	case 12:
		powerup.back().fun = &GameC::PowerUpWindowDecrease;
		powerup.back().initTexture("WindowDecrease");
		break;
	case 13:
		powerup.back().fun = &GameC::PowerUpOpenWalls;
		powerup.back().initTexture("OpenWalls");
		break;
	case 14:
		powerup.back().fun = &GameC::PowerUpClearBoard;
		powerup.back().initTexture("ClearBoard");
		break;
	}
	PowerUpTimer = SDL_GetTicks() + rand() % 3000 + 2000;
	powerup.back().draw();

	redraw(powerup.back().getx(), powerup.back().gety(), powerup.back().getr() + 5);
}


void GameC::PowerUpSpeedUpMe(int n) {
	player[n].PowerUp(PlayerC::powerups::SpeedUp);
}

void GameC::PowerUpSpeedUpElse(int n) {
	for (int i = 0; i < playersalive.size(); i++) {
		if (playersalive[i] != n) player[playersalive[i]].PowerUp(PlayerC::powerups::SpeedUp);
	}
}

void GameC::PowerUpSlowDownMe(int n) {
	player[n].PowerUp(PlayerC::powerups::SlowDown);
}

void GameC::PowerUpSlowDownElse(int n) {
	for (int i = 0; i < playersalive.size(); i++) {
		if (playersalive[i] != n) player[playersalive[i]].PowerUp(PlayerC::powerups::SlowDown);
	}
}

void GameC::PowerUpThickUpMe(int n) {
	player[n].PowerUp(PlayerC::powerups::ThickUp);
}

void GameC::PowerUpThickUpElse(int n) {
	for (int i = 0; i < playersalive.size(); i++) {
		if (playersalive[i] != n) player[playersalive[i]].PowerUp(PlayerC::powerups::ThickUp);
	}
}

void GameC::PowerUpThickDownMe(int n) {
	player[n].PowerUp(PlayerC::powerups::ThickDown);
}

void GameC::PowerUpThickDownElse(int n) {
	for (int i = 0; i < playersalive.size(); i++) {
		if (playersalive[i] != n) player[playersalive[i]].PowerUp(PlayerC::powerups::ThickDown);
	}
}

void GameC::PowerUpReverseControls(int n) {
	for (int i = 0; i < playersalive.size(); i++) {
		if (playersalive[i] != n) player[playersalive[i]].PowerUp(PlayerC::powerups::ReverseControls);
	}
}

void GameC::PowerUpWindowDecrease(int n) {
	WDecrease = true;
}

void GameC::PowerUpOpenWalls(int n) {
	for (int i = 0; i < playersalive.size(); i++) {
		player[playersalive[i]].PowerUp(PlayerC::powerups::OpenWalls);
	}
}

void GameC::PowerUpImmortality(int n) {
	player[n].PowerUp(PlayerC::powerups::Immortality);
}

void GameC::PowerUpManuverityMe(int n) {
	player[n].PowerUp(PlayerC::powerups::Manuverity);
}

void GameC::PowerUpManuverityElse(int n) {
	for (int i = 0; i < playersalive.size(); i++) {
		if (playersalive[i] != n) player[playersalive[i]].PowerUp(PlayerC::powerups::Manuverity);
	}
}

void GameC::PowerUpClearBoard(int n) {
	PlayerC::resetboard();
	Window.screenClear();
	for (int i = 0; i < powerup.size(); i++) {
		powerup[i].draw();
	}
	for (int i = 0; i < player.size(); i++) {
		player[i].resetlastmoves();
	}

}



