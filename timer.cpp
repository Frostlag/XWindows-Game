#include "timer.h"
#include  <sys/time.h>
#include <cstdlib>
#include <unistd.h>
#include <iostream>

using namespace std;

Timer::Timer(int tps):tps(tps){
	timeval timevalue;
	
	gettimeofday(&timevalue,NULL);
	lastticked = timevalue.tv_usec;
}

void Timer::settps(int tps){
	this->tps = tps;
}

void Timer::sleepuntiltick(){

	timeval timevalue;
	gettimeofday(&timevalue,NULL);
	unsigned long long int now = timevalue.tv_usec;
	if (now < lastticked){
		now += 1000000;
	}

	int timeelapsed = now - lastticked;
	
	
	double sleepamount = 1000000/tps;
	if (sleepamount<timeelapsed){ 
		sleepamount = 0;
	}
	else {
		sleepamount = sleepamount - timeelapsed;
	}
	usleep(sleepamount);

	gettimeofday(&timevalue,NULL);
	lastticked = timevalue.tv_usec;
}


map<string,Cooldown> Cooldown::cooldowns;

Cooldown::Cooldown(int us):cdus(us){
	timeval timevalue;
	gettimeofday(&timevalue,NULL);
	this->lasttriggered = (timevalue.tv_sec % 1000) * 1000000 + timevalue.tv_usec;
}

bool Cooldown::isReady(string name){
	Cooldown temp = Cooldown::cooldowns[name];
	timeval timevalue;
	gettimeofday(&timevalue,NULL);
	unsigned long long int now = (timevalue.tv_sec % 1000) * 1000000 + timevalue.tv_usec;
	if (now < temp.lasttriggered) now += 1000000000;
	if (now - temp.lasttriggered < temp.cdus) return false;
	return true;
}

void Cooldown::start(string name,int us){
	if (cooldowns.find(name) == cooldowns.end()) cooldowns.insert(pair<string,Cooldown> (name,Cooldown(us)));

}

void Cooldown::reset(string name){
	timeval timevalue;
	gettimeofday(&timevalue,NULL);
	Cooldown::cooldowns[name].lasttriggered = (timevalue.tv_sec % 1000) * 1000000 + timevalue.tv_usec;

}

void Cooldown::removeCooldowns(){


}
