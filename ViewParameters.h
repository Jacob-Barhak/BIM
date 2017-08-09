/************************************************

	This file is the header file for the
	ViewParameters class

*************************************************/


#ifndef VIEWPARAMETERS_H
#define VIEWPARAMETERS_H





class ViewParameters
{

	public:
	
	// Default constructor
	ViewParameters();
	
	// Destructor
	~ViewParameters();
	
	int ViewPoint[4];
	int LookAtPoint[4];
	int UpVector[4];
	int LightPosition[4];
	bool Ortographic;
	bool Perspective;
	
};

#endif
