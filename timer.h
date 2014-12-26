#ifndef __TIMER_H__
#define __TIMER_H__

#include <sys/time.h>
#include <map>
#include <string>


class Timer{
	unsigned long long int lastticked;
	int tps;

public:
	Timer(int tps);
	void settps(int tps);
	void sleepuntiltick();
};

class Cooldown{
	unsigned long long int lasttriggered;
	unsigned int cdus;

	static std::map<std::string,Cooldown> cooldowns;

	
public:
	Cooldown(int us = 1000000);
	static bool isReady(std::string name);
	static void start(std::string name,int us);
	static void reset(std::string name);
	static void changeTime(std::string name,int us);
	static void removeCooldowns();

};

#endif
