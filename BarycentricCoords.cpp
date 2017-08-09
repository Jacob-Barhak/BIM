#include "stdafx.h"
#include "BarycentricCoords.h"




// Top level function that uses Barycentric coordinates on a face.
XVecf FindNearestPointOnFace(XVecf O, XVecf A, XVecf B, XVecf C, bool& OnFace, bool& Normal_reversed)
{
	XVecf Coords3D[4];
	XVecf Coords2D[4];
	XVecf ptOnFace;
	XVecf temp(0,0,0);

	Coords3D[0] = O;
	Coords3D[1] = A;
	Coords3D[2] = B;
	Coords3D[3] = C;

	Coords2D[0] = Coords2D[1] = Coords2D[2] = Coords2D[3] = temp;

	// Convert to 2D coordinates
	Normal_reversed = ConvertTo2D(Coords3D, Coords2D);
	// Determine if point is inside the face
	OnFace = DetermineIfPointInsideFace(ptOnFace, Coords2D, Coords3D);

	return ptOnFace;
}

// Takes two arrays of vectors by reference and converts them to 2D coords.
bool ConvertTo2D(XVecf threeDim[4], XVecf twoDim[4])
{
	XVecf Normal;
	XVecf AO;
	XVecf u; // Axis in the 2D plane containing our face
	XVecf v; // Axis in the 2D plane containing our face, perpindicular to v
	XVecf AB;
	XVecf AC;
	bool normal_reversed = false;

	AB = threeDim[2] - threeDim[1];
	AC = threeDim[3] - threeDim[1];
	AO = threeDim[0] - threeDim[1];
	Normal = AB.cross(AC);

	// Make the normal face out if we have to.
	if ( AO.dot(Normal) < 0 )
	{
		normal_reversed = true;
		Normal = -Normal;
	}
	
	// Compute 2D axes
	u = AB;
    v = Normal.cross(AB);
    // Normalize axes
	u = u / (pow((double)u.dot(), 0.5));
	v = v / (pow((double)v.dot(), 0.5));

	// Convert to 2D coords
	for (int i = 0; i < 4; i++)
	{
		double x, y;
		x = threeDim[i].dot(u);
		y = threeDim[i].dot(v);
		XVecf temp(x,y,0.0);

		twoDim[i] = temp;
	}

	return normal_reversed;
}

// Determines if the point is inside the triangle.
bool DetermineIfPointInsideFace(XVecf& ptOnFace, XVecf baryCen[4], XVecf Coords3D[4])
{
	double u;
	double v;
	double w;
	double areaABC;
	double areaAOB;
	double areaAOC;
	double areaCOB;
	bool insideFace = false;

	XVecf AO;
	XVecf BO;
	XVecf CO;
	XVecf AC;
	XVecf AB;
	XVecf CB;

	AO = baryCen[0] - baryCen[1];
	BO = baryCen[0] - baryCen[2];
	CO = baryCen[0] - baryCen[3];
	AC = baryCen[3] - baryCen[1];
	AB = baryCen[2] - baryCen[1];
	CB = baryCen[3] - baryCen[2];
	
	// Calculate area of the triangles
	areaABC = 0.5 * pow((double)(AB.cross(AC)).dot(),0.5);
	areaAOB = 0.5 * pow((double)(AB.cross(AO)).dot(),0.5);
	areaAOC = 0.5 * pow((double)(AC.cross(AO)).dot(),0.5);
	areaCOB = 0.5 * pow((double)(CB.cross(CO)).dot(),0.5);

	// Calculate u,v,w to determine if point is inside the triangle.
	u = areaCOB / areaABC;
	v = areaAOC / areaABC;
	w = areaAOB / areaABC;

	// A quick check to see what is going on.
	//TRACE("u,v,w coeffs: %f, %f, %f\n", u, v, w);
	
	// if any of the coefficients are outside the acceptable range, check edges.
	if ( u <= 0 || u >= 1 || 
		 v <= 0 || v >= 1 ||
		 w <= 0 || w >= 1)
	{
		double u;
		
		// check all three edges in turn, if the point is on an edge, stop.
		if (CheckEdges(baryCen[0], baryCen[1], baryCen[2], u))
		{
			insideFace = true;
			ptOnFace = (1-u)*Coords3D[1] + u*Coords3D[2];
		}
		else if(CheckEdges(baryCen[0], baryCen[1], baryCen[3], u))
		{
			insideFace = true;
			ptOnFace = (1-u)*Coords3D[1] + u*Coords3D[3];
		}
		else if(CheckEdges(baryCen[0], baryCen[2], baryCen[3], u))
		{
			insideFace = true;
			ptOnFace = (1-u)*Coords3D[2] + u*Coords3D[3];
		}
	}
	else
	{
		// Else calculate point on the face and return.
		ptOnFace = u * Coords3D[1] + v * Coords3D[2] + w * Coords3D[3];
		insideFace = true;
	}

	return insideFace;
}

// Checks an edge of the face to determine if the point is on the edge.
// Checks if the point is actually projected onto the edge of the face in 2D.
// Not if a project point onto the edge is closer than the vertex.
bool CheckEdges(XVecf O, XVecf A, XVecf B, double& u)
{
	bool onEdge = true;
	XVecf AO;
	XVecf AB;

	// Calculate vectors
	AO = O - A;
	AB = B - A;

	// calculate u
	u = (AO.dot(AB)) / (pow((double)AB.dot(), 0.5));

	// check bounds of u
	if (u < 0 || u > 1)
	{
		onEdge = false;
	}
	else if (AO.cross(AB).dot() != 0)
	{
		onEdge = false;
	}

	return onEdge;
}

// Project the point onto the edges of the face, disregarding normals.
XVecf ProjectOntoEdges(XVecf O, XVecf A, XVecf B, bool& onEdge)
{
	XVecf AO;
	XVecf AB;
	XVecf ptOnEdge;

	AO = O - A;
	AB = B - A;

	//projection of O onto AB.
	double u = (AO.dot(AB)) / (pow((double)AB.dot(), 0.5));

	//Check if we are on the edge.
	if (u < 0 || u > 1)
	{
		onEdge = false;
	}
	else
	{
		onEdge = true;
	}
	
	// Return the projected pt.
	ptOnEdge = A + AB*u;

	return ptOnEdge; 
}