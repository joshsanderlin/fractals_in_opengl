#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <gl/glut.h> //include GLUT library header
#endif

#include <iostream>
#include <cmath>

#include "model.h"
#include "definitions.h"

using namespace std;

GLfloat black[] = {0.0f, 0.0f, 0.0f}; // black color
const int paletteSize = 32;
GLfloat palette[paletteSize][3];

const GLfloat radius = 5.0f;
bool fullScreen=false;

bool firstReshape = true;

Model* model;
WindowController* window;

//****************************************
GLfloat* calculateColorMandelbrot(NUM_TYPE u, NUM_TYPE v){
	NUM_TYPE re = u;
	NUM_TYPE im = v;
	NUM_TYPE tempRe=0.0;
	//The mandelbrot set of points are calculated by squaring the series and adding the original (u,v) coordinates to the complex number.
	for(int i=0; i < paletteSize; i++){
		tempRe = re*re - im*im + u;
		im = re * im * 2 + v;
		re = tempRe;
		if( (re*re + im*im) > radius ){
			return palette[i];
		}
	}
	return black;
}

//****************************************
GLfloat* calculateColorJuliaSin(NUM_TYPE x, NUM_TYPE y, NUM_TYPE cRe, NUM_TYPE cIm){
	NUM_TYPE re = x;
	NUM_TYPE im = y;
	NUM_TYPE tempRe=0.0;
	for(int i=0; i < paletteSize; i++){
		// The main difference in calculating julia vs. mandelbrot sets, is that the mandelbrot adds the original x or y (respectively for real and imaginary components) to the square, whereas the julia adds a static constant which determines it's shape.
		tempRe = re*re - im*im + cRe;
		im = re * im * 2 + cIm;
		re = tempRe;
		tempRe = sin(re) * cosh(im);
		im = cos(re) * sinh(re);
		
		//For julia set, if new point lies outside the circle with radius 2 (hence the 4)
		if( (re*re + im*im) > 4 ){
			return palette[i];
		}
	}
	return black;
}

//****************************************
GLfloat* calculateColorJulia(NUM_TYPE x, NUM_TYPE y, NUM_TYPE cRe, NUM_TYPE cIm){
	NUM_TYPE re = x;
	NUM_TYPE im = y;
	NUM_TYPE tempRe=0.0;
	for(int i=0; i < paletteSize; i++){
		// The main difference in calculating julia vs. mandelbrot sets, is that the mandelbrot adds the original x or y (respectively for real and imaginary components) to the square, whereas the julia adds a static constant which determines it's shape.
		tempRe = re*re - im*im + cRe;
		im = re * im * 2 + cIm;
		re = tempRe;
		//For julia set, if new point lies outside the circle with radius 2 (hence the 4)
		if( (re*re + im*im) > 4 ){
			return palette[i];
		}
	}
	return black;
}


// steps | div
//   32  |  4
//   64  |  2



//****************************************
// This method sets up an array of 128 (or any multiple of 4) colors, which are used in the calculateColor methods
void createPalette(){
	if(paletteSize % 4 != 0) {
		cout << "paletteSize is not a multiple of 4" << endl;
	}
	//4*i is really (128/steps)*i
	int steps = paletteSize/4;
	int halfSteps = 128/steps;
	int fullSteps = 256/steps;
	for(int i=0; i < steps; i++){
		palette[i][0] = (halfSteps*i)/(GLfloat)255;
		palette[i][1] = (128-halfSteps*i)/(GLfloat)255;
		palette[i][2] = (255-fullSteps*i)/(GLfloat)255;
	}
	for(int i=0; i < steps; i++){
		palette[32+i][0] = (GLfloat)1;
		palette[32+i][1] = (fullSteps*i)/(GLfloat)255;
		palette[32+i][2] = (GLfloat)0;
	}
	for(int i=0; i < steps; i++){
		palette[64+i][0] = (128-halfSteps*i)/(GLfloat)255;
		palette[64+i][1] = (GLfloat)1;
		palette[64+i][2] = (fullSteps*i)/(GLfloat)255;
	}
	for(int i=0; i < steps; i++){
		palette[96+i][0] = (GLfloat)0;
		palette[96+i][1] = (255-fullSteps*i)/(GLfloat)255;
		palette[96+i][2] = (fullSteps*i)/(GLfloat)255; 
	}
}

