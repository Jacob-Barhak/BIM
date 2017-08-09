/************************************************

	This file is the header file for the
	SampledPoints class

*************************************************/

#ifndef SAMPLEDPOINTS_H
#define SAMPLEDPOINTS_H


#include <vector>
#include <string>
#include "SampledPoint.h"


class SampledPoint;


//SampledPoints is a collection of SampledPoint
class SampledPoints
{

	public:
	
	// Default constructor
	SampledPoints();

	// Destructor
	~SampledPoints();


	
	
	// function to draw all sample points
	void Draw();
	
	
	// Function to calculate distance of sampled points to mesh
	void CalculateDistance(Mesh* BIMMesh);

	// Function to caluclate bounding box
	void CalculateBoundingBox();
	
	// Function to calculate max distance
	void CalculateMaxDistance();
	
	// Function to calculate min distance
	void CalculateMinDistance();
	
	
	
	// ************************************ //
	// FUTURE IMPLEMENTATION
	
	// Function to extract/filter points
	// from raw measurements obtained
	void ExtractPointsFromRawMeasurements();
	
	// Function to read SampledPoints into the
	// BIMWorkspace given a file pointer
	void ReadSampledPoints(string BIMFile);

	// Function to write SampledPoints into 
	// to a file given it's pointer
	void WriteSampledPoints(string BIMFile);
	
	

	// member variables
	vector<SampledPoint> BIMSampledPoints;

};

#endif

