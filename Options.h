/************************************************

	This file is the header file for the
	Options class

*************************************************/

#ifndef OPTIONS_H
#define OPTIONS_H


///Currently the options class is not implemented, instead several sutrctures are used
// TODO - migrate all options to the options class

typedef struct
{
	int ICP_num_iterations;
	double ICP_epsilon;
	double ICP_std_dev_filter; //a constant used to filer points
	int ICP_num_nearest_neighbor;
	int ICP_2ringfaces;
}ICPOptions;

typedef struct
{
	int smooth_levels;
	double scale_level;
}AutoAlignOptions;



class Options
{
	public:

	// Default constructor
	Options();

	// Destructor
	~Options();

	int ICP_num_iterations;
	double ICP_epsilon;
	double ICP_std_dev_filter;
	double ICP_normal_agreement_filter;
	int ICP_num_nearest_neighbor;
	int ICP_2ringfaces;

	int SmoothingLevels;
	double SmoothStep;
	double SmoothStartRadius;
	bool IncludeBoundaryFeatures;
    double FeatureRadius;
	bool UseFeatureRadiusForNeighborhood;

	double AcceptAlignmentThreshold; // compare to DistanceSTD/BBox diagonal
	
};

#endif
