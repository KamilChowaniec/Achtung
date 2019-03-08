#include <ostream>
#include "PlayerC.h"
#include <math.h>

TextureC PlayerC::circle = { nullptr,0,0 };
WindowC* PlayerC::Window = nullptr;
SDL_Rect* PlayerC::playRect = nullptr;
int **PlayerC::board = nullptr;


int **PlayerC::makeboard(int x, int y) {
	if (!board) {
		board = new int*[x];
		for (int i = 0; i < x; i++) {
			board[i] = new int[y];
		}
	}
	return board;
}

PlayerC::PlayerC(int x, int y, int dir, SDL_Keycode l, SDL_Keycode r, SDL_Color c, int num) : x(x), y(y), direction(dir), left(l), right(r), color(c)
{
	resetlastmoves();
	manuvertimer = 0;
	manuver = false;
	immortal = false;
	walls = false;
	trueleft = l;
	trueright = r;
	thick = 8;
	jumpcount = 1600;
	jumpmax = rand() % 300 + 80;
	speed = 2;
	jump = true;
	pkt = 0;
	direction = rand() % 360;
	number = num;
	alive = true;
	name = "player";
	name.push_back(char(number + 48));
}

void PlayerC::changedir(char znak) {
	if (znak == '+') {
		if (manuver) direction += 90.;
		else direction += 1.5 + speed / 2.0;
	}
	else if (znak == '-') {
		if (manuver) direction -= 90.;
		else direction -= 1.5 + speed / 2.0;
	}

	if (direction >= 360) direction -= 360;
	else if (direction < 0) direction += 360;
}

void PlayerC::reset(int x, int y)
{
	manuverleftactive = false;
	manuverrightactive = false;
	leftactive = false;
	rightactive = false;
	resetlastmoves();
	manuvertimer = 0;
	manuver = false;
	immortal = false;
	walls = true;
	left = trueleft;
	right = trueright;
	PlayerC::x = x;
	PlayerC::y = y;
	direction = rand() % 360;
	alive = true;
	jump = true;
	jumpcount = 1600;
	speed = 2;
	activePowerUp.clear();
	thick = 8;
}

void PlayerC::draw()
{
	Window->renderTexture(circle, int(x) - thick / 2, int(y) - thick / 2, thick, thick, color);

}

void PlayerC::drawErase(SDL_Color c)
{
	Window->renderTexture(circle, int(x) - thick / 2 - 1, int(y) - thick / 2 - 1, thick + 2, thick + 2, c);
}

void PlayerC::redraw(int thick, int x, int y)
{
	thick = getthicknessfromlevel(thick);
	Window->renderTexture(circle, int(x) - thick / 2, int(y) - thick / 2, thick, thick, color);
}

void PlayerC::resetboard()
{
	for (int i = 0; i < Window->getscreenwidth(); i++) {
		for (int j = 0; j < Window->getscreenheight(); j++) {
			board[i][j] = 0;
		}
	}
}

void PlayerC::resetlastmoves() {
	for (int i = 0; i < 20; i++) last[i] = { 0,0 };
}

void PlayerC::PowerUp(powerups type)
{
	int time = 5000;
	switch (type) {
	case SpeedUp:
		activePowerUp.push_back({ SDL_GetTicks() + time,SpeedUp });
		speed++;
		break;
	case SlowDown:
		if (speed == 1) prolongPowerUp(SlowDown, time);
		else {
			activePowerUp.push_back({ SDL_GetTicks() + time,SlowDown });
			speed--;
		}
		break;
	case ThickUp:
		if (thick == 32) prolongPowerUp(ThickUp, time);
		else {
			activePowerUp.push_back({ SDL_GetTicks() + time,ThickUp });
			thick *= 2;
		}
		break;
	case ThickDown:
		if (thick == 2) prolongPowerUp(ThickDown, time);
		else {
			activePowerUp.push_back({ SDL_GetTicks() + time,ThickDown });
			thick /= 2;
		}
		break;
	case ReverseControls:
		activePowerUp.push_back({ SDL_GetTicks() + time,ReverseControls });
		swapcontrols();
		break;
	case OpenWalls:
		activePowerUp.push_back({ SDL_GetTicks() + time,OpenWalls });
		walls = true;
		break;
	case Immortality:
		if (immortal) prolongPowerUp(Immortality, time);
		else {
			activePowerUp.push_back({ SDL_GetTicks() + time,Immortality });
			immortal = true;
			jump = true;
		}
		break;
	case Manuverity:
		if (manuver) prolongPowerUp(Manuverity, time);
		else {
			activePowerUp.push_back({ SDL_GetTicks() + time,Manuverity });
			manuver = true;
			leftactive = false;
			rightactive = false;
		}
		break;
	}

}

