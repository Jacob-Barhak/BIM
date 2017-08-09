/***********************************************************/
// BarycentricCoords.h
// Defines 
//
//
/***********************************************************/

#ifndef BARYCENTRICCOORDS_H
#define BARYCENTRICCOORDS_H

#include "xvec.h"
#include "XVecdim.h"

// Top level function that uses Barycentric coordinates on a face.
XVecf FindNearestPointOnFace(XVecf O, XVecf A, XVecf B, XVecf C, bool& inFace, bool& normal_reversed);

// Takes two arrays of vectors by reference and converts them to 2D coords.
bool ConvertTo2D(XVecf threeDim[4], XVecf twoDim[4]);

// Determines if the point is inside the triangle.
bool DetermineIfPointInsideFace(XVecf& ptOnFace, XVecf baryCen[4], XVecf Coords3D[4]);

// Checks an edge of the face to determine if the point is on the edge.
bool CheckEdges(XVecf O, XVecf A, XVecf B, double& u);

// Projects the point onto an edge of the face, disregarding normals.
XVecf ProjectOntoEdges(XVecf O, XVecf A, XVecf B, bool& onEdge);

#endif