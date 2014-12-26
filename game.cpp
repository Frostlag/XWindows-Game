#include "game.h"
#include "simpwindow.h"
#include <iostream>
#include <map>
#include <vector>
#include <cmath>
#include "screenobj.h"
#include <sstream>

using namespace std;

Game::Game(int tps,double chanceofast):timer(Timer(tps)),tps(tps),chanceofast(chanceofast){
	theWindow = simpWindow::getInstance();
	inSplash = true;
	inGame = false;

	keydict.insert(pair<int,pair<string, string> >(38,pair<string, string>("left","a")));
	keydict.insert(pair<int,pair<string, string> >(40,pair<string, string>("right","d")));
	keydict.insert(pair<int,pair<string, string> >(25,pair<string, string>("up","w")));
	keydict.insert(pair<int,pair<string, string> >(39,pair<string, string>("down","s")));
	keydict.insert(pair<int,pair<string, string> >(111,pair<string, string>("fast","up")));
	keydict.insert(pair<int,pair<string, string> >(116,pair<string, string>("slow","down")));
	keydict.insert(pair<int,pair<string, string> >(24,pair<string, string>("quit","q")));

	pressed["leftmouse"] = false;

	context = this->theWindow->createGC(0,0);

	splash();

	while (inSplash){
		XEvent* e = theWindow->getEventBlocking();
		this->handleSplash(e);
		delete e;
	}
	
	this->begin();

}

Game::~Game(){
	unsigned int i;
	for (i = 0; i < asteroids.size();i++){
		delete asteroids[i];
	}
	for (i = 0; i < bullets.size();i++){
		delete bullets[i];
	}
	delete paddle;
	delete wall;

	theWindow->freeGC(context);
	delete theWindow;
}



void Game::begin(){
	wall = new Wall;
	paddle = new Paddle(wall);
	Cooldown::start("fire",200000);
	inGame = true;
	srand(time(NULL));
	score  = 0;

	//this->theWindow->clear();
	this->gameLoop();

}

void Game::gameLoop(){
	XEvent* e;
	while(inGame){
		timer.sleepuntiltick();
		while ((e = theWindow->getEvent())) { 
			this->handleEvent(e);
		}

		paddle->tick();
		if (pressed["leftmouse"]){
			this->fire(mx,my);
		}

		int astindex = paddle->collide(asteroids);
		if (astindex != -1){
			score -= 10;
			delete asteroids[astindex];
			asteroids.erase(asteroids.begin()+astindex);
		}
		
		for (unsigned int i = 0;i<asteroids.size();i++){

			if (!asteroids[i]->tick()){
				score -= 10;
				delete asteroids[i];
				asteroids.erase(asteroids.begin()+i);
				i--;
			}
		}

		bool deleteboolet = false;

		for (unsigned int i = 0;i<bullets.size();i++){
			
			if (!bullets[i]->tick()){
				deleteboolet = true;
			}
			
			astindex = bullets[i]->collide(asteroids);
			
			if (astindex != -1){
				deleteboolet = true;
				score += 1;
				if (!asteroids[astindex]->hit()){
					
					delete asteroids[astindex];
					asteroids.erase(asteroids.begin()+astindex);
				}
			}

			if (deleteboolet){
				delete bullets[i];
				bullets.erase(bullets.begin()+i);
				i--;
			}

			deleteboolet = false;
		}



		double random = ((double) (rand() % 1000000))/1000000;
		
		if (random < chanceofast){
			this->generateAsteroid();
		}

		this->draw();
		//theWindow->clear();
	}
}

void Game::draw(){
		paddle->draw();
		wall->draw();
		for (unsigned int i = 0;i<asteroids.size();i++){
			asteroids[i]->draw();
		}
		for (unsigned int i = 0;i<bullets.size();i++){
			bullets[i]->draw();
		}
		drawScore();
		theWindow->flush();
}

void Game::drawScore(){
	ostringstream temp;
	temp << score;
	theWindow->drawText(100,10,temp.str(),context);
}

void Game::handleSplash(XEvent* e){
	switch (e->type){
		case KeyPress:
			inSplash = false;
			break;
		case 12:
			splash();
			break;

	}
}

void Game::handleEvent(XEvent* e){
	if ( e == NULL ) return;
	
	switch (e->type){
		case KeyPress:
			this->handleKeyPress(e);
			break;
		case KeyRelease:
			this->handleKeyRelease(e);
			break;
		case ConfigureNotify:
			this->handleResize(e);
			break;
		case MotionNotify:
			mx = e->xmotion.x;
			my = e->xmotion.y;
			//cout << e->xmotion.x << ":" << e->xmotion.y << endl;
			break;
		case ButtonPress:
			this->handleButtonPress(e);
			//cout << e->xbutton.x << ":" << e->xbutton.y << endl;
			break;
		case ButtonRelease:
			this->handleButtonRelease(e);
			//cout << e->xbutton.x << ":" << e->xbutton.y << endl;
			break;
	}

	delete e;
}

void Game::handleResize(XEvent* e){
	theWindow->resize(e);
}

void Game::handleKeyPress(XEvent* e){
	map<int,pair<string,string> >::iterator it =  keydict.find(e->xkey.keycode);
	if (it == keydict.end()) return;
	if (it->second.first == "fast" || it->second.first == "slow"){
		changeSpeed(it->second.first);
	}
	else if (it->second.first == "quit"){
		inGame = false;
	}
	else{
		paddle->keypress(it->second.first);
	}
}

void Game::handleKeyRelease(XEvent* e){
	map<int,pair<string,string> >::iterator it =  keydict.find(e->xkey.keycode);
	if (it == keydict.end()) return;
	paddle->keyrelease(it->second.first);	
}

void Game::handleButtonPress(XEvent* e){
	switch(e->xbutton.button){
		case 1:
			pressed["leftmouse"] = true;
			break;
	}
}

void Game::handleButtonRelease(XEvent* e){
	switch(e->xbutton.button){
		case 1:
			pressed["leftmouse"] = false;
			break;
	}
}

void Game::changeSpeed(string direction){
	if (direction == "fast"){
		tps++;
		timer.settps(tps);
	}
	else if (direction == "slow"){
		tps--;
		timer.settps(tps);
	}
	cout << tps << endl;
}

void Game::fire(int tx, int ty){
	if (!Cooldown::isReady("fire")) return;
	Cooldown::reset("fire");
	int xdist = tx - (paddle->x+5);
	int ydist = ty - (paddle->y+5);
	Bullet* bullet = new Bullet(atan2(ydist,xdist),paddle->x+5,paddle->y+5,3*tps);
	bullets.push_back(bullet);
}


void Game::splash(){
	
	string controls = "Controls are WASD to move your square defender,mouse to shoot from it.";	
 	string continuetext = "Press Q to quit when in game, and any key to continue.";
	string cheattext = "Up and down arrow keys are also available for changing the speed.";

	theWindow->drawText(100,100,"Dingtao Yin",context);
	theWindow->drawText(100,120,"d6yin",context);
	theWindow->drawText(100,140,controls,context);
	theWindow->drawText(100,160,continuetext,context);
	theWindow->drawText(100,180,cheattext,context);
	theWindow->flush();


}

void Game::generateAsteroid(){
	Asteroid* asteroid= new Asteroid(rand()%400);
	if (asteroid->collide(asteroids)!=-1){
		delete asteroid;
		return;
	}	
	asteroids.push_back(asteroid);
}


