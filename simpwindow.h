#ifndef __SIMPWINDOW_H__
#define __SIMPWINDOW_H__


#include <cstdlib>
#include <iostream>
#include <X11/Xlib.h>
#include <vector>

extern int global_width;
extern int global_height;

class simpWindow{

    Display* display;
    Window window;
    int screen, height , width;
	Pixmap pixmap;
    GC BlankGC;
    unsigned long colours[10];
    double xratio,yratio;


    static simpWindow* instance;

    simpWindow();
    
public:

    ~simpWindow();

    static simpWindow* getInstance();

    enum {White=0, Black, Red, Green, Blue, Cyan, Yellow, Magenta, Orange, Brown};

    void drawRectangle(int x, int y, int width, int height, GC context, int colour = Black);
    void drawFillRectangle(int x, int y, int width, int height, GC context, int colour = Black);
    void drawLine(int x1,int y1,int x2,int y2, GC context, int colour = Black);
    void drawText(int x, int y, std::string text, GC context, int colour = Black);
    void drawCircle(int x, int y, int width, int height, int angle1, int angle2,GC context, int colour = Black);
    void drawFillPolygon(GC context, std::vector<std::pair<int,int> > points,int shape,int mode, int colour = Black);

    void freeGC(GC context);
    void clear();
    
    void flush();
    void resize(XEvent* e);

    GC createGC(int valuemask,XGCValues* values);
    
    XEvent* getEvent();
	XEvent* getEventBlocking();
};

#endif
