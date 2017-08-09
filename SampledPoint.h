/************************************************

	This file is the header file for the
	SampledPoint class

*************************************************/



#ifndef SAMPLEDPOINT_H
#define SAMPLEDPOINT_H


#include "Mesh.h"
#include "xvec.h"


class SampledPoint
{

	public:
	
	// Default Constructor
	SampledPoint();
	
	// Destructor
	~SampledPoint();

	// Function to read sampled point
	void ReadSampledPoint(string BIMFile);
	
	// Function to write sampled point
	void WriteSampledPoint(string BIMFile);
	
	// Function to calculate distance between 
	// sampled point and mesh
	void CalculateDistance(Mesh* BIMMesh);
	

	// member variables
	int index;
	bool highlight;
	double Coords[4];
	double dist;
	double SNR;
	double Total;
	double x;
	double y;
	double z;
	double Optimet;
	double ColorAttributes[4];
	bool PointValid;
	XVecf normal;
	
};

#endif
