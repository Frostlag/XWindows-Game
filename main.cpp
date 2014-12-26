#include <cstdlib>
#include <iostream>
#include <X11/Xlib.h>
#include "simpwindow.h"
#include "timer.h"
#include "game.h"

using namespace std;


int global_height = 400;
int global_width = 600;

int main(){
	
	Game* theGame;
	try{
		//theGame = new Game(30,.1);
		theGame = new Game(30,.005);
	}
	catch(char const* error){
		cout << error << endl;
		exit(-1);
	}

	delete theGame;
	return 0;
}
