#ifndef REGISTRATION
#define REGISTRATION

#include "stdafx.h"
#include "xvec.h"
#include "PartMeasurement.h"
#include "BIMWorkspace.h"

#include "BaryCentricCoords.h"
#include "simplemesh.h"
#include "PointClouds.h"
#include <deque>

struct point_info
{
	double max_distance;
	double min_distance;
	XVecf nearest_pt;
	XVecf furthest_pt;
	int min_index;
	int max_index;
	SimpleMeshT::VertexCt::iterator near_it;
	SimpleMeshT::VertexCt::iterator far_it;
};

struct mesh_point_info
{	
	int valence;
	XVecf point;
	vector<XVecf> adjacent_positions;

};


enum {DIMENSION=3, RESULTS=1, MEASURE = 2, ICP = 4};

void CalculateDistances(PartMeasurement& point_cloud, SimpleMeshT* BIMMesh, Options BIMOptions, CWnd* myWin);
// function to run ICP algorithm
bool RunICP(PartMeasurement& PointCloud , PartMesh& Part, CWnd* myWin, Options BIMOptions);

bool RunAutoAlign(PartMeasurement& ptCloud, PartMesh& Part, CWnd* myWin, Options BIMOptions);




// **************************
// below are helper functions

// function to find closest vertex on a mesh from a point cloud
XVecf FindClosestPoint(SampledPoint point, SimpleMeshT* BIMMesh);

//??????
// Function to project the sample point onto the mesh?
void Projection();
//??????

// Function to construct the matrix H
float* ConstructHMatrix(vector<XVecf> ptCloud, vector<XVecf> nearestPts);

XVecf* FindQi(vector<XVecf> pts);

XVecf FindAverage(vector<XVecf> points);

double CalculateError(float* TransformMatrix, vector<XVecf> nearestPts, vector<XVecf>transformed_pointcloud);

// Use single value decomposition (SVD) on the matrix H
float* DecomposeHMatrix(float* HMatrix);

// Function to calculate the determinant of a matrix
float CalculateDet(float* RMatrix);

// Function to calculate the transformation matrix, T
float* CalculateT(float* rotationMatrix, XVecf p, XVecf pprime);

// Function to apply the transformation T to the 
// part measurement
void ApplyTransform(PartMeasurement* PointCloud);

// Function that returns the projected point
// given a vertex and point we want to project
XVecf ProjectOnFacesAroundVertex(XVecf vertex, XVecf point_to_project);

void FindNearestPointToFace(XVecf point/*,Face face*/);

void FindNearestPointToEdge(XVecf point, XVecf A, XVecf B);

void CalculateBarycentricCoords(XVecf &u, XVecf &v, XVecf &w, XVecf point, XVecf A, XVecf B, XVecf C);

double CalculateErrorRatio(double current_error,double previous_error);

// Calculate transform matrix for doing 3 point select method of moving.
float* ThreePtSelectAndXForm(deque<XVecf> pairOne, deque<XVecf> pairTwo);

#endif