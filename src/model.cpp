#include "model.h"


// Global static pointer used to ensure a single instance of the class.
Model* Model::instance = NULL; 
	
/** This function is called to create an instance of the class.
    Calling the constructor publicly is not allowed. The constructor
    is private and is only called by this Instance function.
*/
void Model::getInstance(Model *model) {
	if (!instance)   // Only allow one instance of class to be generated.
	   instance = new Model;
	model = instance;
}