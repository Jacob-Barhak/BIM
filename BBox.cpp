/************************************************

	This file contains the implementation for the
	BBox class.
	In this class, we have the following functions:

	default constructor
	destructor

*************************************************/


#include "stdafx.h"
#include "BBox.h"

using namespace std;

/***********************************************/
/*
	Default constructor
	Initally sets bmin to be very huge
	and sets bmax to be very small
							modified by: Jun
							date: 10/06/04
*/
/***********************************************/

BBox::BBox()
{
	// set bmin to be very big
	// and sets bmax to be very small
	bmin = XVecf(1e15);
    bmax = XVecf(-1e15);
}


/***********************************************/
/*
	Destructor
							modified by: Jun
							date: 10/06/04
*/
/***********************************************/

BBox::~BBox()
{
}

// calculate diagonal of the bounding box
// by Jacob 9/30/2005
double BBox::DiagSize()
{ 
	XVecf Diagonal;
	Diagonal = bmax-bmin;
	return (Diagonal.l2());
}

