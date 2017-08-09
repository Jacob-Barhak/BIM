/************************************************

	This file is the header file for the BBox 
	class

*************************************************/


#ifndef BBOX_H
#define BBOX_H


#include "xvec.h"

class BBox
{
	public:
		
	// Default constructor, sets default values for member variables
	BBox();

	// Destructor
	~BBox();

	// calculate diagonal
	double DiagSize(); 

	// member variables
	XVecf bmin,bmax;

};

#endif
