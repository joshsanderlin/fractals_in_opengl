#ifndef WINDOW_CONTROLLER_H
#define WINDOW_CONTROLLER_H

#include <utility>
#include <iostream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <gl/glut.h> // include GLUT library header
#endif

#include "definitions.h"

using namespace std;

/* This class is responsible for keeping track of the zoom and translate level of the complex plane. It also keeps track of the GL worldview coords to complex plane mapping.
*/
class WindowController
{
	
private:
	int width, height;
	NUM_TYPE minX, maxX, minY, maxY; // complex plane boundaries
	NUM_TYPE stepX, stepY;
	
	void calculateSteps() {
		stepX = (maxX - minX)/(NUM_TYPE)width;
		stepY = (maxY - minY)/(NUM_TYPE)height;
	}
	
public:
	WindowController(int w, int h, NUM_TYPE x, NUM_TYPE X, NUM_TYPE y, NUM_TYPE Y) {
		minX = x;
		maxX = X;
		minY = y;
		maxY = Y;
		setWindowSize(w,h);
	}
	
	// Used to set the bounds on the complex plane
	void setBounds(NUM_TYPE x, NUM_TYPE X, NUM_TYPE y, NUM_TYPE Y) {
		minX = x;
		maxX = X;
		minY = y;
		maxY = Y;
		
		calculateSteps();
	}
	
	//Resize the window
	void setWindowSize(int w, int h) {
		width = w;
		height = h;
		
		glMatrixMode (GL_PROJECTION);
		glLoadIdentity();
		
		cout << "setting width:" << width << " height:" << height << endl;
		glOrtho(0, width, 0, height, ((GLfloat)-1), (GLfloat)1);
		
		calculateSteps();
	}
	
	//Re-center the window on the new x,y coordinates
	void recenter(int x, int y) {
		std::pair<NUM_TYPE,NUM_TYPE> mid = getCoords(x,y);
		NUM_TYPE midX = mid.first;
		NUM_TYPE midY = mid.second;
		NUM_TYPE radX = (maxX - minX)/2;
		NUM_TYPE radY = (maxY - minY)/2;
		NUM_TYPE minX = midX - radX;
		NUM_TYPE maxX = midX + radX;
		NUM_TYPE minY = midY - radY;
		NUM_TYPE maxY = midY + radY;			
		setBounds(minX,maxX,minY,maxY);		
	}
	
	//Here we transpose from GL worldview coords to complex plane coords
	std::pair<NUM_TYPE,NUM_TYPE> getCoords(int x, int y) {
		NUM_TYPE re = minX + x*stepX;
		NUM_TYPE im = minY + y*stepY;
		std::pair<NUM_TYPE,NUM_TYPE> p (re,im);
		return p;
	}
	
	//Zoom the fractal with the specified zoomFactor
	void zoom(NUM_TYPE zoomFactor) {
		NUM_TYPE midX = (minX + maxX)/2;
		NUM_TYPE midY = (minY + maxY)/2;
		NUM_TYPE radX = ((maxX - minX)*zoomFactor)/2;
		NUM_TYPE radY = ((maxY - minY)*zoomFactor)/2;
		NUM_TYPE minX = midX - radX;
		NUM_TYPE maxX = midX + radX;
		NUM_TYPE minY = midY - radY;
		NUM_TYPE maxY = midY + radY;			
		setBounds(minX,maxX,minY,maxY);
	}

	//Translate in the Y direction
	void translateY(NUM_TYPE direction) {
		NUM_TYPE lengthY = (maxY - minY);
		NUM_TYPE translateAmount = lengthY*0.2*direction;
		NUM_TYPE y = minY + translateAmount;
		NUM_TYPE Y = maxY + translateAmount;	
	
		setBounds(minX,maxX,y,Y);
	}

	// -1 for left, 1 for right
	void translateX(NUM_TYPE direction) {
		NUM_TYPE lengthX = (maxX - minX);
		NUM_TYPE translateAmount = lengthX*0.2*direction;
		NUM_TYPE x = minX + translateAmount;
		NUM_TYPE X = maxX + translateAmount;
	
		setBounds(x,X,minY,maxY);
	}

	int getWidth() {
		return width;
	}

	int getHeight() {
		return height;
	}



};

#endif