void PlayerC::prolongPowerUp(powerups type, int time) {
	int j = 0;
	Uint32 maxtime = 0;
	for (int i = 0; i < activePowerUp.size(); i++) {
		if (activePowerUp[i].type == type) {
			if (activePowerUp[i].time > maxtime) {
				maxtime = activePowerUp[i].time > maxtime;
				j = i;
			}
		}
	}
	activePowerUp[j].time += time;
}

void PlayerC::handlepowerups() {
	for (int i = 0; i < activePowerUp.size(); ) {
		if (activePowerUp[i].time < SDL_GetTicks()) {
			int cx = 0, cy = 0;
			switch (activePowerUp[i].type) {
			case SpeedUp: if (speed > 1)speed--; break;
			case SlowDown: speed++; break;
			case ThickUp:
				if (thick > 2) {
					thick /= 2;
					getcollisionpixel(cx, cy, direction);
					x = cx;
					y = cy;
				}
				break;
			case ThickDown: if (thick < 32)thick *= 2; break;
			case ReverseControls: swapcontrols();  break;
			case OpenWalls: walls = false; break;
			case Immortality: immortal = false; jump = false; break;
			case Manuverity: manuver = false;
			}
			activePowerUp.erase(activePowerUp.begin() + i);
		}
		else i++;
	}

}

void PlayerC::swapcontrols() {
	SDL_Keycode tmp = left;
	left = right;
	right = tmp;
}

void PlayerC::handle(SDL_Event &e) {
	switch (e.type) {
	case SDL_KEYDOWN:
		if (e.key.keysym.sym == left) {
			if (manuver) {
				if (manuverleftactive) {
					manuverleftactive = false;
					changedir('+');
				}
			}
			else leftactive = true;
		}
		else if (e.key.keysym.sym == right) {
			if (manuver) {
				if (manuverrightactive) {
					manuverrightactive = false;
					changedir('-');
				}
			}
			else rightactive = true;
		}
		break;
	case SDL_KEYUP:
		if (e.key.keysym.sym == left) {
			leftactive = false;
			manuverleftactive = true;
		}
		if (e.key.keysym.sym == right) {
			rightactive = false;
			manuverrightactive = true;
		}
	}
}

void PlayerC::move() {

	Timer = SDL_GetTicks();
	handlepowerups();
	if (!immortal) {
		if (jump) {
			jumpcount -= 10;
			if (jumpcount < 0) {
				jump = false;
				jumpmax = rand() % 300 + 100;

			}
		}
		else {
			jumpcount++;
			if (jumpcount == jumpmax) {
				jumpcount = 200;
				jump = true;
			}
		}
	}
	for (int i = 19; i > 0; i--) {
		last[i].x = last[i - 1].x;
		last[i].y = last[i - 1].y;
	}
	if (jump) {
		last[0].x == 0;
		last[0].y = 0;
	}
	else {
		last[0].x = x;
		last[0].y = y;
	}

	x += sin(direction*M_PI / 180) * speed;
	y += cos(direction*M_PI / 180) * speed;

	outofmap();
	if (!jump) collision();

	if (leftactive) changedir('+');
	if (rightactive) changedir('-');
}

void PlayerC::collision() {

	if (outofmap() || pixelcollision()) {
		alive = false;  //end();
	}
	else {
		int b = thick / sqrt(8);
		b--;
		if (b < 1)b = 1;
		int maxx = last[0].x + b > playRect->x + playRect->w ? playRect->x + playRect->w - 1 : last[0].x + b;
		int maxy = last[0].y + b > playRect->y + playRect->h ? playRect->y + playRect->h - 1 : last[0].y + b;
		int miny = last[0].y - b < playRect->y ? playRect->y + 1 : last[0].y - b;
		int minx = last[0].x - b < playRect->x ? playRect->x + 1 : last[0].x - b;
		for (int i = minx; i < maxx; i++)
			for (int j = miny; j < maxy; j++)
				if (board[i][j] == 0) board[i][j] = -number * 10;
		board[int(x)][int(y)] = number + 10 * getthicknesslevel(thick);
		int slowness = 8 - speed + getthicknesslevel(thick);
		if (speed == 1) slowness += 7;
		maxx = last[slowness].x + b > playRect->x + playRect->w ? playRect->x + playRect->w - 1 : last[slowness].x + b;
		maxy = last[slowness].y + b > playRect->y + playRect->h ? playRect->y + playRect->h - 1 : last[slowness].y + b;
		miny = last[slowness].y - b < playRect->y ? playRect->y + 1 : last[slowness].y - b;
		minx = last[slowness].x - b < playRect->x ? playRect->x + 1 : last[slowness].x - b;
		for (int i = minx; i < maxx; i++)
			for (int j = miny; j < maxy; j++)
				if (board[i][j] < 1) board[i][j] = -1;
	}
}

