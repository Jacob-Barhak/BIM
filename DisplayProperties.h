/************************************************

	This file is the header file for 
	DisplayProperties class

*************************************************/


#ifndef DISPLAYPROPERTIES_H
#define DISPLAYPROPERTIES_H


#include <string>
#include "BBox.h"

struct ColorToolBarOptions
{
	double min,max;
};

class DisplayProperties
{

	public:
	
	// Default constructor
	DisplayProperties();
	
	// Constructor with arguments
	// This will set the display properties based on a certain value
	// This value has yet to be defined
	DisplayProperties(int value);

	// Destructor
	~DisplayProperties();

	// Function to read display properties to a file
	void ReadDisplayProperties(string BIMFile);

	// Function to write display properties to a file
	void WriteDisplayProperties(string BIMFile);

	// member variables
	ColorToolBarOptions DisplayPropertiesColorToolBarOptions;
	bool MaterialColor;
	double PickedMaterialColor[4];
	int DrawMode;
	BBox DisplayPropertiesBBox;

	double PointSize;
	int PointType;

};

#endif
