//------------------------------------------
//	3DPoint.h
//
//	Desc: These 2 classes describe 2 
//		  3D point types.  One is a 3D 
//		  crosshatch of lines and the other
//		  is a tetrahedron.
//------------------------------------------
#include "stdafx.h"

using namespace std;

#ifndef POINT3D_H
#define POINT3D_H

////////////////////////////////////////////
// Class CrossHatchPoint
//
// Desc: A class that defines a point
//       by 3 intersecting lines.
//
// Modified: Geoff Blake 9/23/04
////////////////////////////////////////////
class CrossHatchPoint
{
protected:
	GLdouble points[6][3];

public:
	// default constructor
	CrossHatchPoint();
	// can set the diameter of the point
	CrossHatchPoint(double size);

	// Draws point, translated by x,y,z
	void Draw(double x, double y, double z);
};

////////////////////////////////////////////
// Class TetraHedronPoint
//
// Desc: A class that defines a point
//       with one tetrahedron on top of each
//       other.
//
// Modified: Geoff Blake 9/23/04, Jun 01/17
////////////////////////////////////////////
class TetraHedronPoint
{
protected:
	GLdouble points[6][3];


public:
	// default constructor
	TetraHedronPoint();
	// can set diameter of the point
	TetraHedronPoint(double size);

	// Draws point, translated by x,y,z
	void Draw(double x, double y, double z);
};


////////////////////////////////////////////
// Class DoubleTetraHedronPoint
//
// Desc: A class that defines a point
//       with two tetrahedron on top of each
//       other.
//
// Modified: Jun 01/17/05
////////////////////////////////////////////
class DoubleTetraHedronPoint
{
protected:
	GLdouble points[6][3];


public:
	// default constructor
	DoubleTetraHedronPoint();
	// can set diameter of the point
	DoubleTetraHedronPoint(double size);

	// Draws point, translated by x,y,z
	void Draw(double x, double y, double z);
};

#endif