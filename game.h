#ifndef __GAME_H__
#define __GAME_H__

#include "simpwindow.h"
#include "timer.h"
#include <vector>
#include <map>
#include "screenobj.h"



class Game{

	simpWindow* theWindow;
	bool inSplash,inGame;
	Timer timer;
	Paddle* paddle;
	std::vector<Asteroid*> asteroids;
	std::vector<Bullet*> bullets;
	Wall* wall;
	int tps,mx,my;
	double chanceofast;
	int score;
	GC context;

	std::map<int,std::pair<std::string, std::string> > keydict;
	std::map<std::string, bool> pressed;
		
	void draw();

	void splash();
	void handleKeyPress(XEvent* e);
	void handleKeyRelease(XEvent* e);
	void handleResize(XEvent* e);
	void handleButtonPress(XEvent* e);
	void handleButtonRelease(XEvent* e);
	void handleSplash(XEvent* e);
	void generateAsteroid();
	void changeSpeed(std::string direction);
	void drawScore();

	void fire(int tx, int ty);
public:
	Game(int tps,double chanceofast);
	~Game();
	void begin();
	void gameLoop();
	void handleEvent(XEvent* e);
};

#endif
