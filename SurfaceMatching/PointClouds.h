#pragma once


#include <vector>
#include "ANN.h"
#include "basePoints.h"


class PointClouds
{
public:
	PointClouds(void);
	~PointClouds(void);
	PointClouds(const PointClouds& _pc);
	PointClouds& operator=(const PointClouds& _pc);
//	{
//		...
//		return *this;
//	}

public:		// methods
	bool BuildKDTree( int N, float*, float *, bool has_normal );
	void SetCameraData(XVecf p, XVecf d);
	bool ComputeNormal();
	float ComputeRadius( );
	void FindBoundary();
	void SmoothPointCloud();
	void SmoothOnePoint(int i, int l);
	void FindLocalMaxMin(int i);
	void ComputeCurvatures( );
	bool MatchTagent( PointClouds *PC1, float scale, float [3][4]);
	float NearestNg(ANNpoint p);
	void SaveMatchTagent(char *);
	bool MatchTagentFromFile(char *fname, float scale, float[3][4]);
	void SetLevel(int l){
		m_level = l;
	}
	int GetLevel( ){
		return m_level;
	}
	int GetNumOfExtrema(){
		return numOfExtrema;
	}
	float GetSmoothRadius(){
		return (m_radius);
	}
	bool SetStartRadius(float _radius){
		if ( _radius <= 0 )
			return false;
		else
			startRadius = _radius;
		return true;
	}
	bool SetSmoothStep(float _step){
		if ( _step <= 1 ){
			smooth_step = _step;
			return true;
		}else
			return false;
	}
	bool SetFeatureRadius(bool _isradius, float _radius){
		if (_isradius){
			isFeatureRadius = true;
			featureRadius = _radius;
		} else if ( _radius < 5 ){
			return false;
		} else{
			isFeatureRadius = false;
			featureRadius = _radius;
			return true;
		}
	}
	bool IfSmoothed();
// temp use
	void SaveToObjFile();
	float GetScale();
	// for curvature
//	void CurvaturePrinceton();
//	void SaveMatchCurvature(char *);
//	bool MatchCurvature(char *fname, float scale, float[3][4]);
	XVecf ComputeTagentPlane(XVecf N);

private:
	float MedianOfArray(float * farray, int n);

public:
	int NumOfPoints;
	float m_radius, m_radius2;
	int dim;
	XVecf CamCenter, CamDir;
	std::vector<BasePoints> PointsCloud;
	ANNpointArray	m_data;     // store the coordinates
	ANNkd_tree      *m_kdtree;  // the kd_tree
	ANNdistArray    m_dists;    // an array for storing the result of kd_tree search, the index of m_data
	ANNidxArray     m_nidx;     // the distance info. of the searching result, dis^2;
	float m_eps;
	std::vector<float> dists;
	std::vector<int> idx;

public:	// for smoothing
	const static int maxLevels = 50;
	int		m_level;
	float *	level_radius;
	float	smooth_step;
	float	accuracy;
	// after smoothing, compute the tagent direction
	float * minmax;
	XVecf * TDirection;
	XVecf * Normals;
	XVecf * Positions;
	float * mCurvatures;	
 //private:
	int	numOfExtrema;
	bool	is_smooth;
	float startRadius;
	bool smoothStep;
	bool isFeatureRadius;
	float featureRadius;
};
