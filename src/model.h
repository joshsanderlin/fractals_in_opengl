#ifndef MODEL_H
#define MODEL_H

#include "window_controller.h"

/*
State Model for the project, keeps track of various state variables among other things.
*/
class Model{
public:
	static void getInstance(Model&);

	WindowController* window;
	int windowID;
	int initialWidth, initialHeight; // window size
	
	static const int MANDELBROT_MODE = 0;
	static const int JULIA_MODE = 1;
	static const int JULIA_SIN_MODE = 2;
	int fractalMode;
	int selectedJulia;
	
	float juliaReal;
	float juliaImaginary;

private:
	Model()  //don't want anyone calling the default constructor so it's private
	{
		initialWidth = 600;
		initialHeight = 600;
		fractalMode = MANDELBROT_MODE;
		selectedJulia = 0;
	};
	
	Model(Model const&){}; //Copy-constructor is also private
	Model& operator=(Model const&); //assignment operator is also private
	static Model* instance; //The variable to hold our singleton instance
	
};

#endif