//****************************************
void repaint() {// function called to repaint the window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the screen buffer
	
	glBegin(GL_POINTS); // start drawing in single pixel mode

	//lookup our julia constants just once
	float jRe = model->juliaReal;
	float jIm = model->juliaImaginary;

	for(GLint y = window->getHeight(); y >= 0; y--){
		for(GLint x = 0; x <= window->getWidth(); x++){
			
			//Here we map from the complex plane to our worldview coords
			pair<NUM_TYPE,NUM_TYPE> ri = window->getCoords(x,y);
		
			if(model->fractalMode == Model::MANDELBROT_MODE) {
				glColor3fv(calculateColorMandelbrot(ri.first,ri.second)); // set color
			} else if(model->fractalMode == Model::JULIA_MODE) {
				glColor3fv(calculateColorJulia(ri.first,ri.second,jRe,jIm)); // set color
			} else if(model->fractalMode == Model::JULIA_SIN_MODE) {
				glColor3fv(calculateColorJuliaSin(ri.first,ri.second,jRe,jIm)); // set color
			}
			glVertex3f(x, y, 0.0f); // put pixel on screen (buffer) - [ 1 ]
		}
	}
	
	glEnd(); // end drawing
	glutSwapBuffers(); // swap the buffers - [ 2 ]
}

//****************************************
void reshape (int w, int h) { // function called when window size is changed
	cout << "Reshaping to new width:" << w << " and new height:" << h << endl;
	model->window->setWindowSize(w,h);
	glViewport (0, 0, (GLsizei)w, (GLsizei)h); // set new dimension of viewable screen
	glutPostRedisplay(); // repaint the window
}

int randomFractalsSize = 19;


//This array contains some interesting Julia Set fractals that we found while using the "random" julia set functionality (by hitting '3'). Here the Julia Sets are referenced by the real and imaginary parts of the constant C. 
pair<float,float> randomFractals[] = {
	pair<float,float>(-0.7, 0.27015),
	pair<float,float>(-0.835, -0.2321),
	pair<float,float>(0.285, 0.01),
	pair<float,float>(-0.70176, -0.3842),
	pair<float,float>(-0.4,0.6),
	pair<float,float>(-0.751442,-0.0747098),
	pair<float,float>(-0.692048, -0.253816),
	pair<float,float>(-1.03884, 0.214964),
	pair<float,float>(-0.743621,-0.0344564  ),
	pair<float,float>(-0.756475,-0.0707046  ),
	pair<float,float>(-0.179884,0.689243    ),
	pair<float,float>(0.391465,0.349051     ),
	pair<float,float>(-0.232621,-0.653532   ),
	pair<float,float>(-0.511001,0.608968    ),
	pair<float,float>(-1.34819,-0.0665871   ),
	pair<float,float>(-0.745417,-0.226992   ),
	pair<float,float>(0.349831,-0.385603    ),
	pair<float,float>(0.316808,-0.413749    ),
	pair<float,float>(-1.31481,0.0579096    )
};


