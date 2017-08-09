/************************************************

	This file is the header file for the
	PartMeasurement class

*************************************************/


#ifndef PARTMEASUREMENT_H
#define PARTMEASUREMENT_H

#include <string>
#include "SampledPoints.h"
#include "TransMatrix.h"
#include "DisplayProperties.h"
#include "Flags.h"
#include "xvec.h"
#include "PointClouds.h"

typedef struct
{
	XVecf ViewPoint;
	XVecf ViewDir;
} PartMeasurement_POV;

template<class real_t> 
class XVecTn
{
public:
	XVecT<real_t> pt;
	bool rn;
};

typedef XVecTn<float> XVecfn;

class PartMeasurement
{

	
	public:
	
	// Default constructor , - set matrix to unit matrix
	PartMeasurement();

	// Destructor
	~PartMeasurement();

	// Copy Constructor
	PartMeasurement(const PartMeasurement& rhs);
	
	// Function to read part measurement from a file
	// and populates the SampledPoints
	bool ReadPartMeasurement(string BIMFile);

	// Function to write part measurement to a file
	void WritePartMeasurement(string BIMFile);

	// Function to draw part measurement, also has ability to highlight
	void Draw();
	
	// Function to set the flags and reset the from the mesh
	void ResetPropAfterTrans(enum value);

	// Function to run ICP algorithm for registering the part measurement
	// to the mesh
	void RegisterByICP(Mesh* BIMMesh);

	// Function to run PCA algorithm for registering the part measurement
	// to the mesh
	void RegisterByPCA(Mesh* BIMMesh);
	
	// Function to calculate color map for the
	// PartMeasuremnt
	void CalculateColorMap(int VALUE/*total,snr,etc.*/);
	
	// Function to apply color map
	void ApplyColorMap();

	// Function to calculate bouding box for PartMeasurement
	void CalculateBoundingBox();

	// Calculate center of mass of point cloud for moving it around
	XVecf CalculateCenterOfMass();

	// Set Nearest points to CAD model and their normals
	void SetClosestPoints(vector<XVecfn> closestPts, vector<XVecfn> closestPtsNrml);

	// Update distances
	void UpdateDistances();
	// TBD
	// Function that returns the range of the color map
	// return type needs to be defined
	// ? GetColorMapRange();
	
	// Function to update the color bar for the part measurement
	void UpdatePartMeasurementColorBar();
	
	// Calculates points std dev.  This is so we can filter out points.
	void CalculateStdDev();

	// This function claculates the transformed point cloud and the nearest points after filtering for Normals and StdDev
	// These points are returned in the RetTransPoints and the RetClosestPoints variables. 
	// These vercors are assumed to be empty.
	// The function returns the number of filtered points.
	int FilterPointtsForICP ( vector<XVecf> &RetTransPoints, vector<XVecf> &RetClosestPoints);

	// Exports the transformed data points to a file
	void ExportTransformedDataPts(int i);

	// Calculate Normals
	void CalculateNormals();

	// member variables
	SampledPoints PartMeasurementSampledPoints;
	TransMatrix PartMeasurementTransMatrix;
	DisplayProperties PartMeasurementDisplayProperties;
	PartMeasurement_POV pov;
	Flags PartMeasurementFlags;

	

	XVecf middle;
	vector<XVecfn> closestPoints;
	vector<XVecfn> closestPointsNormal;
	vector<double> distances;
	vector<double> NormalAgreement;
	vector<double> StdDevToFilerRatios;
	double std_dev_filter;
	double NormalAgreementFilter;
	double DistAvg; // average of all distances (should be around zero usually)
	double DistStd; // standard deviation of all point distances

	PointClouds* PC;

	string FileName;
	string Comments;
	

	// // clears the closet points and propogates the update for distances and all other variables
	void ClearClosestPointsAndUpdate(void);
	void InitPointCloud(void);
};	

#endif
