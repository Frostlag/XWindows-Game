#include <cstdlib>
#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <cstring>
#include "simpwindow.h"
#include <unistd.h>
#include <vector>

using namespace std;

simpWindow* simpWindow::instance = NULL;

simpWindow* simpWindow::getInstance(){
    if (simpWindow::instance == NULL){
			try{
        simpWindow::instance = new simpWindow;
			}
			catch(char const* error){
				delete simpWindow::instance;
				throw;
			}
    }
    return simpWindow::instance;
}

simpWindow::simpWindow():height(global_height),width(global_width),xratio(1),yratio(1){

    display =  XOpenDisplay(NULL);

    if (display == NULL){
        throw("Cannot open display");
    }

    screen = DefaultScreen(display);
    window = XCreateSimpleWindow(
         display,                       // display where window appears
         DefaultRootWindow(display),    // window's parent
         10, 10,                        // location: x,y
         width, height,                      // size: width, height
         2,                             // width of border
         BlackPixel(display, screen),   // foreground colour
         WhitePixel(display, screen));  // background colour
    XSetStandardProperties(
        display,      // display containing the window
        window,       // window whose properties are set
        "Tower Defense",    // window's title
        "TD",               // icon's title
        None,               // pixmap for the icon
        NULL,0,        // applications command line args
        None ); 

    XMapRaised(display, window);        // put window on screen

    XFlush(display);                    // flush the output buffer
		
	sleep(1);
	XSelectInput(display,window,
			ButtonPressMask|
            ButtonReleaseMask|
            KeyReleaseMask|
	       	KeyPressMask|
			ExposureMask|
            StructureNotifyMask|
			ButtonMotionMask|
			PointerMotionMask);

    BlankGC = XCreateGC(display,window,0,0);
    int depth = DefaultDepth(display, DefaultScreen(display));
    pixmap = XCreatePixmap(display,window,2000,2000,depth);
    XSetForeground(display, BlankGC, WhitePixel(display,screen));
    XFillRectangle(display, pixmap, BlankGC, 0, 0, width, height);
	
    XAutoRepeatOff(display);

    XColor xcolour;
    Colormap cmap;
    char color_vals[10][10] = {"white", "black", "red", "green", "blue", "cyan", "yellow", "magenta", "orange", "brown"}; 
    cmap = DefaultColormap(display,screen);
    for (int i = 0; i < 10;i++){
    	XParseColor(display,cmap,color_vals[i],&xcolour);
	XAllocColor(display,cmap,&xcolour);
	colours[i] = xcolour.pixel;
    }
	
}

simpWindow::~simpWindow(){
    XFreePixmap(display,pixmap);
    freeGC(BlankGC);
    XCloseDisplay(display);
    
}

GC simpWindow::createGC(int valuemask,XGCValues* values){
    GC ret = XCreateGC(display,window,valuemask,values);
    return ret;
}

void simpWindow::drawText(int x, int y, std::string text, GC context, int colour){
	XSetForeground(display, context, colours[colour]);
    XDrawString(display, pixmap, context, x*xratio, y*yratio, text.c_str(), text.length());
    XSetForeground(display, context, colours[Black]);
}

void simpWindow::drawRectangle(int x, int y, int width, int height, GC context, int colour){
    XSetForeground(display, context, colours[colour]);
    XDrawRectangle(display, pixmap, context, x*xratio, y*yratio, width*xratio, height*yratio);
    XSetForeground(display, context, colours[Black]);
}

void simpWindow::drawFillRectangle(int x, int y, int width, int height, GC context, int colour){
    XSetForeground(display, context, colours[colour]);
    XFillRectangle(display, pixmap, context, x*xratio, y*yratio, width*xratio, height*yratio);
    XSetForeground(display, context, colours[Black]);
}

void simpWindow::drawLine(int x1,int y1,int x2,int y2, GC context, int colour){
    XSetForeground(display,context,colours[colour]);
    XDrawLine(display,pixmap,context,x1,y1,x2,y2);
    XSetForeground(display,context,colours[Black]);
}

void simpWindow::drawCircle(int x, int y, int width, int height, int angle1, int angle2,GC context, int colour){
    XSetForeground(display,context,colours[colour]);
    XFillArc(display,pixmap, context , x*xratio, y*yratio, width*xratio, height*yratio, angle1, angle2);
    XSetForeground(display,context,colours[Black]);
}

void simpWindow::drawFillPolygon(GC context, vector<pair<int,int> > points,int shape,int mode,int colour){
    XPoint xpoints [points.size()];
		for (unsigned int i = 0; i < points.size();i++){
			xpoints[i] = XPoint();
			xpoints[i].x = points[i].first*xratio;
			xpoints[i].y = points[i].second*yratio;
		}
    XSetForeground(display,context,colours[colour]);
    XFillPolygon(display,pixmap,context,xpoints,points.size(),shape,mode);
    XSetForeground(display,context,colours[Black]);
}

void simpWindow::clear(){
    XClearWindow(display,pixmap);
    XSetForeground(display, BlankGC, WhitePixel(display,screen));
    XFillRectangle(display, pixmap, BlankGC, 0, 0, width, height);
}

void simpWindow::flush(){
    XCopyArea(display,pixmap,window,BlankGC,0,0,width*xratio,height*yratio,0,0);
    XFlush(display);
    XSetForeground(display, BlankGC, WhitePixel(display,screen));
    XFillRectangle(display, pixmap, BlankGC, 0, 0, width*xratio, height*yratio);
}

XEvent* simpWindow::getEventBlocking(){
	XEvent* ret =  new XEvent;
	XNextEvent(this->display, ret);
	return ret;
}


XEvent* simpWindow::getEvent(){
	XEvent* ret =  new XEvent;

	if (XPending(this->display)>0){
		XNextEvent(this->display, ret);
        if (ret->type == MotionNotify){
            ret->xmotion.x /= xratio ;
            ret->xmotion.y /= yratio ;
        }
		return ret;
	}
	delete ret;
	return NULL;
}

void simpWindow::resize(XEvent* e){
    xratio = (double) e->xconfigure.width / width ; 
    yratio = (double) e->xconfigure.height/ height ;
}

void simpWindow::freeGC(GC context){
    XFreeGC(display,context);
}