//****************************************
void keyFunction(unsigned char key, int x, int y){ // function to handle key pressing
	int width, height;
	float LO = -1.5;
	float HI = 1.5;
	
	switch(key){
		case 'F': // pressing F is turning on/off fullscreen mode
		case 'f':
			if(fullScreen) {
				cout << "setting from keypress" << endl;
				window->setWindowSize(model->initialWidth, model->initialHeight);				
				glutReshapeWindow(window->getWidth(), window->getHeight()); // sets default window size
				GLsizei windowX = (glutGet(GLUT_SCREEN_WIDTH)-window->getWidth())/2;
				GLsizei windowY = (glutGet(GLUT_SCREEN_HEIGHT)-window->getHeight())/2;
				glutPositionWindow(windowX, windowY); // centers window on the screen
				fullScreen = false;
			}
			else {
				width = glutGet(GLUT_SCREEN_WIDTH);
				height = glutGet(GLUT_SCREEN_HEIGHT);
				window->setWindowSize(width, height);
				fullScreen = true;
				glutFullScreen(); // go to fullscreen mode
			}
			break;
			
		case 27 : // escape key - close the program
			glutDestroyWindow(model->windowID);
			exit(0);
			break;
			
		//The following four cases allow for zooming in on the fractal 
		case '+':
		case '=':
			window->zoom(0.8);
			break;
			
		case '-':
		case '_':
			window->zoom(1.2);
			break;
			
		case 'w':
		case 'W':
			window->translateY(1);
			break;
		
		case 's':
		case 'S':
			window->translateY(-1);
			break;
		
		case 'a':
		case 'A':
			window->translateX(-1);
			break;
		
		case 'd':
		case 'D':
			window->translateX(1);
			break;
			
		//Here we define the different fractal modes
		case '1':
			model->fractalMode = Model::MANDELBROT_MODE;
			window->setBounds(-2.2f, 0.8f, -1.5f, 1.5f);
			break;
		case '2':
			model->fractalMode = Model::JULIA_MODE;
			//Now setup the julia constants for this specific julia set fractal
			model->juliaReal = randomFractals[(model->selectedJulia % randomFractalsSize)].first;
			model->juliaImaginary = randomFractals[(model->selectedJulia % randomFractalsSize)].second;	
			window->setBounds(-1.7f, 1.2f, -1.5f, 1.5f);
			break;
		case '3':
			model->fractalMode = Model::JULIA_SIN_MODE;
			//Now setup the julia constants for this specific julia set fractal
			model->juliaReal = randomFractals[(model->selectedJulia % randomFractalsSize)].first;
			model->juliaImaginary = randomFractals[(model->selectedJulia % randomFractalsSize)].second;	
			window->setBounds(-1.7f, 1.2f, -1.5f, 1.5f);
			break;
			
			//Now generate a random real and imaginary constant component, and then display that random Julia Set. Note most of these are fairly boring, but after several interations you should see some interesting fractals.
		case '4':
			model->fractalMode = Model::JULIA_MODE;
			model->juliaReal = LO + (float)rand()/((float)RAND_MAX/(HI-LO));
			model->juliaImaginary = LO + (float)rand()/((float)RAND_MAX/(HI-LO));
			window->setBounds(-1.7f, 1.2f, -1.5f, 1.5f);
			cout << "Using constants, real:" << model->juliaReal << " imag:" << model->juliaImaginary << endl;
			break;
		
		//Go to the next item in the array of Julia sets
		case ']':
			model->selectedJulia++;
			model->juliaReal = randomFractals[(model->selectedJulia % randomFractalsSize)].first;
			model->juliaImaginary = randomFractals[(model->selectedJulia % randomFractalsSize)].second;	
			window->setBounds(-1.7f, 1.2f, -1.5f, 1.5f);
			break;
			
		//Go to the previous item in the array of Julia sets
		case '[':
			model->selectedJulia--;
			model->juliaReal = randomFractals[(model->selectedJulia % randomFractalsSize)].first;
			model->juliaImaginary = randomFractals[(model->selectedJulia % randomFractalsSize)].second;	
			window->setBounds(-1.7f, 1.2f, -1.5f, 1.5f);
			break;
			
		//Interesting julia sets
		//real:-0.692048 imag:-0.253816
		//real:-1.03884 imag:0.214964
		//real:-0.743621 imag:-0.0344564 //swirly awesome fractal
		//real:-0.756475 imag:-0.0707046
		//real:-0.179884 imag:0.689243 //triple swirl
		//real:0.391465 imag:0.349051 //palm trees
		//real:-0.232621 imag:-0.653532 //julia spider
		//real:-0.511001 imag:0.608968 //
		//real:-1.34819 imag:-0.0665871 //fire pulse
		//real:-0.745417 imag:-0.226992 //galactic
		//real:0.349831 imag:-0.385603 //fireworks
		//real:0.316808 imag:-0.413749  //dragon heads
		//real:-1.31481 imag:0.0579096 

			
			
		//Hitting 'R' will reset the zoom and translate back to default values
		case 'r':
		case 'R':
			window->setBounds(-2.2f, 0.8f, -1.5f, 1.5f);
			break;
			
	}
	
	glutPostRedisplay();
}

//Also allow the use of the arrow keys to navigate the fractals
void specialFunction(int key, int x, int y) { // function to handle special key pressing
	
	switch(key) {
		case GLUT_KEY_LEFT:
			window->translateX(-1);
			break;
		case GLUT_KEY_RIGHT:
			window->translateX(1);
			break;
		case GLUT_KEY_UP:
			window->translateY(1);
			break;
		case GLUT_KEY_DOWN:
			window->translateY(-1);
			break;
			
	}
	
	glutPostRedisplay();
	
}

void mouseFunction(int button, int state, int x, int y) {
	if(state == GLUT_DOWN) {
		window->recenter(x,window->getHeight()-y);
		glutPostRedisplay();
	}
}

//Pretty standard glut initialization code.
void init() {
	//First get the PaintModel instance so we can start setting values
	Model::getInstance(model);
	
	createPalette();
	
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	
	GLsizei windowX = (glutGet(GLUT_SCREEN_WIDTH)-model->initialWidth)/2;
	GLsizei windowY = (glutGet(GLUT_SCREEN_HEIGHT)-model->initialHeight)/2;
	glutInitWindowPosition(windowX, windowY);
	glutInitWindowSize(model->initialWidth, model->initialHeight);
	model->windowID = glutCreateWindow("Mandelbrot Zoom");
	
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glViewport (0, 0, (GLsizei) model->initialWidth, (GLsizei) model->initialHeight);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, model->initialWidth, 0, model->initialHeight, ((GLfloat)-1), (GLfloat)1);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	//Initialize the WindowController
	model->window = new WindowController(model->initialWidth,model->initialHeight,-2.2f, 0.8f, -1.5f, 1.5f);
	window = model->window;

	// set the event handling methods
	glutDisplayFunc(repaint);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyFunction);
	glutSpecialFunc(specialFunction);
	glutMouseFunc(mouseFunction);
}

//****************************************
int main(int argc, char** argv) {
	glutInit(&argc, argv);

	init();

	glutMainLoop();

	return 0;
}