int PlayerC::getthicknesslevel(int thick) {
	switch (thick) {
	case 2: return 0;
	case 4: return 1;
	case 8: return 2;
	case 16: return 3;
	case 32: return 4;
	}

}

int PlayerC::getthicknessfromlevel(int lvl) {
	switch (lvl) {
	case 0: return 2;
	case 1: return 4;
	case 2: return 8;
	case 3: return 16;
	case 4: return 32;
	}
}

void PlayerC::getcollisionpixel(int & cx, int & cy, float dir)
{
	cx = x + ((thick / 2) + 1) * sin((int)dir  * M_PI / 180);
	cy = y + ((thick / 2) + 1) * cos((int)dir *M_PI / 180);
}

bool PlayerC::pixelcollision() {
	int cx = 0, cy = 0;
	float dir = direction;
	getcollisionpixel(cx, cy, dir);
	if (cx<playRect->x || cx>playRect->x + playRect->w || cy<playRect->y || cy>playRect->y + playRect->h);
	else if (board[cx][cy] < 0 && board[cx][cy] != -number * 10) return true;

	dir = direction + 22.5;
	if (dir > 360)dir -= 360;
	getcollisionpixel(cx, cy, dir);
	if (cx<playRect->x || cx>playRect->x + playRect->w || cy<playRect->y || cy>playRect->y + playRect->h);
	else if (board[cx][cy] < 0 && board[cx][cy] != -number * 10) return true;

	dir = direction + 45;
	if (dir > 360)dir -= 360;

	getcollisionpixel(cx, cy, dir);
	if (cx<playRect->x || cx>playRect->x + playRect->w || cy<playRect->y || cy>playRect->y + playRect->h);
	else if (board[cx][cy] < 0 && board[cx][cy] != -number * 10) return true;

	dir = direction - 22.5;
	if (dir < 0)dir += 360;
	getcollisionpixel(cx, cy, dir);
	if (cx<playRect->x || cx>playRect->x + playRect->w || cy<playRect->y || cy>playRect->y + playRect->h);
	else if (board[cx][cy] < 0 && board[cx][cy] != -number * 10) return true;

	dir = direction - 45;
	if (dir < 0)dir += 360;
	getcollisionpixel(cx, cy, dir);
	if (cx<playRect->x || cx>playRect->x + playRect->w || cy<playRect->y || cy>playRect->y + playRect->h);
	else if (board[cx][cy] < 0 && board[cx][cy] != -number * 10) return true;

	return false;
}

bool PlayerC::outofmap() {
	if (x > playRect->x + playRect->w) {
		if (walls) x = playRect->x;
		else return true;
	}
	else if (x < playRect->x) {
		if (walls) x = playRect->x + playRect->w;
		else return true;
	}
	else if (y < playRect->y) {
		if (walls) y = playRect->y + playRect->h;
		else return true;
	}
	else if (y > playRect->y + playRect->h) {
		if (walls) y = playRect->y;
		else return true;
	}
	return false;

}



std::ostream & operator<<(std::ostream & os, const PlayerC & player)
{
	return os << (int)player.color.r << " " << (int)player.color.g << " " << (int)player.color.b << " " << player.left << " " << player.right << " " << (player.name.length() < 1 ? "Name" : player.name.c_str());
}

std::istream & operator >> (std::istream & is, PlayerC & player)
{
	int r, g, b, a;
	std::string s;
	is >> r >> g >> b;
	player.changecolor({ Uint8(r),Uint8(g),Uint8(b),Uint8(255) });
	player.colorbutton->changecolor({ Uint8(r),Uint8(g),Uint8(b),Uint8(255) });
	player.colorbutton->changeclickedcolor({ Uint8(r),Uint8(g),Uint8(b),Uint8(255) });
	is >> a;
	player.changeleftb(a);
	player.leftbutton->changetexture(SDL_GetKeyName(a));
	is >> a;
	player.changerightb(a);
	player.rightbutton->changetexture(SDL_GetKeyName(a));
	is.ignore(1);
	std::getline(is, s);
	player.changename(s);
	player.namebutton->changetexture(s);
	return is;
}


