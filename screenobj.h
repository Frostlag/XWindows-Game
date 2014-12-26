#ifndef __SCREENOBJ_H__
#define __SCREENOBJ_H__

#include <vector>
#include <map>
#include "simpwindow.h"


extern int global_width,global_height;

class Asteroid;
class Wall;

class ScreenObj{

protected:
	simpWindow* theWindow;
	ScreenObj(int x, int y);



	GC context;

public:
	int x,y;
	double ax,ay;
	virtual void draw()=0;
	virtual ~ScreenObj()=0;
	
};

class Paddle:public ScreenObj{

	int height,width;
	double xspeed,yspeed;
	int speedmod;
	float speeddecay;
	Wall* wall;

	std::map<std::string, bool> keypressed;
	
public:
	Paddle(Wall* wall);
	~Paddle();
	void move();
	void keypress(std::string direction);
	void keyrelease(std::string direction);
	void tick();
	void fire(int x, int y);
	void draw();
	int collide(std::vector<Asteroid*> asteroids);
};

class Wall:public ScreenObj{
	int width,height;
	friend void Paddle::move();
public:

	Wall();
	~Wall();
	void draw();
};

class Bullet:public ScreenObj{
	int basespeed;
	double xspeed,yspeed;
	double xcenter,ycenter,radius;
	std::vector<std::pair<int,int> > points;
	int lifetime, maxlife;

public:
	
	Bullet(double angle,int xpos,int ypos,int maxlife);
	~Bullet();
	void draw();
	bool tick();
	int collide(std::vector<Asteroid*> asteroids);
};

class Asteroid:public ScreenObj{
	int basespeed,width;
	double xspeed,yspeed;

public:
	bool tick();
	Asteroid(int y);
	~Asteroid();
	void draw();
	bool hit();
	int collide(std::vector<Asteroid*> asteroids); 
	friend int Paddle::collide(std::vector<Asteroid*> asteroids);
	friend int Bullet::collide(std::vector<Asteroid*> asteroids);

};


#endif