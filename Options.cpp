/************************************************

	This file contains the implementation for the
	Options class
	In this class, we have the following functions:

	default constructor
	destructor

*************************************************/

#include "stdafx.h"
#include "Options.h"



/***********************************************/
/*
	TO DO:
	add description
	add comments
							modified by:
							date:
*/
/***********************************************/
// Default constructor
Options::Options()
{
	// Setting default ICP algorithm.
	ICP_2ringfaces = BST_UNCHECKED;
	ICP_epsilon = 0.001;
	ICP_num_iterations = 200;
	ICP_num_nearest_neighbor = 1;
	ICP_std_dev_filter = 3;
	ICP_normal_agreement_filter=0.5;


	SmoothStep = sqrt(sqrt(2.0));
	SmoothingLevels = 12;
	SmoothStartRadius = 0.0075;
	IncludeBoundaryFeatures = false;
    FeatureRadius = 6.0;
	UseFeatureRadiusForNeighborhood = true;

	AcceptAlignmentThreshold = 0.01;
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
Options::~Options()
{
}
