/************************************************

	This file contains the implementation for the
	SampledPoint class.
	In this class, we have the following functions:

	default constructor
	destructor
	read sample point from file
	write sample point to file
	calculate distances to mesh
*************************************************/

#include "stdafx.h"
#include "SampledPoint.h"


/***********************************************/
/*
	TO DO:
	add description
	add comments
							modified by:
							date:
*/
/***********************************************/
// Default Constructor
SampledPoint::SampledPoint()
{
	x = 0.0;
	y = 0.0;
	z = 0.0;
	SNR = 0.0;
	// default to blue
	ColorAttributes[0] = 0.0;
	ColorAttributes[1] = 0.0;
	ColorAttributes[2] = 1.0;
	ColorAttributes[3] = 1.0;
	PointValid = false;
	dist = 0.0;
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
SampledPoint::~SampledPoint()
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
// Function to read sampled point from a file
void SampledPoint::ReadSampledPoint(string BIMFile)
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
// Function to write sampled point to a file
void SampledPoint::WriteSampledPoint(string BIMFile)
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
// Function to calculate distance between 
// sampled point and mesh
void SampledPoint::CalculateDistance(Mesh* BIMMesh)
{
}
     
