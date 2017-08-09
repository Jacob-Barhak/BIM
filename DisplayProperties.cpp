/************************************************

	This file contains the implementation for the
	DisplayProperties class.
	In this class, we have the following functions:

	default constructor
	constructor with an argument
	destructor
	read display properties from a file
	write display properties to a file

*************************************************/

#include "stdafx.h"
#include "DisplayProperties.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/***********************************************/
/*
							modified by: Jun
							date: 01/21/05
*/
/***********************************************/
// Default constructor
DisplayProperties::DisplayProperties()
{
	PointSize = 0.03;
	PointType = 2;
	MaterialColor = false;
}
	


/***********************************************/
/*
	TO DO:
	add description
	add comments
							modified by:
							date:
*/
/***********************************************/
// Constructor with arguments
// This will set the display properties based on a certain value
// This value has yet to be defined
DisplayProperties::DisplayProperties(int value)
{
}


/***********************************************/
/*
	TO DO:
	add description
	add comments
							modified by:
							date:
*/
/***********************************************/
// Destructor
DisplayProperties::~DisplayProperties()
{
}



/***********************************************/
/*
	TO DO:
	add description
	add comments
							modified by:
							date:
*/
/***********************************************/
// Function to read display properties to a file
void DisplayProperties::ReadDisplayProperties(string BIMFile)
{
}




/***********************************************/
/*
	TO DO:
	add description
	add comments
							modified by:
							date:
*/
/***********************************************/
// Function to write display properties to a file
void DisplayProperties::WriteDisplayProperties(string BIMFile)
{
}
