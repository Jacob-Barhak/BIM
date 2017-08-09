/************************************************

	This file is the header file for the
	PartMeasurements class

*************************************************/


#ifndef PARTMEASUREMENTS_H
#define PARTMEASUREMENTS_H


#include <vector>
#include "PartMeasurement.h"
#include "xvec.h"


class PartMeasurements
{
	
	public:
	
	//Function that calculates the bounds for all the PartMeasurements
	void CalculatePartMeasurementsBounds();

	// Function that will update the display properties for all part measurements
	void UpdateDisplayProperties();
	
	// accessor function to bmin
	XVecf GetMinBound(){return bmin;}

	// accessor funtion to bmax
	XVecf GetMaxBound(){return bmax;}

	// Function to delete a PartMeasurement based on its name
	// from our collection of PartMeasurements
	void DeletePartMeasurement(int name);

	// Function to update the global color bar
	// for all part measurements
	void UpdatePartMeasurementsColorBar();
	
	DisplayProperties PartMeasurementsDisplayProperties;
	// member variables

	void SaveScans(CWnd* myWin);

	// collection of PartMeasurement
	vector<PartMeasurement> BIMPartMeasurements;
	XVecf bmin,bmax;
	
	

};

#endif
