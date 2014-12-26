#include "screenobj.h"
#include "simpwindow.h"
#include <iostream>
#include <map>
#include <vector>
#include <cmath>

using namespace std;

enum {White=0, Black, Red, Green, Blue, Cyan, Yellow, Magenta, Orange, Brown};

ScreenObj::ScreenObj(int x, int y):x(x),y(y),ax(x),ay(y){
	this->theWindow = simpWindow::getInstance();
	context =  theWindow->createGC(0,0);
}

Wall::Wall():ScreenObj(25,0),width(50),height(global_height-1){
	theWindow->freeGC(context);
	XGCValues values;
	values.line_width = 3;
	
	context = theWindow->createGC(GCLineWidth,&values);
}

void Wall::draw(){
	theWindow->drawRectangle(x,y,width,height,context);
	for( int i = 0; i <= height/10 ; i++)
		theWindow->drawRectangle(x+width-10,y+i*10,10,10,context);
}


Paddle::Paddle(Wall* wall):ScreenObj(100,350),height(10),width(10),xspeed(0),yspeed(0),speedmod(2),speeddecay(.80),wall(wall){
	keypressed["left"] = false;
	keypressed["right"] = false;
	keypressed["down"] = false;
	keypressed["up"] = false;
}

void Paddle::keypress(string direction){
	keypressed[direction] = true;
}

void Paddle::keyrelease(string direction){
	keypressed[direction] = false;
}

void Paddle::tick(){
	if (keypressed["up"]){
		yspeed -= speedmod;
	}
	if (keypressed["down"]){
		yspeed += speedmod;
	}
	if (keypressed["right"]){
		xspeed += speedmod;
	}
	if (keypressed["left"]){
		xspeed -= speedmod;
	}
	xspeed = (int)(xspeed * speeddecay);
	yspeed = (int)(yspeed * speeddecay);
	this->move();
}

void Paddle::move(){

	int xpot = x + xspeed;

	if (xpot < 25 + wall->width){
		x = 25 + wall->width;
	}
	else{
		x += xspeed;
	}
	y += yspeed;

	if (x < 0) x = 0;
	if (x + 10 > global_width) x = global_width - 10;
	if (y < 0) y = 0;
	if (y + 10 > global_height) y = global_height - 10;
}

void Paddle::draw(){
	theWindow->drawFillRectangle(this->x, this->y,this->width,this->height,context,Green);
}

int Paddle::collide(vector<Asteroid*> asteroids){
	double tempx = x + 5 ;
	double tempy = y + 5 ;
	for (unsigned int i = 0;i<asteroids.size();i++){
		if (sqrt(pow(asteroids[i]->x+asteroids[i]->width-tempx,2) + pow(asteroids[i]->y+asteroids[i]->width-tempy,2)) < asteroids[i]->width + 5){
			return i;
		}

	}
	return -1;
}


void mult( double** a, vector<pair<int,int> > &b){
	int temp0,temp1;
	for (unsigned int i = 0; i < b.size(); i++){
		temp0 = a[0][0] * b[i].first + a[0][1] * b[i].second;
		temp1 = a[1][0] * b[i].first + a[1][1] * b[i].second;
		b[i].first = temp0;
		b[i].second = temp1;							 
	}
}

double** createAngleArray(double angle){
	double** ret = new double*[2];
	ret[0] = new double[2];
	ret[1] = new double[2];
	ret[0][0] =  cos(angle); 
	ret[0][1] = -sin(angle);
	ret[1][0] =  sin(angle);
	ret[1][1] =  cos(angle);
	return ret;
}

Bullet::Bullet(double angle,int xpos,int ypos,int maxlife):ScreenObj(xpos,ypos),basespeed(7){
	ax = xpos;
	ay = ypos;
	this->maxlife = maxlife;
	this->lifetime = 0;
	points.push_back(pair<int,int>(0,0));
	points.push_back(pair<int,int>(5,0));
	points.push_back(pair<int,int>(5,5));
	points.push_back(pair<int,int>(0,5));
	xspeed = cos(angle) * basespeed;
	yspeed = sin(angle) * basespeed;

	radius = 2.5;
	
	double** anglearray = createAngleArray(angle);
	mult(anglearray,points);

	xcenter = points[0].first + points[2].first;
	ycenter = points[0].second + points[2].second;
	
	delete [] anglearray[0];
	delete [] anglearray[1];
	delete [] anglearray;

}

void Bullet::draw(){
	vector<pair<int,int> > temp;
	temp.resize(points.size());
	for (unsigned int i = 0;i<points.size();i++){
		temp[i].first  = ax + points[i].first;
		temp[i].second = ay + points[i].second;
	}
	theWindow->drawFillPolygon(context,temp,Convex,CoordModeOrigin);	
}

bool Bullet::tick(){
	ax += xspeed;
	ay += yspeed;
	x = (int) ax;
	y = (int) ay;
	lifetime++;

	if (lifetime > maxlife || x < 25 + 50 + 5){
		return false;
	}
	return true;
}

int Bullet::collide(vector<Asteroid*> asteroids){
	double tempx = xcenter + ax;
	double tempy = ycenter + ay;
	for (unsigned int i = 0;i<asteroids.size();i++){
		if (sqrt(pow(asteroids[i]->x+asteroids[i]->width/2-tempx,2) + pow(asteroids[i]->y+asteroids[i]->width/2-tempy,2)) < asteroids[i]->width/2 + radius){
			return i;
		}

	}
	return -1;
}

Asteroid::Asteroid(int):ScreenObj(global_width,y),basespeed(4){
	width = rand() % 60 + 10;
	ax = x;
	ay = y;
	int targety = rand() % 350 + 25;
	int diffy = targety - this->y;
	int diffx = -500;
	double anglediff = atan2(diffy,diffx);
	xspeed = cos(anglediff) * basespeed;
	yspeed = sin(anglediff) * basespeed;
	
}

void Asteroid::draw(){
	theWindow->drawCircle(x,y,width,width,0,360*64,context,Red);
}

bool Asteroid::tick(){
	ax += xspeed;
	ay += yspeed;
	x = (int) ax;
	y = (int) ay;
	if (x < 25 + 50){
		return false;
	}
	return true;
}

bool Asteroid::hit(){
	width -= 5;
	ax += 2.5;
	ax += 2.5;
	if (width < 20){
		return false;
	}
	return true;
}

int Asteroid::collide(vector<Asteroid*> asteroids){
	double tempx = ax+width/2;
	double tempy = ay+width/2;
	for (unsigned int i = 0;i<asteroids.size();i++){
		if (sqrt(pow(asteroids[i]->x+asteroids[i]->width/2-tempx,2) + pow(asteroids[i]->y+asteroids[i]->width/2-tempy,2)) < asteroids[i]->width/2 + this->width/2){
			return i;
		}
	}
	return -1;
}

ScreenObj::~ScreenObj(){theWindow->freeGC(context);}

Paddle::~Paddle(){}

Wall::~Wall(){}

Asteroid::~Asteroid(){}

Bullet::~Bullet(){}